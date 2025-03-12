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

void handleToggleAction(AsyncWebServer &server)
{
    server.on("/toggle", HTTP_ANY, [](AsyncWebServerRequest *request)
              {
        if (!isAuthenticated(request)) {
            redirectToAccessDenied(request);
            return;
        }

        int motorIdx = request->getParam("motor")->value().toInt() - 1;
        String action = request->getParam("action")->value();

        if (motorIdx < 0 || motorIdx > 2) {
            request->send(400, "text/plain", "Motor inválido!");
            return;
        }

        if (action == "ligar") {
            digitalWrite(pinosMotores[motorIdx], HIGH);
            delay(500);
            digitalWrite(pinosMotores[motorIdx], LOW);

            if (digitalRead(pinosStatus[motorIdx]) == HIGH) {
                motoresLigados[motorIdx] = true;
                request->send(200, "text/plain", "Motor " + String(motorIdx + 1) + " ligado!");
            } else {
                request->send(400, "text/plain", "Falha ao ligar o motor.");
            }
        } else if (action == "desligar") {
            motoresLigados[motorIdx] = false;
            request->send(200, "text/plain", "Motor " + String(motorIdx + 1) + " desligado!");
        } else {
            request->send(400, "text/plain", "Ação inválida!");
        }

        // Atualizar estado imediatamente após a ação
        saveMotorState(arquivosEstados[motorIdx], motoresLigados[motorIdx]); });
}
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

// Função para salvar o estado dos pinos de status
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

void setupLigaDesliga(AsyncWebServer &server)
{
    initSPIFFS();

    // Configura os pinos de controle dos motores
    for (int i = 0; i < 3; i++)
    {
        pinMode(pinosMotores[i], OUTPUT);
        motoresLigados[i] = readMotorState(arquivosEstados[i]);
        digitalWrite(pinosMotores[i], LOW); // Garante que os motores comecem desligados
    }

    // Configura os pinos de status para os compressores
    for (int i = 0; i < 3; i++)
    {
        pinMode(pinosStatus[i], INPUT); // Esses pinos irão ler o status do compressor
    }

    server.on("/toggle", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        unsigned long currentMillis = millis();
        int motorIdx = request->getParam("motor")->value().toInt() - 1;

        if (motorIdx < 0 || motorIdx > 2) {
            request->send(400, "text/plain", "Motor inválido!");
            Serial.println("Erro: Motor inválido! (Índice fora do intervalo)");
            return;
        }

        if (motoresLigados[motorIdx]) {
            if (currentMillis - lastToggleTime[motorIdx] < 0) {
                request->send(200, "text/plain", "Comando ignorado. Aguarde 1 hora entre as tentativas de desligar.");
                Serial.println("Erro: Comando ignorado. Aguarde 1 hora entre as tentativas de desligar.");
                return;
            }
        }

        lastToggleTime[motorIdx] = currentMillis;
        motoresLigados[motorIdx] = !motoresLigados[motorIdx];
        digitalWrite(pinosMotores[motorIdx], motoresLigados[motorIdx] ? HIGH : LOW);

        String message = motoresLigados[motorIdx] ? "Motor " + String(motorIdx + 1) + " ligado!" : "Motor " + String(motorIdx + 1) + " desligado!";
        request->send(200, "text/plain", message);
        saveMotorState(arquivosEstados[motorIdx], motoresLigados[motorIdx]);

        if (motoresLigados[motorIdx]) {
            previousMillis[motorIdx] = millis();
            timersAtivos[motorIdx] = true;
        } });

    setupTimeClient();
}

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