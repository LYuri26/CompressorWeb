#include "tempo.h"

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -10800; // Offset de -3 horas em segundos
const int daylightOffset_sec = 0;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, 0); // Usar 0 para o fuso horário, vamos ajustar manualmente

String currentTime = "";

// Declarações de funções
void updateTime();
void printInternalTime();

void setupTimeClient()
{
    timeClient.begin();
    updateTime();
    setTimeFromNTP();
    Serial.print("Hora inicial da Internet: ");
    Serial.println(currentTime);
}

String getTimeClient()
{
    return currentTime;
}

void updateTime() {
    if (WiFi.status() == WL_CONNECTED) {
        timeClient.update();
        unsigned long epochTime = timeClient.getEpochTime();
        time_t time = static_cast<time_t>(epochTime);
        struct tm timeInfo;
        localtime_r(&time, &timeInfo);
        char timeString[20];
        snprintf(timeString, sizeof(timeString), "%04d-%02d-%02dT%02d:%02d", 
                 timeInfo.tm_year + 1900, timeInfo.tm_mon + 1, timeInfo.tm_mday, 
                 timeInfo.tm_hour, timeInfo.tm_min);
        currentTime = String(timeString);

        Serial.println("Hora atual da Internet: " + currentTime);
    } else {
        Serial.println("WiFi desconectado. Não é possível atualizar o tempo.");
    }
}

void setTimeFromNTP()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

        struct tm timeInfo;
        if (getLocalTime(&timeInfo))
        {
            Serial.print("Hora interna configurada para: ");
            Serial.println(asctime(&timeInfo));
        }
        else
        {
            Serial.println("Falha ao obter hora NTP.");
        }
    }
    else
    {
        Serial.println("WiFi desconectado. Não foi possível configurar a hora interna.");
    }
}

void printInternalTime()
{
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    char timeString[20];
    snprintf(timeString, sizeof(timeString), "%02d-%02d-%04dT%02d:%02d",
             timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
             timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    Serial.println("Hora interna do ESP32: " + String(timeString));
}
