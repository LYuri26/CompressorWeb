// -------------------------------------------------------------------------
// Inclusões de Bibliotecas e Cabeçalhos
// -------------------------------------------------------------------------
#include <FS.h>                // Biblioteca para manipulação do sistema de arquivos
#include <SPIFFS.h>            // Biblioteca para o sistema de arquivos SPIFFS
#include <ESPAsyncWebServer.h> // Biblioteca para servidor web assíncrono
#include "autenticador.h"      // Cabeçalho onde a variável userLoggedIn é declarada
#include "ligadesliga.h"       // Cabeçalho para manipulação do compressor
#include "tempo.h"             // Cabeçalho para manipulação de tempo
#include "manutencao.h"

// -------------------------------------------------------------------------
// Configurações e Variáveis Globais
// -------------------------------------------------------------------------
const int pinoLigaDesliga = 2;                    // Pino do compressor
const long intervalo = 300000;                        // Intervalo de 5 minutos (300000 ms)
const String arquivoEstado = "/estadocompressor.txt"; // Arquivo para salvar o estado do compressor

bool compressorLigado = false;    // Estado atual do compressor (ligado ou desligado)
bool timerAtivo = false;          // Indicador de se o timer está ativo
unsigned long previousMillis = 0; // Tempo de referência para o controle do compressor
unsigned long lastToggleTime = 0; // Tempo da última alteração do estado

// -------------------------------------------------------------------------
// Funções de Manipulação do Compressor
// -------------------------------------------------------------------------

/**
 * Função para manipular a ação de ligar/desligar o compressor.
 *
 * @param server Instância do servidor web assíncrono.
 */
void handleToggleAction(AsyncWebServer &server)
{
    server.on("/toggle", HTTP_ANY, [](AsyncWebServerRequest *request)
              {
        // Verifica se o usuário está autenticado
        if (!isAuthenticated(request)) {
            redirectToAccessDenied(request); // Redireciona se não autenticado
            return;
        }

        // Lógica para alternar o estado do compressor com base no parâmetro "action"
        String action = request->getParam("action")->value();
        if (action == "ligar") {
            compressorLigado = true; // Liga o compressor
            request->send(200, "text/plain", "Compressor ligado!"); // Resposta ao cliente
        } else if (action == "desligar") {
            compressorLigado = false; // Desliga o compressor
            request->send(200, "text/plain", "Compressor desligado!"); // Resposta ao cliente
        } else {
            request->send(400, "text/plain", "Ação inválida!"); // Ação inválida
        } });
}

/**
 * Função para inicializar o sistema de arquivos SPIFFS.
 */
void initSPIFFS()
{
    if (!SPIFFS.begin(true))
    {
        Serial.println("Erro ao iniciar SPIFFS. O sistema de arquivos não pôde ser montado.");
        return;
    }
    Serial.println("SPIFFS inicializado com sucesso.");
}

/**
 * Função para ler o estado do compressor a partir do arquivo SPIFFS.
 *
 * @return Estado do compressor (ligado ou desligado).
 */
bool readCompressorState()
{
    Serial.println("Tentando ler o estado do compressor do arquivo.");
    File file = SPIFFS.open(arquivoEstado, "r");
    if (!file)
    {
        Serial.println("Arquivo de estado não encontrado, assumindo estado desligado.");
        return false; // Assumindo que o compressor está desligado se o arquivo não for encontrado
    }

    String state = file.readStringUntil('\n'); // Lê o estado do arquivo
    file.close();
    bool estado = state.toInt() == 1; // Converte o valor lido para booleano
    Serial.printf("Estado lido do arquivo: %s\n", estado ? "Ligado" : "Desligado");
    return estado;
}

/**
 * Função para salvar o estado do compressor no arquivo SPIFFS.
 *
 * @param state Estado atual do compressor (ligado ou desligado).
 */
