#include "wificonexao.h"

RTC_DS3231 rtc;        // Definição correta da variável rtc

// Definição das credenciais de WiFi
const char *ssid1 = "CFPFR_WIFI";
const char *password1 = "#CFP-Ur@107!";
const char *ssid2 = "LenonClaro_2.4G";
const char *password2 = "13539406670";
const char *ap_ssid = "CompressorWeb";
const char *ap_password = "12345678";

// Definição das variáveis de rede
IPAddress local_IP(10, 107, 0, 47);
IPAddress gateway(10, 107, 0, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);   // Google DNS
IPAddress secondaryDNS(8, 8, 4, 4); // Google DNS

// Função para tentar conectar a uma rede WiFi
bool tryConnectToWiFi(const char *ssid, const char *password, bool useStaticIP)
{
    Serial.print("Conectando à rede WiFi: ");
    Serial.println(ssid);

    // Configura o WiFi para usar IP fixo se necessário
    if (useStaticIP)
    {
        // Configuração IP estático (descomente se necessário)
        /*
        bool configResult = WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);
        if (!configResult)
        {
            Serial.println("Falha ao configurar IP fixo. Tentando com IP dinâmico.");
        }
        */
    }
    else
    {
        // Reseta a configuração de IP estático para usar IP dinâmico
        WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
    }

    WiFi.begin(ssid, password);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20)
    {
        delay(1000);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println();
        Serial.print("Conectado com sucesso! Endereço IP: ");
        Serial.println(WiFi.localIP());
        Serial.print("Tentativas de conexão: ");
        Serial.println(attempts);
        return true;
    }
    else
    {
        Serial.println();
        Serial.println("Falha ao conectar-se ao WiFi");
        Serial.print("Status: ");
        Serial.println(WiFi.status());
        Serial.print("Tentativas de conexão: ");
        Serial.println(attempts);
        return false;
    }
}

// Função para conectar ao WiFi
void connectToWiFi()
{
    Serial.println("Tentando conectar ao WiFi...");

    // Tenta conectar à primeira rede WiFi, se falhar, tenta a segunda
    bool connected = tryConnectToWiFi(ssid1, password1, true); // Tenta com IP fixo
    if (!connected)
    {
        Serial.println("Tentando conectar à rede WiFi de backup...");
        connected = tryConnectToWiFi(ssid2, password2, true); // Tenta com IP fixo
    }

    // Se a conexão falhar, tenta novamente com IP dinâmico
    while (!connected)
    {
        Serial.println("Falha ao conectar-se ao WiFi com IP fixo. Tentando novamente com IP dinâmico...");
        delay(5000);                                           // Aguarda 5 segundos antes de tentar novamente
        connected = tryConnectToWiFi(ssid1, password1, false); // Tenta com IP dinâmico
        if (!connected)
        {
            connected = tryConnectToWiFi(ssid2, password2, false); // Tenta com IP dinâmico
        }

        // Se ainda não estiver conectado, entra no modo Access Point
        if (!connected)
        {
            Serial.println("Falha ao conectar-se a redes WiFi. Entrando no modo Access Point...");
            WiFi.mode(WIFI_AP);
            WiFi.softAP(ap_ssid, ap_password); // Nome e senha do Access Point
            Serial.print("Modo AP iniciado. Endereço IP: ");
            Serial.println(WiFi.softAPIP());
            break; // Sai do loop
        }
    }
}
