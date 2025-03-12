#include <FS.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include "autenticador.h"
#include "ligadesliga.h"
#include "tempo.h"
#include "manutencao.h"

int pinosMotores[] = {26, 27, 12};
const long intervalo = 300000;
const String arquivosEstados[] = {"/motor1.txt", "/motor2.txt", "/motor3.txt"};
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
            Serial.println("Erro: Motor inválido! (Índice fora do intervalo)");
            return;
        }

        if (action == "ligar") {
            motoresLigados[motorIdx] = true;
            request->send(200, "text/plain", "Motor " + String(motorIdx + 1) + " ligado!");
        } else if (action == "desligar") {
            motoresLigados[motorIdx] = false;
            request->send(200, "text/plain", "Motor " + String(motorIdx + 1) + " desligado!");
        } else {
            request->send(400, "text/plain", "Ação inválida!");
            Serial.println("Erro: Ação inválida!");
        } });
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

int getHoursFromTime(const String &time)
{
    return time.substring(11, 13).toInt();
}

int getMinutesFromTime(const String &time)
{
    return time.substring(14, 16).toInt();
}

bool isAfterClosingTime()
{
    updateTime();
    int hour = getHoursFromTime(getTimeClient());
    int minute = getMinutesFromTime(getTimeClient());
    return (hour > 22) || (hour == 22 && minute >= 30);
}

bool isBeforeOpeningTime()
{
    updateTime();
    int hour = getHoursFromTime(getTimeClient());
    int minute = getMinutesFromTime(getTimeClient());
    return (hour < 7) || (hour == 7 && minute < 30);
}

void setupLigaDesliga(AsyncWebServer &server)
{
    initSPIFFS();

    for (int i = 0; i < 3; i++)
    {
        pinMode(pinosMotores[i], OUTPUT);
        motoresLigados[i] = readMotorState(arquivosEstados[i]);
        digitalWrite(pinosMotores[i], motoresLigados[i] ? HIGH : LOW);
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
            if (currentMillis - lastToggleTime[motorIdx] < 3600000) {
                request->send(200, "text/plain", "Comando ignorado. Aguarde 1 hora entre as tentativas de desligar.");
                Serial.println("Erro: Comando ignorado. Aguarde 1 hora entre as tentativas de desligar.");
                return;
            }
        }

        lastToggleTime[motorIdx] = currentMillis;
        motoresLigados[motorIdx] = !motoresLigados[motorIdx];
        digitalWrite(pinosMotores[motorIdx], motoresLigados[motorIdx] ? HIGH : LOW);

        String message = motoresLigados[motorIdx] ? "Motor " + String(motorIdx + 1) + " ligado!" : "Motor " + String(motorIdx + 1) + " desligado!";

        if (motoresLigados[motorIdx] && (isAfterClosingTime() || isBeforeOpeningTime())) {
            message += " Alerta! Atente-se para desligar o motor " + String(motorIdx + 1) + " após o uso.";
        }

        request->send(200, "text/plain", message);
        saveMotorState(arquivosEstados[motorIdx], motoresLigados[motorIdx]);

        if (motoresLigados[motorIdx]) {
            previousMillis[motorIdx] = millis();
            timersAtivos[motorIdx] = true;
        } });

    setupTimeClient();

    if (isAfterClosingTime() || isBeforeOpeningTime())
    {
        for (int i = 0; i < 3; i++)
        {
            motoresLigados[i] = false;
            digitalWrite(pinosMotores[i], LOW);
            saveMotorState(arquivosEstados[i], motoresLigados[i]);
        }
        Serial.println("Motores desligados devido ao horário na inicialização.");
    }
}

void updateMotorStatus()
{
    unsigned long currentMillis = millis();

    for (int i = 0; i < 3; i++)
    {
        if (motoresLigados[i] && (currentMillis - previousMillis[i] >= intervalo))
        {
            previousMillis[i] = currentMillis;
            saveMotorState(arquivosEstados[i], motoresLigados[i]);
        }
    }
}
