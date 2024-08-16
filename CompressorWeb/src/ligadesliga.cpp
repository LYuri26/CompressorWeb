// -------------------------------------------------------------------------
// Inclusão de Bibliotecas
// -------------------------------------------------------------------------
#include <FS.h>                // Biblioteca base para acesso ao sistema de arquivos no ESP32. Fornece funções genéricas para manipulação de arquivos.
#include <SPIFFS.h>            // Biblioteca específica para o sistema de arquivos SPIFFS (SPI Flash File System), utilizado para armazenar arquivos no flash do ESP32.
#include <ESPAsyncWebServer.h> // Biblioteca para criar um servidor web assíncrono no ESP32, permitindo o tratamento de requisições HTTP de maneira não bloqueante e eficiente.
#include "autenticador.h"      // Cabeçalho personalizado que provavelmente define funções para autenticação de usuários.
#include "ligadesliga.h"       // Cabeçalho personalizado que provavelmente define funções para ligar e desligar dispositivos.
#include "tempo.h"             // Cabeçalho personalizado que provavelmente define funções para manipulação e sincronização de tempo.
#include "manutencao.h"        // Cabeçalho personalizado que provavelmente define funções para o modo de manutenção do sistema.

// -------------------------------------------------------------------------
// Declarações e Definições Globais
// -------------------------------------------------------------------------
int pinosMotores[] = {26, 27, 28}; // Declara um array de inteiros para armazenar os números dos pinos aos quais os motores estão conectados.

const long intervalo = 300000; // Define o intervalo (em milissegundos) para atualizar o estado dos motores. Aqui, é 5 minutos.

const String arquivosEstados[] = {"/motor1.txt", "/motor2.txt", "/motor3.txt"}; // Declara um array de Strings com os caminhos dos arquivos onde o estado de cada motor será salvo.

bool motoresLigados[] = {false, false, false}; // Declara um array de booleanos para armazenar o estado atual dos motores (ligado ou desligado).

bool timersAtivos[] = {false, false, false}; // Declara um array de booleanos para controlar se os timers para cada motor estão ativos.

unsigned long previousMillis[] = {0, 0, 0}; // Declara um array para armazenar o timestamp da última atualização para cada motor. Usado para controle de tempo.

unsigned long lastToggleTime[] = {0, 0, 0}; // Declara um array para armazenar o timestamp da última vez que cada motor foi ligado ou desligado.

// -------------------------------------------------------------------------
// Função para manipular a ação de ligar/desligar motores
// -------------------------------------------------------------------------
void handleToggleAction(AsyncWebServer &server)
{
    server.on("/toggle", HTTP_ANY, [](AsyncWebServerRequest *request) // Define a rota "/toggle" que aceita qualquer método HTTP e usa uma função lambda para processar a requisição.
              {
        if (!isAuthenticated(request)) { // Verifica se o usuário está autenticado.
            redirectToAccessDenied(request); // Redireciona para a página de acesso negado se não estiver autenticado.
            return; // Sai da função se a autenticação falhar.
        }

        int motorIdx = request->getParam("motor")->value().toInt() - 1; // Obtém o índice do motor da requisição e ajusta para 0 baseado.
        String action = request->getParam("action")->value(); // Obtém a ação desejada ("ligar" ou "desligar") da requisição.

        if (motorIdx < 0 || motorIdx > 2) { // Verifica se o índice do motor é válido.
            request->send(400, "text/plain", "Motor inválido!"); // Envia uma resposta de erro 400 (Bad Request) se o índice for inválido.
            return; // Sai da função se o índice for inválido.
        }

        if (action == "ligar") { // Se a ação for ligar o motor.
            motoresLigados[motorIdx] = true; // Define o estado do motor como ligado.
            request->send(200, "text/plain", "Motor " + String(motorIdx + 1) + " ligado!"); // Envia uma resposta de sucesso 200 (OK) indicando que o motor foi ligado.
        } else if (action == "desligar") { // Se a ação for desligar o motor.
            motoresLigados[motorIdx] = false; // Define o estado do motor como desligado.
            request->send(200, "text/plain", "Motor " + String(motorIdx + 1) + " desligado!"); // Envia uma resposta de sucesso 200 (OK) indicando que o motor foi desligado.
        } else { // Se a ação não for "ligar" nem "desligar".
            request->send(400, "text/plain", "Ação inválida!"); // Envia uma resposta de erro 400 (Bad Request) indicando que a ação não é válida.
        } });
}