void saveCompressorState(bool state)
{
    Serial.printf("Salvando o estado do compressor: %s\n", state ? "Ligado" : "Desligado");
    File file = SPIFFS.open(arquivoEstado, "w");
    if (!file)
    {
        Serial.println("Erro ao abrir o arquivo para escrita.");
        return;
    }

    file.println(state ? "1" : "0"); // Salva o estado como "1" para ligado e "0" para desligado
    file.close();
    Serial.println("Estado do compressor salvo com sucesso.");
}

// -------------------------------------------------------------------------
// Funções de Manipulação de Tempo
// -------------------------------------------------------------------------

/**
 * Função auxiliar para obter as horas a partir do formato de data e hora.
 *
 * @param time String com a data e hora.
 * @return Horas extraídas da string.
 */
int getHoursFromTime(const String &time)
{
    return time.substring(11, 13).toInt(); // Extrai as horas do formato de data e hora
}

/**
 * Função auxiliar para obter os minutos a partir do formato de data e hora.
 *
 * @param time String com a data e hora.
 * @return Minutos extraídos da string.
 */
int getMinutesFromTime(const String &time)
{
    return time.substring(14, 16).toInt(); // Extrai os minutos do formato de data e hora
}

/**
 * Função para verificar se o horário atual é após o horário de fechamento.
 *
 * @return Verdadeiro se for após o horário de fechamento, caso contrário, falso.
 */
bool isAfterClosingTime()
{
    updateTime();                          // Atualiza o tempo antes de verificar
    String time = getTimeClient();         // Obtém a hora atual
    int hour = getHoursFromTime(time);     // Obtém as horas atuais
    int minute = getMinutesFromTime(time); // Obtém os minutos atuais

    // Verifica se é após as 22:30
    bool resultado = (hour > 22) || (hour == 22 && minute >= 30);
    Serial.printf("Verificação de horário (fechamento): %s\n", resultado ? "Após 22:30" : "Antes de 22:30");
    return resultado;
}

/**
 * Função para verificar se o horário atual é antes do horário de abertura.
 *
 * @return Verdadeiro se for antes do horário de abertura, caso contrário, falso.
 */
bool isBeforeOpeningTime()
{
    updateTime();                          // Atualiza o tempo antes de verificar
    String time = getTimeClient();         // Obtém a hora atual
    int hour = getHoursFromTime(time);     // Obtém as horas atuais
    int minute = getMinutesFromTime(time); // Obtém os minutos atuais

    // Verifica se é antes das 07:30
    bool resultado = (hour < 7) || (hour == 7 && minute < 30);
    Serial.printf("Verificação de horário (abertura): %s\n", resultado ? "Antes de 07:30" : "Após 07:30");
    return resultado;
}

// -------------------------------------------------------------------------
// Funções de Configuração do Servidor Web
// -------------------------------------------------------------------------

/**
 * Função para configurar o servidor Web para controle do compressor.
 *
 * @param server Instância do servidor web assíncrono.
 */
