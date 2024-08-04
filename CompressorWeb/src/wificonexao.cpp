#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "wificonexao.h"

// Configurações do modo Access Point
const char *ap_ssid = "CompressorWeb";
const char *ap_password = "12345678";

// Configurações de rede WiFi
const char *ssid1 = "CFPFR_WIFI";
const char *password1 = "#CFP-Ur@107!";
const char *ssid2 = "LenonClaro_2.4G";
const char *password2 = "13539406670";

// Configuração do IP fixo para o modo AP
IPAddress local_ip(192, 168, 4, 1);  // Endereço IP do Access Point
IPAddress gateway(192, 168, 4, 1);   // Gateway (normalmente o mesmo IP)
IPAddress subnet(255, 255, 255, 0); // Máscara de sub-rede

// Funções
void connectToWiFi() {
    Serial.println("Tentando conectar ao WiFi...");

    // Tenta conectar à primeira rede WiFi
    bool connected = tryConnectToWiFi(ssid1, password1);

    // Se a conexão falhar, tenta conectar à segunda rede WiFi
    if (!connected) {
        Serial.println("Tentando conectar à rede WiFi de backup...");
        connected = tryConnectToWiFi(ssid2, password2);
    }

    // Se a conexão falhar, entra no modo Access Point
    if (!connected) {
        Serial.println("Falha ao conectar-se a redes WiFi. Entrando no modo Access Point...");
        WiFi.mode(WIFI_AP);
        WiFi.softAPConfig(local_ip, gateway, subnet); // Configura IP fixo
        WiFi.softAP(ap_ssid, ap_password); // Nome e senha do Access Point
        Serial.print("Modo AP iniciado. Endereço IP: ");
        Serial.println(WiFi.softAPIP());
    } else {
        Serial.print("Conectado ao WiFi. Endereço IP: ");
        Serial.println(WiFi.localIP());
    }
}

bool tryConnectToWiFi(const char *ssid, const char *password) {
    Serial.print("Conectando à rede WiFi: ");
    Serial.println(ssid);

    // Conecta ao WiFi
    WiFi.begin(ssid, password);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(1000);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println();
        Serial.print("Conectado com sucesso! Endereço IP: ");
        Serial.println(WiFi.localIP());
        Serial.print("Tentativas de conexão: ");
        Serial.println(attempts);
        return true;
    } else {
        Serial.println();
        Serial.println("Falha ao conectar-se ao WiFi");
        Serial.print("Status: ");
        Serial.println(WiFi.status());
        Serial.print("Tentativas de conexão: ");
        Serial.println(attempts);
        return false;
    }
}
