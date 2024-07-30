#include "ligadesliga.h"
#include <NTPClient.h>
#include <WiFiUdp.h>

// Variáveis globais
bool compressorLigado = false;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", -3 * 3600, 60000); // Ajuste o fuso horário conforme necessário

unsigned long previousMillis = 0; // Tempo anterior para controle de atualização
const long interval = 300000;     // Intervalo de 5 minutos
unsigned long lastToggleTime = 0; // Tempo da última troca para debounce
bool timerAtivo = false;          // Flag para verificar se o timer está ativo

void printCurrentTime()
{
    timeClient.update();
    int hour = timeClient.getHours();
    int minute = timeClient.getMinutes();
    Serial.print("Hora atual: ");
    Serial.print(hour);
    Serial.print(":");
    Serial.println(minute);
}

bool isAfterClosingTime()
{
    printCurrentTime();
    int hour = timeClient.getHours();
    int minute = timeClient.getMinutes();
    return (hour > 22) || (hour == 22 && minute >= 30);
}

bool isBeforeOpeningTime()
{
    printCurrentTime();
    int hour = timeClient.getHours();
    int minute = timeClient.getMinutes();
    return (hour < 7) || (hour == 7 && minute < 30);
}

void printCompressorStatus()
{
    String message = compressorLigado ? "Compressor ligado!" : "Compressor desligado!";
    Serial.print("Estado do compressor: ");
    Serial.println(message);
}

void updateCompressorStatus()
{
    unsigned long currentMillis = millis();
    if (compressorLigado && (currentMillis - previousMillis >= interval))
    {
        previousMillis = currentMillis;
        if (isAfterClosingTime() || isBeforeOpeningTime())
        {
            compressorLigado = false;
            Serial.println("Compressor desligado devido ao horário.");
        }
        else
        {
            compressorLigado = !compressorLigado;
            printCompressorStatus();
        }
    }
}

void setupLigaDesliga(WebServer &server)
{
    server.on("/toggle", HTTP_GET, [&server]()
              {
        unsigned long currentMillis = millis();

        if (compressorLigado && (currentMillis - lastToggleTime < interval))
        {
            server.send(200, "text/plain", "Comando ignorado. Aguarde 5 minutos entre as tentativas.");
            return;
        }

        lastToggleTime = currentMillis;
        compressorLigado = !compressorLigado;

        String message = compressorLigado ? "Compressor ligado!" : "Compressor desligado!";
        if (compressorLigado && (isAfterClosingTime() || isBeforeOpeningTime()))
        {
            message += " Alerta! Já se passou das 22:30 e ainda não chegou às 07:30, atente-se para desligar o compressor após o uso.";
        }

        Serial.print("Estado do compressor: ");
        Serial.println(message);

        server.send(200, "text/plain", message);

        if (compressorLigado)
        {
            previousMillis = millis();
            timerAtivo = true;
        }
    });

    timeClient.begin();
    printCompressorStatus();
    printCurrentTime();

    if (isAfterClosingTime() || isBeforeOpeningTime())
    {
        compressorLigado = false;
        Serial.println("Compressor desligado devido ao horário na inicialização.");
    }
    else
    {
        Serial.println("Compressor inicializado dentro do horário permitido.");
    }
}
