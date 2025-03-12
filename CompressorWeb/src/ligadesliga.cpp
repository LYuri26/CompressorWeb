#include <FS.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include "autenticador.h"
#include "ligadesliga.h"
#include "tempo.h"
#include "manutencao.h"

// Pinos de controle de motores e pinos adicionais para status
int pinosMotores[] = {26, 27, 12}; // Para gerar pulsos rápidos
int pinosStatus[] = {32, 33, 35};  // Para indicar estado do compressor

const long intervalo = 1000; // Intervalo para gravação de estado
const String arquivosEstados[] = {"/motor1.txt", "/motor2.txt", "/motor3.txt", "/status1.txt", "/status2.txt", "/status3.txt"};

bool motoresLigados[] = {false, false, false};
bool timersAtivos[] = {false, false, false};
unsigned long previousMillis[] = {0, 0, 0};
unsigned long lastToggleTime[] = {0, 0, 0};

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
    updateTime();                                      // Atualiza o horário atual
    int hour = getHoursFromTime(getTimeClient());      // Obtém a hora atual
    int minute = getMinutesFromTime(getTimeClient());  // Obtém os minutos atuais
    return (hour > 22) || (hour == 22 && minute >= 0); // Retorna verdadeiro se for após 22:00
}

// -------------------------------------------------------------------------
// Função para verificar se o horário atual é antes do horário de abertura
// -------------------------------------------------------------------------
bool isBeforeOpeningTime()
{
    updateTime();                                     // Atualiza o horário atual
    int hour = getHoursFromTime(getTimeClient());     // Obtém a hora atual
    int minute = getMinutesFromTime(getTimeClient()); // Obtém os minutos atuais
    return (hour < 8) || (hour == 8 && minute < 0);   // Retorna verdadeiro se for antes de 08:00
}

// -------------------------------------------------------------------------
// Função para desligar todos os motores e bloqueá-los
// -------------------------------------------------------------------------
void desligarTodosMotores()
{
    for (int i = 0; i < 3; i++)
    {
        digitalWrite(pinosMotores[i], HIGH);
        delay(500); // Mantém o pulso por 500ms
        digitalWrite(pinosMotores[i], LOW);
        motoresLigados[i] = false;
        saveMotorState(arquivosEstados[i], false);
    }
}

// -------------------------------------------------------------------------
// Função para verificar se o sistema deve estar bloqueado (fora do horário de funcionamento ou em manutenção)
// -------------------------------------------------------------------------
bool sistemaDeveEstarBloqueado()
{
    return sistemaEmManutencao || isAfterClosingTime() || isBeforeOpeningTime();
}

// -------------------------------------------------------------------------
// Função para atualizar o estado dos motores com base no horário e estado de manutenção
// -------------------------------------------------------------------------
void atualizarEstadoMotores()
{
    if (sistemaDeveEstarBloqueado())
    {
        desligarTodosMotores();
    }
}

// -------------------------------------------------------------------------
// Função para configurar a página de login e o controle dos motores
// -------------------------------------------------------------------------
void handleToggleAction(AsyncWebServer &server)
{
    server.on("/toggle", HTTP_ANY, [](AsyncWebServerRequest *request)
              {
        if (!isAuthenticated(request)) {
            redirectToAccessDenied(request);
            return;
        }

        if (sistemaDeveEstarBloqueado()) {
            request->send(200, "text/plain", "Sistema bloqueado. Operação não permitida.");
            return;
        }

        int motorIdx = request->getParam("motor")->value().toInt() - 1;
        String action = request->getParam("action")->value();

        if (motorIdx < 0 || motorIdx > 2) {
            request->send(400, "text/plain", "Motor inválido!");
            return;
        }

        if (action == "ligar") {
            if (isAfterClosingTime() || isBeforeOpeningTime()) {
                request->send(200, "text/plain", "Não é permitido ligar o motor fora do horário de funcionamento.");
                return;
            }

            // Envia um pulso rápido
            digitalWrite(pinosMotores[motorIdx], HIGH);
            delay(500); // Mantém o pulso por 500ms
            digitalWrite(pinosMotores[motorIdx], LOW);

            // Verifica o status do motor após o pulso
            if (digitalRead(pinosStatus[motorIdx]) == HIGH) {
                motoresLigados[motorIdx] = true;
                request->send(200, "text/plain", "Motor " + String(motorIdx + 1) + " ligado!");
            } else {
                request->send(400, "text/plain", "Falha ao ligar o motor.");
            }
        } else if (action == "desligar") {
            // Envia um pulso rápido
            digitalWrite(pinosMotores[motorIdx], HIGH);
            delay(500); // Mantém o pulso por 500ms
            digitalWrite(pinosMotores[motorIdx], LOW);

            motoresLigados[motorIdx] = false;
            request->send(200, "text/plain", "Motor " + String(motorIdx + 1) + " desligado!");
        } else {
            request->send(400, "text/plain", "Ação inválida!");
        }

        // Atualizar estado imediatamente após a ação
        saveMotorState(arquivosEstados[motorIdx], motoresLigados[motorIdx]); });
}

