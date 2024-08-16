// -------------------------------------------------------------------------
// Bibliotecas necessárias
// -------------------------------------------------------------------------
#include <FS.h>                // Biblioteca para manipulação do sistema de arquivos (SPIFFS)
#include <SPIFFS.h>            // Biblioteca para manipulação do sistema de arquivos do ESP32 (SPIFFS)
#include <ESPAsyncWebServer.h> // Biblioteca para criar servidor web assíncrono no ESP32
#include "autenticador.h"      // Biblioteca personalizada para autenticação de usuário
#include "ligadesliga.h"       // Biblioteca personalizada para funções de ligar/desligar motores
#include "tempo.h"             // Biblioteca personalizada para manipulação do tempo
#include "manutencao.h"        // Biblioteca personalizada para controle de manutenção

// -------------------------------------------------------------------------
// Definição de variáveis globais
// -------------------------------------------------------------------------
int pinosMotores[] = {26, 27, 12};                                              // Pinos do ESP32 conectados aos motores
const long intervalo = 300000;                                                  // Intervalo para salvar o estado dos motores (em milissegundos)
const String arquivosEstados[] = {"/motor1.txt", "/motor2.txt", "/motor3.txt"}; // Arquivos SPIFFS para armazenar o estado dos motores
bool motoresLigados[] = {false, false, false};                                  // Estado atual dos motores (ligado/desligado)
bool timersAtivos[] = {false, false, false};                                    // Flags para verificar se o timer está ativo para cada motor
unsigned long previousMillis[] = {0, 0, 0};                                     // Armazena o tempo da última atualização dos motores
unsigned long lastToggleTime[] = {0, 0, 0};                                     // Armazena o tempo da última mudança de estado dos motores

// -------------------------------------------------------------------------
// Função para configurar a página de login
// -------------------------------------------------------------------------
void handleToggleAction(AsyncWebServer &server)
{
    server.on("/toggle", HTTP_ANY, [](AsyncWebServerRequest *request)
              {
        if (!isAuthenticated(request)) {  // Verifica se o usuário está autenticado
            redirectToAccessDenied(request); // Redireciona para página de acesso negado se não autenticado
            return;
        }

        int motorIdx = request->getParam("motor")->value().toInt() - 1; // Obtém o índice do motor (de 0 a 2)
        String action = request->getParam("action")->value();           // Obtém a ação (ligar/desligar)

        if (motorIdx < 0 || motorIdx > 2) {  // Verifica se o índice do motor é válido
            request->send(400, "text/plain", "Motor inválido!");  // Responde com erro 400 se o motor for inválido
            Serial.println("Erro: Motor inválido! (Índice fora do intervalo)");
            return;
        }

        if (action == "ligar") {
            motoresLigados[motorIdx] = true;  // Liga o motor
            request->send(200, "text/plain", "Motor " + String(motorIdx + 1) + " ligado!");
        } else if (action == "desligar") {
            motoresLigados[motorIdx] = false;  // Desliga o motor
            request->send(200, "text/plain", "Motor " + String(motorIdx + 1) + " desligado!");
        } else {
            request->send(400, "text/plain", "Ação inválida!");  // Responde com erro 400 se a ação for inválida
            Serial.println("Erro: Ação inválida!");
        } });
}

// -------------------------------------------------------------------------
// Função para inicializar o SPIFFS
// -------------------------------------------------------------------------
void initSPIFFS()
{
    if (!SPIFFS.begin(true))
    {                                              // Tenta iniciar o SPIFFS, formatando se necessário
        Serial.println("Erro ao iniciar SPIFFS."); // Exibe mensagem de erro se falhar
    }
    else
    {
        Serial.println("SPIFFS inicializado."); // Exibe mensagem de sucesso se inicializado
    }
}

// -------------------------------------------------------------------------
// Função para ler o estado do motor a partir de um arquivo SPIFFS
// -------------------------------------------------------------------------
bool readMotorState(const String &arquivoEstado)
{
    File file = SPIFFS.open(arquivoEstado, "r"); // Abre o arquivo de estado do motor no modo leitura
    if (!file)
    { // Verifica se o arquivo foi aberto com sucesso
        Serial.println("Arquivo de estado não encontrado: " + arquivoEstado + ". Assumindo estado desligado.");
        return false; // Retorna falso se o arquivo não existir, assumindo que o motor está desligado
    }

    String state = file.readStringUntil('\n'); // Lê o estado do motor do arquivo
    file.close();                              // Fecha o arquivo após a leitura
    Serial.println("Estado lido do arquivo " + arquivoEstado + ": " + state);
    return state.toInt() == 1; // Retorna verdadeiro se o estado lido for "1", indicando que o motor está ligado
}

