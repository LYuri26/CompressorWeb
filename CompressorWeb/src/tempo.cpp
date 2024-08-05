#include <WiFi.h>          // Biblioteca para funcionalidades WiFi
#include <NTPClient.h>     // Biblioteca para cliente NTP
#include <WiFiUdp.h>       // Biblioteca para comunicação UDP

// -------------------------------------------------------------------------
// Configurações do NTP
// -------------------------------------------------------------------------
const char* ntpServer = "pool.ntp.org";          // Endereço do servidor NTP
const long gmtOffset_sec = -10800;               // Offset GMT para -3 horas (em segundos)
const int daylightOffset_sec = 0;                // Offset para horário de verão

// -------------------------------------------------------------------------
// Instâncias
// -------------------------------------------------------------------------
WiFiUDP ntpUDP;                                 // Instância do cliente UDP para comunicação NTP
NTPClient timeClient(ntpUDP, ntpServer, 0);     // Instância do cliente NTP, com offset inicial 0

// -------------------------------------------------------------------------
// Variáveis Globais
// -------------------------------------------------------------------------
String currentTime = "";                        // Armazena o tempo atual em formato de string

// -------------------------------------------------------------------------
// Declarações de Funções
// -------------------------------------------------------------------------
void updateTime();                             // Atualiza o tempo atual a partir do servidor NTP
void setTimeFromNTP();                         // Configura o tempo do dispositivo com base no NTP
void printInternalTime();                      // Imprime o tempo interno do dispositivo

// -------------------------------------------------------------------------
// Função de Configuração do Cliente NTP
// -------------------------------------------------------------------------
void setupTimeClient() {
    timeClient.begin();                        // Inicializa o cliente NTP
    updateTime();                              // Atualiza o tempo inicial a partir do servidor NTP
    setTimeFromNTP();                          // Configura o tempo interno com base no servidor NTP
    Serial.print("Hora inicial da Internet: ");
    Serial.println(currentTime);               // Imprime a hora inicial obtida do NTP
}

// -------------------------------------------------------------------------
// Função para Obter o Tempo Atual do Cliente NTP
// -------------------------------------------------------------------------
String getTimeClient() {
    return currentTime;                        // Retorna a hora atual armazenada
}

// -------------------------------------------------------------------------
// Função para Atualizar o Tempo
// -------------------------------------------------------------------------
void updateTime() {
    if (WiFi.status() == WL_CONNECTED) {       // Verifica se o WiFi está conectado
        timeClient.update();                   // Atualiza o tempo a partir do servidor NTP
        unsigned long epochTime = timeClient.getEpochTime(); // Obtém o tempo atual em epoch (segundos desde 1970)
        time_t time = static_cast<time_t>(epochTime);       // Converte para o tipo time_t
        struct tm timeInfo;
        localtime_r(&time, &timeInfo);         // Converte o tempo para a estrutura tm
        char timeString[20];
        snprintf(timeString, sizeof(timeString), "%04d-%02d-%02dT%02d:%02d", 
                 timeInfo.tm_year + 1900, timeInfo.tm_mon + 1, timeInfo.tm_mday, 
                 timeInfo.tm_hour, timeInfo.tm_min); // Formata a string de tempo
        currentTime = String(timeString);       // Atualiza a variável currentTime

        Serial.println("Hora atual da Internet: " + currentTime); // Imprime a hora atual obtida do NTP
    } else {
        Serial.println("WiFi desconectado. Não é possível atualizar o tempo."); // Mensagem de erro
    }
}

// -------------------------------------------------------------------------
// Função para Configurar o Tempo Interno com NTP
// -------------------------------------------------------------------------
void setTimeFromNTP() {
    if (WiFi.status() == WL_CONNECTED) {       // Verifica se o WiFi está conectado
        configTime(gmtOffset_sec, daylightOffset_sec, ntpServer); // Configura o tempo local com base no servidor NTP

        struct tm timeInfo;
        if (getLocalTime(&timeInfo)) {         // Obtém a hora local
            Serial.print("Hora interna configurada para: ");
            Serial.println(asctime(&timeInfo)); // Imprime a hora interna configurada
        } else {
            Serial.println("Falha ao obter hora NTP."); // Mensagem de erro
        }
    } else {
        Serial.println("WiFi desconectado. Não foi possível configurar a hora interna."); // Mensagem de erro
    }
}

// -------------------------------------------------------------------------
// Função para Imprimir o Tempo Interno do Dispositivo
// -------------------------------------------------------------------------
void printInternalTime() {
    time_t now;
    struct tm timeinfo;
    time(&now);                               // Obtém o tempo atual
    localtime_r(&now, &timeinfo);             // Converte o tempo para a estrutura tm
    char timeString[20];
    snprintf(timeString, sizeof(timeString), "%02d-%02d-%04dT%02d:%02d:%02d",
             timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900,
             timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec); // Formata a string de tempo
    Serial.println("Hora interna do ESP32: " + String(timeString)); // Imprime a hora interna do ESP32
}