// -------------------------------------------------------------------------
// Função para inicializar o sistema de arquivos SPIFFS
// -------------------------------------------------------------------------
void initSPIFFS()
{
    if (!SPIFFS.begin(true)) // Tenta iniciar o SPIFFS. O parâmetro 'true' força a formatação se houver falha na inicialização.
    {
        Serial.println("Erro ao iniciar SPIFFS."); // Imprime uma mensagem de erro se a inicialização falhar.
    }
    else
    {
        Serial.println("SPIFFS inicializado."); // Imprime uma mensagem de sucesso se a inicialização for bem-sucedida.
    }
}

// -------------------------------------------------------------------------
// Função para ler o estado do motor a partir de um arquivo
// -------------------------------------------------------------------------
bool readMotorState(const String &arquivoEstado)
{
    File file = SPIFFS.open(arquivoEstado, "r"); // Abre o arquivo de estado no modo leitura.
    if (!file)                                   // Se o arquivo não for aberto corretamente.
    {
        Serial.println("Arquivo de estado não encontrado, assumindo estado desligado."); // Imprime uma mensagem indicando que o arquivo não foi encontrado.
        return false;                                                                    // Retorna false assumindo que o motor está desligado.
    }

    String state = file.readStringUntil('\n'); // Lê a primeira linha do arquivo, que contém o estado do motor.
    file.close();                              // Fecha o arquivo.
    return state.toInt() == 1;                 // Retorna true se o estado for 1 (ligado) e false se for 0 (desligado).
}

// -------------------------------------------------------------------------
// Função para salvar o estado do motor em um arquivo
// -------------------------------------------------------------------------
void saveMotorState(const String &arquivoEstado, bool state)
{
    File file = SPIFFS.open(arquivoEstado, "w"); // Abre o arquivo de estado no modo escrita.
    if (!file)                                   // Se o arquivo não for aberto corretamente.
    {
        Serial.println("Erro ao abrir o arquivo para escrita."); // Imprime uma mensagem de erro.
        return;                                                  // Sai da função se não puder abrir o arquivo.
    }

    file.println(state ? "1" : "0"); // Escreve "1" se o estado for verdadeiro (ligado) ou "0" se for falso (desligado).
    file.close();                    // Fecha o arquivo.
}

// -------------------------------------------------------------------------
// Funções para manipulação de tempo
// -------------------------------------------------------------------------
int getHoursFromTime(const String &time)
{
    return time.substring(11, 13).toInt(); // Extrai e converte as horas do tempo fornecido no formato de String.
}

int getMinutesFromTime(const String &time)
{
    return time.substring(14, 16).toInt(); // Extrai e converte os minutos do tempo fornecido no formato de String.
}

// -------------------------------------------------------------------------
// Verifica se o horário atual é após o horário de fechamento
// -------------------------------------------------------------------------
bool isAfterClosingTime()
{
    updateTime();                                       // Atualiza o tempo.
    int hour = getHoursFromTime(getTimeClient());       // Obtém a hora atual.
    int minute = getMinutesFromTime(getTimeClient());   // Obtém os minutos atuais.
    return (hour > 22) || (hour == 22 && minute >= 30); // Retorna true se a hora for após 22:30.
}

// -------------------------------------------------------------------------
// Verifica se o horário atual é antes do horário de abertura
// -------------------------------------------------------------------------
bool isBeforeOpeningTime()
{
    updateTime();                                     // Atualiza o tempo.
    int hour = getHoursFromTime(getTimeClient());     // Obtém a hora atual.
    int minute = getMinutesFromTime(getTimeClient()); // Obtém os minutos atuais.
    return (hour < 7) || (hour == 7 && minute < 30);  // Retorna true se a hora for antes das 07:30.
}