// -------------------------------------------------------------------------
// Função para salvar o estado do motor em um arquivo SPIFFS
// -------------------------------------------------------------------------
void saveMotorState(const String &arquivoEstado, bool state)
{
    File file = SPIFFS.open(arquivoEstado, "w"); // Abre o arquivo de estado do motor no modo escrita
    if (!file)
    { // Verifica se o arquivo foi aberto com sucesso
        Serial.println("Erro ao abrir o arquivo para escrita: " + arquivoEstado);
        return;
    }

    file.println(state ? "1" : "0"); // Escreve "1" se o motor estiver ligado, ou "0" se estiver desligado
    file.close();                    // Fecha o arquivo após a escrita
    Serial.println("Estado salvo no arquivo " + arquivoEstado + ": " + (state ? "Ligado" : "Desligado"));
}

// -------------------------------------------------------------------------
// Função para extrair as horas de uma string de horário
// -------------------------------------------------------------------------
int getHoursFromTime(const String &time)
{
    return time.substring(11, 13).toInt(); // Extrai e retorna as horas da string de horário
}

// -------------------------------------------------------------------------
// Função para extrair os minutos de uma string de horário
// -------------------------------------------------------------------------
int getMinutesFromTime(const String &time)
{
    return time.substring(14, 16).toInt(); // Extrai e retorna os minutos da string de horário
}

// -------------------------------------------------------------------------
// Função para verificar se o horário atual é após o horário de fechamento
// -------------------------------------------------------------------------
bool isAfterClosingTime()
{
    updateTime();                                       // Atualiza o horário atual
    int hour = getHoursFromTime(getTimeClient());       // Obtém a hora atual
    int minute = getMinutesFromTime(getTimeClient());   // Obtém os minutos atuais
    return (hour > 22) || (hour == 22 && minute >= 30); // Retorna verdadeiro se for após 22:30
}

// -------------------------------------------------------------------------
// Função para verificar se o horário atual é antes do horário de abertura
// -------------------------------------------------------------------------
bool isBeforeOpeningTime()
{
    updateTime();                                     // Atualiza o horário atual
    int hour = getHoursFromTime(getTimeClient());     // Obtém a hora atual
    int minute = getMinutesFromTime(getTimeClient()); // Obtém os minutos atuais
    return (hour < 7) || (hour == 7 && minute < 30);  // Retorna verdadeiro se for antes de 07:30
}

