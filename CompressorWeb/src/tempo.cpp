#include "tempo.h"
#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h> // Biblioteca NTPClient

// Configurações NTP
const char* ntpServer = "pool.ntp.org";
const long utcOffsetInSeconds = -3 * 3600; // Ajuste de fuso horário para UTC-3

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, utcOffsetInSeconds);

// Variáveis globais
String currentTime = "";

void setupTimeClient() {
    timeClient.begin();
    updateTime(); // Inicializa a hora na configuração
    Serial.print("Hora inicial: ");
    Serial.println(currentTime);
}

String getTimeClient() {
    return currentTime;
}

void updateTime() {
    if (WiFi.status() == WL_CONNECTED) {
        timeClient.update(); // Atualiza o tempo
        unsigned long epochTime = timeClient.getEpochTime(); // Obtém o tempo em segundos desde 01/01/1970
        int hours = (epochTime % 86400L) / 3600;
        int minutes = (epochTime % 3600) / 60;
        int seconds = epochTime % 60;
        char timeString[20];
        snprintf(timeString, sizeof(timeString), "%02d:%02d:%02d", hours, minutes, seconds);
        currentTime = String(timeString);
    } else {
        Serial.println("WiFi desconectado");
    }
}
