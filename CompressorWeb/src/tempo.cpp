#include "tempo.h"
#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <time.h>

const char* ntpServer = "pool.ntp.org";
const long utcOffsetInSeconds = -3 * 3600;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, utcOffsetInSeconds);

String currentTime = "";

// Declarações de funções
void setTimeFromNTP();
void printInternalTime();

void setupTimeClient() {
    timeClient.begin();
    updateTime();  // Atualiza a hora pela primeira vez
    setTimeFromNTP();  // Configura o relógio interno com a hora NTP
    Serial.print("Hora inicial da Internet: ");
    Serial.println(currentTime);
}

String getTimeClient() {
    return currentTime;
}

void updateTime() {
    if (WiFi.status() == WL_CONNECTED) {
        timeClient.update();
        unsigned long epochTime = timeClient.getEpochTime();
        int hours = (epochTime % 86400L) / 3600;
        int minutes = (epochTime % 3600) / 60;
        int seconds = epochTime % 60;
        char timeString[20];
        snprintf(timeString, sizeof(timeString), "%02d-%02d-%04dT%02d:%02d:%02d", 
                 timeClient.getHours(), timeClient.getMinutes(), timeClient.getSeconds(), hours, minutes, seconds);
        currentTime = String(timeString);

        if (seconds == 0) {
            Serial.println("Hora atual da Internet: " + currentTime);
            printInternalTime(); // Mostra a hora interna
        }
    } else {
        Serial.println("WiFi desconectado");
    }
}

void setTimeFromNTP() {
    if (WiFi.status() == WL_CONNECTED) {
        timeClient.update();
        unsigned long epochTime = timeClient.getEpochTime();
        
        // Configura a hora interna do ESP32
        time_t time = static_cast<time_t>(epochTime); // Converte para time_t
        struct tm timeInfo;
        gmtime_r(&time, &timeInfo); // Converte epochTime para tm
        
        Serial.print("Hora interna configurada para: ");
        Serial.println(asctime(&timeInfo));
        
        // Definindo a hora do sistema
        configTime(utcOffsetInSeconds, 0, ntpServer);
    } else {
        Serial.println("WiFi desconectado. Não foi possível configurar a hora interna.");
    }
}

void printInternalTime() {
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    char timeString[20];
    snprintf(timeString, sizeof(timeString), "%02d-%02d-%04dT%02d:%02d:%02d", 
             timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, 
             timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    Serial.println("Hora interna do ESP32: " + String(timeString));
}