// -------------------------------------------------------------------------
// Função para configurar o sistema de ligar/desligar motores
// -------------------------------------------------------------------------
void setupLigaDesliga(AsyncWebServer &server)
{
    initSPIFFS(); // Inicializa o SPIFFS para acessar os arquivos de estado dos motores

    for (int i = 0; i < 3; i++)
    {
        pinMode(pinosMotores[i], OUTPUT);                              // Configura os pinos dos motores como saída
        motoresLigados[i] = readMotorState(arquivosEstados[i]);        // Lê o estado dos motores a partir dos arquivos SPIFFS
        digitalWrite(pinosMotores[i], motoresLigados[i] ? HIGH : LOW); // Define o estado inicial dos motores (ligado/desligado)
    }

    server.on("/toggle", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        unsigned long currentMillis = millis();  // Obtém o tempo atual em milissegundos desde o início do programa
        int motorIdx = request->getParam("motor")->value().toInt() - 1;  // Obtém o índice do motor a partir dos parâmetros da requisição

        if (motorIdx < 0 || motorIdx > 2) {  // Verifica se o índice do motor é válido
            request->send(400, "text/plain", "Motor inválido!");  // Responde com erro 400 se o motor for inválido
            Serial.println("Erro: Motor inválido! (Índice fora do intervalo)");
            return;
        }

        if (motoresLigados[motorIdx] && (currentMillis - lastToggleTime[motorIdx] < 30000)) {
            request->send(200, "text/plain", "Comando ignorado. Aguarde 30 segundos entre as tentativas.");  // Responde com erro se a tentativa for muito próxima da anterior
            Serial.println("Erro: Comando ignorado. Aguarde 30 segundos entre as tentativas.");
            return;
        }

        lastToggleTime[motorIdx] = currentMillis;  // Atualiza o tempo da última mudança de estado do motor
        motoresLigados[motorIdx] = !motoresLigados[motorIdx];  // Alterna o estado do motor (liga/desliga)
        digitalWrite(pinosMotores[motorIdx], motoresLigados[motorIdx] ? HIGH : LOW);  // Atualiza o estado físico do motor

        String message = motoresLigados[motorIdx] ? "Motor " + String(motorIdx + 1) + " ligado!" : "Motor " + String(motorIdx + 1) + " desligado!";

        if (motoresLigados[motorIdx] && (isAfterClosingTime() || isBeforeOpeningTime())) {
            message += " Alerta! Atente-se para desligar o motor " + String(motorIdx + 1) + " após o uso.";  // Adiciona um alerta se o motor for ligado fora do horário permitido
        }

        request->send(200, "text/plain", message);  // Responde com o estado atual do motor
        saveMotorState(arquivosEstados[motorIdx], motoresLigados[motorIdx]);  // Salva o estado do motor no arquivo SPIFFS

        if (motoresLigados[motorIdx]) {
            previousMillis[motorIdx] = millis();  // Atualiza o tempo da última ativação do motor
            timersAtivos[motorIdx] = true;  // Ativa o timer para o motor
        } });

    setupTimeClient(); // Configura o cliente de tempo (NTP) para sincronizar o horário

    if (isAfterClosingTime() || isBeforeOpeningTime())
    { // Verifica se o horário atual está fora do horário de operação permitido
        for (int i = 0; i < 3; i++)
        {
            motoresLigados[i] = false;                             // Desliga todos os motores
            digitalWrite(pinosMotores[i], LOW);                    // Atualiza o estado físico dos motores para desligado
            saveMotorState(arquivosEstados[i], motoresLigados[i]); // Salva o estado desligado dos motores no arquivo SPIFFS
        }
        Serial.println("Motores desligados devido ao horário na inicialização."); // Informa que os motores foram desligados na inicialização devido ao horário
    }
}

// -------------------------------------------------------------------------
// Função para atualizar o estado dos motores periodicamente
// -------------------------------------------------------------------------
void updateMotorStatus()
{
    unsigned long currentMillis = millis(); // Obtém o tempo atual em milissegundos

    for (int i = 0; i < 3; i++)
    {
        if (motoresLigados[i] && (currentMillis - previousMillis[i] >= intervalo))
        {
            previousMillis[i] = currentMillis;                     // Atualiza o tempo da última verificação
            saveMotorState(arquivosEstados[i], motoresLigados[i]); // Salva o estado atual do motor no arquivo SPIFFS
        }

        if (sistemaEmManutencao || (motoresLigados[i] && isAfterClosingTime()))
        {                                                                                             // Verifica se o sistema está em manutenção ou fora do horário permitido
            motoresLigados[i] = false;                                                                // Desliga o motor
            digitalWrite(pinosMotores[i], LOW);                                                       // Atualiza o estado físico do motor para desligado
            saveMotorState(arquivosEstados[i], motoresLigados[i]);                                    // Salva o estado desligado do motor no arquivo SPIFFS
            Serial.println("Motor " + String(i + 1) + " desligado devido ao horário ou manutenção."); // Informa que o motor foi desligado devido ao horário ou manutenção
        }
    }
}

// -------------------------------------------------------------------------
// Função para desligar o sistema e salvar o estado dos motores
// -------------------------------------------------------------------------
void shutdown()
{
    for (int i = 0; i < 3; i++)
    {
        saveMotorState(arquivosEstados[i], motoresLigados[i]); // Salva o estado atual dos motores no arquivo SPIFFS
    }
    Serial.println("Dispositivo desligado. Estado dos motores salvo."); // Informa que o dispositivo foi desligado e o estado dos motores foi salvo
    delay(1000);                                                        // Aguarda 1 segundo antes de reiniciar
    ESP.restart();                                                      // Reinicia o ESP32
}