// -------------------------------------------------------------------------
// Configuração das rotas de ligar/desligar
// -------------------------------------------------------------------------
void setupLigaDesliga(AsyncWebServer &server)
{
    initSPIFFS(); // Inicializa o sistema de arquivos SPIFFS.

    for (int i = 0; i < 3; i++) // Loop para configurar os pinos dos motores.
    {
        pinMode(pinosMotores[i], OUTPUT);                              // Define o modo dos pinos como OUTPUT.
        motoresLigados[i] = readMotorState(arquivosEstados[i]);        // Lê o estado inicial dos motores do SPIFFS.
        digitalWrite(pinosMotores[i], motoresLigados[i] ? HIGH : LOW); // Define o estado inicial dos motores com base nos arquivos de estado.
    }

    server.on("/toggle", HTTP_GET, [](AsyncWebServerRequest *request) // Define a rota "/toggle" para o método GET.
              {
        unsigned long currentMillis = millis(); // Obtém o tempo atual em milissegundos.
        int motorIdx = request->getParam("motor")->value().toInt() - 1; // Obtém o índice do motor da requisição.

        if (motorIdx < 0 || motorIdx > 2) { // Verifica se o índice do motor é válido.
            request->send(400, "text/plain", "Motor inválido!"); // Envia uma resposta de erro 400 (Bad Request) se o índice for inválido.
            return; // Sai da função se o índice for inválido.
        }

        if (motoresLigados[motorIdx] && (currentMillis - lastToggleTime[motorIdx] < 30000)) { // Verifica se o motor foi alternado recentemente.
            request->send(200, "text/plain", "Comando ignorado. Aguarde 30 segundos entre as tentativas."); // Envia uma mensagem informando que a tentativa foi ignorada devido ao intervalo de 30 segundos.
            return; // Sai da função se o comando for ignorado.
        }

        lastToggleTime[motorIdx] = currentMillis; // Atualiza o timestamp da última alteração para o motor.
        motoresLigados[motorIdx] = !motoresLigados[motorIdx]; // Alterna o estado do motor.
        digitalWrite(pinosMotores[motorIdx], motoresLigados[motorIdx] ? HIGH : LOW); // Atualiza o pino do motor com o novo estado.

        String message = motoresLigados[motorIdx] ? "Motor " + String(motorIdx + 1) + " ligado!" : "Motor " + String(motorIdx + 1) + " desligado!"; // Prepara a mensagem a ser enviada.
        if (motoresLigados[motorIdx] && (isAfterClosingTime() || isBeforeOpeningTime())) { // Verifica se o motor está ligado fora do horário permitido.
            message += " Alerta! Atente-se para desligar o motor " + String(motorIdx + 1) + " após o uso."; // Adiciona uma mensagem de alerta.
        }

        request->send(200, "text/plain", message); // Envia a resposta com o status do motor.
        saveMotorState(arquivosEstados[motorIdx], motoresLigados[motorIdx]); // Salva o novo estado do motor no arquivo.

        if (motoresLigados[motorIdx]) { // Se o motor está ligado.
            previousMillis[motorIdx] = millis(); // Atualiza o timestamp da última atualização para o motor.
            timersAtivos[motorIdx] = true; // Marca o timer como ativo para o motor.
        } });

    setupTimeClient(); // Configura o cliente de tempo para obter o tempo atual.

    if (isAfterClosingTime() || isBeforeOpeningTime()) // Se a inicialização ocorre fora do horário permitido.
    {
        for (int i = 0; i < 3; i++) // Loop para desligar todos os motores.
        {
            motoresLigados[i] = false;                             // Define o estado dos motores como desligado.
            digitalWrite(pinosMotores[i], LOW);                    // Atualiza o pino do motor para desligado.
            saveMotorState(arquivosEstados[i], motoresLigados[i]); // Salva o estado dos motores no arquivo.
        }
        Serial.println("Motores desligados devido ao horário na inicialização."); // Imprime uma mensagem indicando que os motores foram desligados devido ao horário.
    }
}

// -------------------------------------------------------------------------
// Função para atualizar o status dos motores
// -------------------------------------------------------------------------
void updateMotorStatus()
{
    unsigned long currentMillis = millis(); // Obtém o tempo atual em milissegundos.

    for (int i = 0; i < 3; i++) // Loop para atualizar o status de cada motor.
    {
        if (motoresLigados[i] && (currentMillis - previousMillis[i] >= intervalo))
        {                                                          // Verifica se o motor deve ser atualizado.
            previousMillis[i] = currentMillis;                     // Atualiza o timestamp da última atualização.
            saveMotorState(arquivosEstados[i], motoresLigados[i]); // Salva o estado atualizado do motor no arquivo.
        }

        if (sistemaEmManutencao || (motoresLigados[i] && isAfterClosingTime()))
        {                                                          // Verifica se o sistema está em manutenção ou se o motor está ligado fora do horário permitido.
            motoresLigados[i] = false;                             // Define o estado do motor como desligado.
            digitalWrite(pinosMotores[i], LOW);                    // Atualiza o pino do motor para desligado.
            saveMotorState(arquivosEstados[i], motoresLigados[i]); // Salva o estado atualizado do motor no arquivo.
        }
    }
}

// -------------------------------------------------------------------------
// Função para desligar o sistema
// -------------------------------------------------------------------------
void shutdown()
{
    for (int i = 0; i < 3; i++) // Loop para salvar o estado de todos os motores antes de desligar.
    {
        saveMotorState(arquivosEstados[i], motoresLigados[i]); // Salva o estado do motor no arquivo.
    }
    Serial.println("Dispositivo desligado. Estado dos motores salvo."); // Imprime uma mensagem indicando que o dispositivo está desligado e o estado dos motores foi salvo.
    delay(1000);                                                        // Aguarda 1 segundo para garantir que a mensagem seja registrada.
    ESP.restart();                                                      // Reinicia o ESP32.
}