// -------------------------------------------------------------------------
// Função para monitorar o status dos compressores
// -------------------------------------------------------------------------
void monitorarStatusCompressores()
{
    for (int i = 0; i < 3; i++)
    {
        bool estadoAtual = digitalRead(pinosStatus[i]) == HIGH; // Lê o status atual do compressor

        // Verifica se o estado mudou desde a última verificação
        if (estadoAtual != motoresLigados[i])
        {
            motoresLigados[i] = estadoAtual; // Atualiza o estado do motor

            // Salva o estado no arquivo correspondente
            saveStatusState(pinosStatus[i], estadoAtual);

            // Exibe a atualização no console
            Serial.println("Compressor " + String(i + 1) + " " + (estadoAtual ? "ligado" : "desligado"));
        }
    }
}

// -------------------------------------------------------------------------
// Função para inicializar o SPIFFS
// -------------------------------------------------------------------------
void initSPIFFS()
{
    if (!SPIFFS.begin(true))
    {
        Serial.println("Erro ao iniciar SPIFFS.");
    }
    else
    {
        Serial.println("SPIFFS inicializado.");
    }
}

// -------------------------------------------------------------------------
// Função para ler o estado do motor a partir de um arquivo SPIFFS
// -------------------------------------------------------------------------
bool readMotorState(const String &arquivoEstado)
{
    File file = SPIFFS.open(arquivoEstado, "r");
    if (!file)
    {
        Serial.println("Arquivo de estado não encontrado: " + arquivoEstado + ". Assumindo estado desligado.");
        return false;
    }

    String state = file.readStringUntil('\n');
    file.close();
    Serial.println("Estado lido do arquivo " + arquivoEstado + ": " + state);
    return state.toInt() == 1;
}

// -------------------------------------------------------------------------
// Função para salvar o estado do motor em um arquivo SPIFFS
// -------------------------------------------------------------------------
void saveMotorState(const String &arquivoEstado, bool state)
{
    File file = SPIFFS.open(arquivoEstado, "w");
    if (!file)
    {
        Serial.println("Erro ao abrir o arquivo para escrita: " + arquivoEstado);
        return;
    }

    file.println(state ? "1" : "0");
    file.close();
    Serial.println("Estado salvo no arquivo " + arquivoEstado + ": " + (state ? "Ligado" : "Desligado"));
}

// -------------------------------------------------------------------------
// Função para salvar o estado dos pinos de status
// -------------------------------------------------------------------------
void saveStatusState(int pin, bool state)
{
    String arquivoEstado;
    if (pin == 32)
        arquivoEstado = "/status1.txt";
    else if (pin == 33)
        arquivoEstado = "/status2.txt";
    else if (pin == 35)
        arquivoEstado = "/status3.txt";

    saveMotorState(arquivoEstado, state);
}

// -------------------------------------------------------------------------
// Função para configurar o sistema de ligar/desligar motores
// -------------------------------------------------------------------------
void setupLigaDesliga(AsyncWebServer &server)
{
    initSPIFFS();

    // Configura os pinos de controle dos motores
    for (int i = 0; i < 3; i++)
    {
        pinMode(pinosMotores[i], OUTPUT);
        digitalWrite(pinosMotores[i], LOW); // Garante que os motores comecem desligados
    }

    // Configura os pinos de status para os compressores
    for (int i = 0; i < 3; i++)
    {
        pinMode(pinosStatus[i], INPUT); // Esses pinos irão ler o status do compressor
    }

    handleToggleAction(server);
    setupTimeClient();

    // Atualiza o estado dos motores no início
    atualizarEstadoMotores();
}

// -------------------------------------------------------------------------
// Função para atualizar o estado dos motores
// -------------------------------------------------------------------------
void updateMotorStatus()
{
    Serial.println("Atualizando status dos motores...");

    for (int i = 0; i < 3; i++)
    {
        bool motorLigado = digitalRead(pinosStatus[i]); // Lê o status real do motor

        // Define o nome do arquivo correto para cada motor
        String arquivoEstado = "/status" + String(i + 1) + ".txt";

        // Abre o arquivo para escrita
        File file = SPIFFS.open(arquivoEstado, "w");
        if (!file)
        {
            Serial.println("Erro ao abrir " + arquivoEstado + " para escrita!");
            continue;
        }

        // Escreve o status no arquivo (ON/OFF)
        file.print(motorLigado ? "ON" : "OFF");
        file.close();

        Serial.println("Status do Motor " + String(i + 1) + " atualizado: " + (motorLigado ? "ON" : "OFF"));
    }
}