void setupLigaDesliga(AsyncWebServer &server)
{
    Serial.println("Configurando o servidor Web para controle do compressor.");
    initSPIFFS(); // Inicializa o sistema de arquivos SPIFFS

    pinMode(pinoLigaDesliga, OUTPUT);                             // Configura o pino do compressor como saída
    compressorLigado = readCompressorState();                     // Lê o estado do compressor do arquivo
    digitalWrite(pinoLigaDesliga, compressorLigado ? HIGH : LOW); // Define o estado inicial do compressor

    server.on("/toggle", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        unsigned long currentMillis = millis(); // Obtém o tempo atual
        Serial.println("Requisição recebida para alternar o estado do compressor.");

        // Verifica se o compressor foi alterado recentemente e evita comandos repetidos
        if (compressorLigado && (currentMillis - lastToggleTime < 30000)) {
            Serial.println("Comando ignorado. O compressor foi alterado recentemente. Aguarde 30 segundos entre as tentativas.");
            request->send(200, "text/plain", "Comando ignorado. Aguarde 30 segundos entre as tentativas.");
            return;
        }

        lastToggleTime = currentMillis; // Atualiza o tempo da última alteração
        compressorLigado = !compressorLigado; // Alterna o estado do compressor
        digitalWrite(pinoLigaDesliga, compressorLigado ? HIGH : LOW); // Atualiza o pino do compressor

        // Mensagem de resposta com base no estado do compressor
        String message = compressorLigado ? "Compressor ligado!" : "Compressor desligado!";
        if (compressorLigado && (isAfterClosingTime() || isBeforeOpeningTime())) {
            message += " Alerta! Já se passou das 22:30 e ainda não chegou às 07:30, atente-se para desligar o compressor após o uso.";
            Serial.println("Alerta: Compressor ligado fora do horário permitido.");
        }

        Serial.print("Estado do compressor: ");
        Serial.println(message);
        request->send(200, "text/plain", message); // Envia a resposta ao cliente

        saveCompressorState(compressorLigado); // Salva o estado do compressor no arquivo

        // Inicializa o timer se o compressor estiver ligado
        if (compressorLigado) {
            previousMillis = millis();
            timerAtivo = true;
            Serial.println("Compressor ligado. Timer ativado.");
        } else {
            Serial.println("Compressor desligado. Timer desativado.");
        } });

    setupTimeClient(); // Configura o cliente de tempo

    Serial.print("Estado inicial do compressor: ");
    Serial.println(compressorLigado ? "Ligado" : "Desligado");

    updateTime(); // Atualiza o tempo
    Serial.print("Hora atual: ");
    Serial.println(getTimeClient()); // Exibe a hora atual em formato completo

    // Desliga o compressor se estiver fora do horário permitido
    if (isAfterClosingTime() || isBeforeOpeningTime())
    {
        compressorLigado = false;
        digitalWrite(pinoLigaDesliga, LOW);
        saveCompressorState(compressorLigado); // Salva o estado do compressor como desligado
        Serial.println("Compressor desligado devido ao horário na inicialização.");
    }
    else
    {
        Serial.println("Compressor inicializado dentro do horário permitido.");
    }
}

// -------------------------------------------------------------------------
// Funções de Atualização e Desligamento
// -------------------------------------------------------------------------

// Função para atualizar o status do compressor com base no intervalo definido
void updateCompressorStatus()
{
    static unsigned long lastStatusUpdate = 0;
    unsigned long currentMillis = millis();

    // Atualiza o estado do compressor com base no intervalo definido
    if (compressorLigado && (currentMillis - lastStatusUpdate >= intervalo))
    {
        lastStatusUpdate = currentMillis;      // Atualiza o tempo de referência
        saveCompressorState(compressorLigado); // Salva o estado do compressor
    }

    // Desliga o compressor se o sistema estiver em manutenção
    if (sistemaEmManutencao)
    {
        if (compressorLigado)
        {
            compressorLigado = false;
            digitalWrite(pinoLigaDesliga, LOW);    // Desliga o compressor
            saveCompressorState(compressorLigado); // Salva o estado do compressor
            Serial.println("Sistema em manutenção. Compressor desligado.");
        }
    }
    else if (compressorLigado && isAfterClosingTime())
    {
        compressorLigado = false;
        digitalWrite(pinoLigaDesliga, LOW);    // Desliga o compressor
        saveCompressorState(compressorLigado); // Salva o estado do compressor
        Serial.println("Desligando o compressor pois é após as 22:30.");
    }
}

/**
 * Função para desligar o dispositivo e salvar o estado do compressor.
 */
void shutdown()
{
    saveCompressorState(compressorLigado); // Salva o estado do compressor
    Serial.println("Dispositivo desligado. Estado do compressor salvo.");
    delay(1000);   // Aguarda um segundo para garantir que a mensagem seja exibida
    ESP.restart(); // Reinicia o ESP32
}
