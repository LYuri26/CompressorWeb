#include "ligadesliga.h" // Biblioteca para funções de ligar e desligar o compressor
#include <NTPClient.h>   // Biblioteca para obter o tempo da Internet usando o protocolo NTP
#include <WiFiUdp.h>     // Biblioteca para comunicação UDP necessária para NTPClient
#include <FS.h>          // Biblioteca para manipulação do sistema de arquivos
#include <SPIFFS.h>      // Biblioteca para usar o sistema de arquivos SPIFFS

// Variáveis globais
bool compressorLigado = false;                                  // Flag global que indica se o compressor está ligado ou desligado
WiFiUDP ntpUDP;                                                 // Objeto para gerenciar pacotes UDP necessários para o cliente NTP
NTPClient timeClient(ntpUDP, "pool.ntp.org", -3 * 3600, 60000); // Cria um cliente NTP para obter o tempo, ajuste o fuso horário para GMT-3 e atualiza a cada 60 segundos

unsigned long previousMillis = 0; // Armazena o tempo da última atualização para controle de intervalos
const long interval = 300000;     // Intervalo de 5 minutos (em milissegundos) para atualizar o status do compressor
unsigned long lastToggleTime = 0; // Tempo da última troca de estado do compressor para debounce
bool timerAtivo = false;          // Flag para verificar se o timer está ativo

// Função para inicializar o sistema de arquivos SPIFFS
void initSPIFFS()
{
    if (!SPIFFS.begin(true))
    {                                                                                          // Inicia o SPIFFS, o parâmetro 'true' força a formatação se falhar
        Serial.println("Erro ao iniciar SPIFFS. O sistema de arquivos não pôde ser montado."); // Mensagem de erro se a inicialização falhar
        return;                                                                                // Retorna para encerrar a função em caso de erro
    }
    Serial.println("SPIFFS inicializado com sucesso."); // Mensagem de sucesso se a inicialização for bem-sucedida
}

// Função para ler o estado do compressor do arquivo
bool readCompressorState()
{
    Serial.println("Tentando ler o estado do compressor do arquivo."); // Mensagem informativa sobre a leitura do estado
    File file = SPIFFS.open("/estadocompressor.txt", "r");             // Abre o arquivo em modo leitura
    if (!file)
    {
        Serial.println("Arquivo de estado não encontrado, assumindo estado desligado."); // Mensagem de erro se o arquivo não for encontrado
        return false;                                                                    // Se o arquivo não existir, assume-se que o compressor está desligado
    }

    String state = file.readStringUntil('\n'); // Lê o conteúdo do arquivo até o caractere de nova linha
    file.close();                              // Fecha o arquivo

    bool estado = state.toInt() == 1;                                               // Retorna verdadeiro se o estado for 1 (ligado), caso contrário, falso
    Serial.printf("Estado lido do arquivo: %s\n", estado ? "Ligado" : "Desligado"); // Mensagem com o estado lido
    return estado;                                                                  // Retorna o estado do compressor
}

// Função para salvar o estado do compressor no arquivo
void saveCompressorState(bool state)
{
    Serial.printf("Salvando o estado do compressor: %s\n", state ? "Ligado" : "Desligado"); // Mensagem informativa sobre o estado que será salvo
    File file = SPIFFS.open("/estadocompressor.txt", "w");                                  // Abre o arquivo em modo escrita
    if (!file)
    {
        Serial.println("Erro ao abrir o arquivo para escrita."); // Mensagem de erro se o arquivo não puder ser aberto para escrita
        return;                                                  // Retorna para encerrar a função em caso de erro
    }

    file.println(state ? "1" : "0");                           // Escreve 1 se o compressor estiver ligado, 0 se estiver desligado
    file.close();                                              // Fecha o arquivo
    Serial.println("Estado do compressor salvo com sucesso."); // Mensagem de sucesso após salvar o estado
}

// Função para verificar se a hora atual está após o horário de fechamento
bool isAfterClosingTime()
{
    timeClient.update();                                                                                                          // Atualiza a hora do cliente NTP
    int hour = timeClient.getHours();                                                                                             // Obtém a hora atual
    bool resultado = hour >= 22;                                                                                                  // Verifica se a hora é 22 ou mais
    Serial.printf("Verificação de horário: %s\n", resultado ? "Após o horário de fechamento" : "Antes do horário de fechamento"); // Mensagem com o resultado da verificação
    return resultado;                                                                                                             // Retorna o resultado da verificação
}

// Função para verificar se a hora atual está antes do horário de abertura
bool isBeforeOpeningTime()
{
    timeClient.update();                                                                                                      // Atualiza a hora do cliente NTP
    int hour = timeClient.getHours();                                                                                         // Obtém a hora atual
    bool resultado = hour < 7;                                                                                                // Verifica se a hora é antes das 7
    Serial.printf("Verificação de horário: %s\n", resultado ? "Antes do horário de abertura" : "Após o horário de abertura"); // Mensagem com o resultado da verificação
    return resultado;                                                                                                         // Retorna o resultado da verificação
}

