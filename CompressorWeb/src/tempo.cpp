#include <WiFi.h>      // Biblioteca para funcionalidades WiFi
#include <NTPClient.h> // Biblioteca para cliente NTP (Network Time Protocol)
#include <WiFiUdp.h>   // Biblioteca para comunicação UDP (User Datagram Protocol)

// -------------------------------------------------------------------------
// Configurações do NTP
// -------------------------------------------------------------------------
const char *ntpServer = "pool.ntp.org"; // Endereço do servidor NTP público usado para sincronização de tempo
const long gmtOffset_sec = -10800;      // Offset GMT para -3 horas (em segundos), usado para ajustar o tempo de acordo com o fuso horário
const int daylightOffset_sec = 0;       // Offset para horário de verão, definido como 0 se não for usado

// -------------------------------------------------------------------------
// Instâncias
// -------------------------------------------------------------------------
WiFiUDP ntpUDP;                             // Instância do cliente UDP para comunicação com o servidor NTP
NTPClient timeClient(ntpUDP, ntpServer, 0); // Instância do cliente NTP, usando o cliente UDP criado e o servidor NTP especificado, com offset inicial 0

// -------------------------------------------------------------------------
// Variáveis Globais
// -------------------------------------------------------------------------
String currentTime = ""; // Armazena o tempo atual em formato de string, para ser usado em outras partes do código

// -------------------------------------------------------------------------
// Declarações de Funções
// -------------------------------------------------------------------------
void updateTime();        // Atualiza o tempo atual a partir do servidor NTP
void setTimeFromNTP();    // Configura o tempo do dispositivo com base no NTP
void printInternalTime(); // Imprime o tempo interno do dispositivo no monitor serial

// -------------------------------------------------------------------------
// Função de Configuração do Cliente NTP
// -------------------------------------------------------------------------
void setupTimeClient()
{
    timeClient.begin(); // Inicializa o cliente NTP, preparando-o para enviar e receber pacotes UDP
    updateTime();       // Atualiza o tempo inicial a partir do servidor NTP
    setTimeFromNTP();   // Configura o tempo interno do dispositivo com base no servidor NTP
    Serial.print("Hora inicial da Internet: ");
    Serial.println(currentTime); // Imprime a hora inicial obtida do servidor NTP no monitor serial
}

// -------------------------------------------------------------------------
// Função para Obter o Tempo Atual do Cliente NTP
// -------------------------------------------------------------------------
String getTimeClient()
{
    return currentTime; // Retorna a hora atual armazenada na variável global currentTime
}

// -------------------------------------------------------------------------
// Função para Atualizar o Tempo
// -------------------------------------------------------------------------
void updateTime()
{
    if (WiFi.status() == WL_CONNECTED)
    {                                                        // Verifica se o dispositivo está conectado a uma rede Wi-Fi
        timeClient.update();                                 // Atualiza o tempo a partir do servidor NTP, realizando uma nova requisição
        unsigned long epochTime = timeClient.getEpochTime(); // Obtém o tempo atual em formato epoch (segundos desde 1º de janeiro de 1970)
        time_t time = static_cast<time_t>(epochTime);        // Converte o tempo epoch para o tipo time_t, que é utilizado para manipulação de datas e horas
        struct tm timeInfo;
        localtime_r(&time, &timeInfo); // Converte o tempo em time_t para uma estrutura tm, que representa a data e a hora local
        char timeString[20];
        snprintf(timeString, sizeof(timeString), "%04d-%02d-%02dT%02d:%02d",
                 timeInfo.tm_year + 1900, timeInfo.tm_mon + 1, timeInfo.tm_mday,
                 timeInfo.tm_hour, timeInfo.tm_min); // Formata a string de tempo para o formato "YYYY-MM-DDTHH:MM"
        currentTime = String(timeString);            // Atualiza a variável global currentTime com a nova hora formatada

        Serial.println("Hora atual da Internet: " + currentTime); // Imprime a hora atual obtida do servidor NTP no monitor serial
    }
    else
    {
        Serial.println("WiFi desconectado. Não é possível atualizar o tempo."); // Mensagem de erro se o WiFi não estiver conectado
    }
}

// -------------------------------------------------------------------------
// Função para Configurar o Tempo Interno com NTP
// -------------------------------------------------------------------------
void setTimeFromNTP()
{
    if (WiFi.status() == WL_CONNECTED)
    {                                                             // Verifica se o dispositivo está conectado a uma rede Wi-Fi
        configTime(gmtOffset_sec, daylightOffset_sec, ntpServer); // Configura o tempo local com base no servidor NTP e os offsets fornecidos (fuso horário e horário de verão)

        struct tm timeInfo;
        if (getLocalTime(&timeInfo))
        { // Obtém a hora local a partir do sistema, usando o tempo configurado pelo NTP
            Serial.print("Hora interna configurada para: ");
            Serial.println(asctime(&timeInfo)); // Imprime a hora interna configurada no monitor serial, usando a função asctime para formatar a hora
        }
        else
        {
            Serial.println("Falha ao obter hora NTP."); // Mensagem de erro se a hora local não puder ser obtida
        }
    }
    else
    {
        Serial.println("WiFi desconectado. Não foi possível configurar a hora interna."); // Mensagem de erro se o WiFi não estiver conectado
    }
}

// -------------------------------------------------------------------------
// Função para Imprimir o Tempo Interno do Dispositivo
// -------------------------------------------------------------------------
void printInternalTime()
{
    time_t now;
    struct tm timeinfo;
    time(&now);                   // Obtém o tempo atual do sistema em formato time_t
    localtime_r(&now, &timeinfo); // Converte o tempo em time_t para uma estrutura tm
    char timeString[20];
    snprintf(timeString, sizeof(timeString), "%02d-%02d-%04dT%02d:%02d:%02d",
             timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900,
             timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);   // Formata a string de tempo para o formato "DD-MM-YYYYTHH:MM:SS"
    Serial.println("Hora interna do ESP32: " + String(timeString)); // Imprime a hora interna do ESP32 no monitor serial
}
