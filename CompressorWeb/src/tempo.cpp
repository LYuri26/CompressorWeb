#include "tempo.h"
#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

// Configurações do servidor NTP e fuso horário
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -10800;  // Offset de -3 horas em segundos
const int daylightOffset_sec = 0;

// Configuração do cliente NTP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, 0);  // Usar 0 para o fuso horário, ajustar manualmente

String currentTime = "";

// Declaração de funções
void setupTimeClient();
String getTimeClient();
void updateTime();
void setTimeFromNTP();
void updateCurrentTime();
String getInternTime();
void printTimes();
bool isTimeSynced(); // Adicionada função para verificar sincronização

void setupTimeClient() {
    timeClient.begin();
    Serial.println("Aguardando sincronização com o servidor NTP...");
    
    // Aguarda a sincronização com o servidor NTP
    unsigned long startMillis = millis();
    while (!isTimeSynced() && millis() - startMillis < 10000) {
        delay(1000); // Aguarda 1 segundo antes de tentar novamente
    }
    
    // Verifica se a sincronização foi bem-sucedida
    if (isTimeSynced()) {
        updateTime(); // Atualiza o tempo após a sincronização
        setTimeFromNTP();
        Serial.print("Hora inicial da Internet: ");
        Serial.println(currentTime);
    } else {
        Serial.println("Falha na sincronização com o servidor NTP.");
    }
}

String getTimeClient() {
    return currentTime;
}

void updateTime() {
    if (WiFi.status() == WL_CONNECTED) {
        timeClient.update();
        updateCurrentTime();
        
        // Exibir as horas se for o início de um novo minuto
        if (timeClient.getSeconds() == 0) {
            printTimes();
        }
    } else {
        Serial.println("WiFi desconectado");
    }
}

void setTimeFromNTP() {
    if (WiFi.status() == WL_CONNECTED) {
        configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

        struct tm timeInfo;
        if (getLocalTime(&timeInfo)) {
            Serial.print("Hora interna configurada para: ");
            Serial.println(asctime(&timeInfo));
        } else {
            Serial.println("Falha ao obter hora NTP.");
        }
    } else {
        Serial.println("WiFi desconectado. Não foi possível configurar a hora interna.");
    }
}

void updateCurrentTime() {
    unsigned long epochTime = timeClient.getEpochTime();
    time_t time = static_cast<time_t>(epochTime);
    struct tm timeInfo;
    localtime_r(&time, &timeInfo);
    char timeString[20];
    snprintf(timeString, sizeof(timeString), "%04d-%02d-%02dT%02d:%02d", 
             timeInfo.tm_year + 1900, timeInfo.tm_mon + 1, timeInfo.tm_mday, 
             timeInfo.tm_hour, timeInfo.tm_min);
    currentTime = String(timeString);
}

String getInternTime() {
    time_t now;
    struct tm timeInfo;
    time(&now);
    localtime_r(&now, &timeInfo);
    char timeString[20];
    snprintf(timeString, sizeof(timeString), "%04d-%02d-%02dT%02d:%02d", 
             timeInfo.tm_year + 1900, timeInfo.tm_mon + 1, timeInfo.tm_mday, 
             timeInfo.tm_hour, timeInfo.tm_min);
    return String(timeString);
}

void printTimes() {
    Serial.println("Hora atual da Internet: " + currentTime);
    Serial.println("Hora interna do ESP32: " + getInternTime());
}

// Função para verificar se o tempo foi sincronizado
bool isTimeSynced() {
    return timeClient.getEpochTime() > 0; // Retorna true se o tempo for maior que zero
}
