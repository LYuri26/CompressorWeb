#include <WiFi.h>

// Declarações de variáveis para SSIDs e senhas
const char *ssid1 = "CFPFR_WIFI";
const char *password1 = "#CFP-Ur@107!";
const char *ssid2 = "LenonClaro_2.4G";
const char *password2 = "13539406670";

// Nome e senha do Access Point
const char *apSSID = "CompressorWeb";
const char *apPassword = "12345678";

// Função para configurar o WiFi em modo Access Point
void setupAccessPoint()
{
    WiFi.softAP(apSSID, apPassword);
    Serial.print("Access Point configurado. IP do AP: ");
    Serial.println(WiFi.softAPIP());
}

// Função para tentar conectar ao WiFi
bool tryConnectToWiFi(const char *ssid, const char *password)
{
    Serial.print("Tentando conectar à rede WiFi: ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 10)
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
        return true;
    }
    else
    {
        Serial.println();
        Serial.println("Falha ao conectar-se ao WiFi");
        return false;
    }
}

// Função principal para conectar ao WiFi
void connectToWiFi()
{
    Serial.println("Tentando conectar ao WiFi...");

    // Tenta conectar à primeira rede WiFi
    if (tryConnectToWiFi(ssid1, password1))
    {
        return;
    }

    // Se falhar, tenta conectar à segunda rede WiFi
    if (tryConnectToWiFi(ssid2, password2))
    {
        return;
    }

    // Se falhar em ambas as redes, ativa o modo Access Point
    Serial.println("Falha ao conectar-se às redes WiFi conhecidas.");
    setupAccessPoint();
}