// Função para configurar o servidor Web para lidar com as solicitações de ligar/desligar o compressor
void setupLigaDesliga(WebServer &server)
{
    Serial.println("Configurando o servidor Web para controle do compressor."); // Mensagem informativa sobre a configuração do servidor
    initSPIFFS();                                                               // Inicializa o sistema de arquivos SPIFFS

    // Carrega o estado do compressor do arquivo
    compressorLigado = readCompressorState(); // Atualiza o estado do compressor com o valor lido do arquivo

    // Configura a rota "/toggle" para lidar com requisições GET
    server.on("/toggle", HTTP_GET, [&server]()
              {
        unsigned long currentMillis = millis(); // Obtém o tempo atual em milissegundos
        Serial.println("Requisição recebida para alternar o estado do compressor."); // Mensagem informativa sobre a requisição recebida

        // Verifica se o compressor foi ligado/desligado recentemente e impede a troca até que o intervalo de 5 minutos tenha passado
        if (compressorLigado && (currentMillis - lastToggleTime < 30000))
        {
            Serial.println("Comando ignorado. O compressor foi alterado recentemente. Aguarde 5 minutos entre as tentativas."); // Mensagem de erro se a troca for feita antes do intervalo de debounce
            server.send(200, "text/plain", "Comando ignorado. Aguarde 5 minutos entre as tentativas."); // Responde com uma mensagem informando que o comando foi ignorado
            return; // Sai da função
        }

        lastToggleTime = currentMillis; // Atualiza o tempo da última troca de estado do compressor
        compressorLigado = !compressorLigado; // Alterna o estado do compressor

        String message = compressorLigado ? "Compressor ligado!" : "Compressor desligado!"; // Mensagem dependendo do novo estado do compressor
        if (compressorLigado && (isAfterClosingTime() || isBeforeOpeningTime()))
        {
            message += " Alerta! Já se passou das 22:30 e ainda não chegou às 07:30, atente-se para desligar o compressor após o uso."; // Adiciona um alerta se estiver fora do horário permitido
            Serial.println("Alerta: Compressor ligado fora do horário permitido."); // Mensagem de alerta para o caso de estar fora do horário
        }

        Serial.print("Estado do compressor: ");
        Serial.println(message); // Imprime o status atualizado do compressor
        server.send(200, "text/plain", message); // Responde com o status do compressor

        saveCompressorState(compressorLigado); // Salva o estado do compressor no arquivo

        if (compressorLigado)
        {
            previousMillis = millis(); // Atualiza o tempo da última atualização para controle do intervalo
            timerAtivo = true; // Marca o timer como ativo
            Serial.println("Compressor ligado. Timer ativado."); // Mensagem informativa sobre o estado do compressor e o timer
        }
        else
        {
            Serial.println("Compressor desligado. Timer desativado."); // Mensagem informativa sobre o estado do compressor e o timer
        } });

    timeClient.begin(); // Inicia o cliente NTP para obter o tempo

    // Imprime o status inicial do compressor
    Serial.print("Estado inicial do compressor: ");
    Serial.println(compressorLigado ? "Ligado" : "Desligado"); // Mensagem com o estado inicial do compressor

    // Imprime a hora atual
    timeClient.update(); // Atualiza a hora do cliente NTP
    Serial.print("Hora atual: ");
    Serial.println(timeClient.getFormattedTime()); // Mensagem com a hora atual formatada

    // Verifica o horário e ajusta o estado inicial do compressor
    if (isAfterClosingTime() || isBeforeOpeningTime())
    {
        compressorLigado = false;                                                   // Desliga o compressor se estiver fora do horário permitido
        saveCompressorState(compressorLigado);                                      // Atualiza o arquivo com o novo estado
        Serial.println("Compressor desligado devido ao horário na inicialização."); // Mensagem informativa sobre a desativação do compressor
    }
    else
    {
        Serial.println("Compressor inicializado dentro do horário permitido."); // Mensagem informativa sobre a inicialização do compressor dentro do horário permitido
    }
}

// Função para atualizar o status do compressor
void updateCompressorStatus()
{
    unsigned long currentMillis = millis(); // Obtém o tempo atual em milissegundos

    if (compressorLigado && (currentMillis - previousMillis >= interval))
    {
        previousMillis = currentMillis;                  // Atualiza o tempo da última atualização
        Serial.println("Compressor status atualizado."); // Mensagem informativa sobre a atualização do status do compressor
        // Aqui você pode adicionar lógica para verificar e atualizar o status do compressor, se necessário
    }
}
