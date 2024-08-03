#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "wificonexao.h"

// Protótipo da função startAccessPoint()
void startAccessPoint();

// Configurações do modo Access Point
const char *ap_ssid = "CompressorWeb";
const char *ap_password = "12345678";

// Configurações de rede WiFi
const char *ssid1 = "CFPFR_WIFI";
const char *password1 = "#CFP-Ur@107!";
const char *ssid2 = "LenonClaro_2.4G";
const char *password2 = "13539406670";

// Configuração do IP fixo para o modo AP
IPAddress local_ip(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

// Funções
void connectToWiFi() {
    Serial.println("Tentando conectar ao WiFi...");

    if (tryConnectToWiFi(ssid1, password1) || tryConnectToWiFi(ssid2, password2)) {
        Serial.print("Conectado ao WiFi. Endereço IP: ");
        Serial.println(WiFi.localIP());
    } else {
        startAccessPoint();
    }
}

bool tryConnectToWiFi(const char *ssid, const char *password) {
    Serial.printf("Conectando à rede WiFi: %s\n", ssid);

    WiFi.begin(ssid, password);
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(1000);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.printf("\nConectado com sucesso! Endereço IP: %s\n", WiFi.localIP().toString().c_str());
        return true;
    } else {
        Serial.printf("\nFalha ao conectar-se ao WiFi. Status: %d\n", WiFi.status());
        return false;
    }
}

void startAccessPoint() {
    Serial.println("Falha ao conectar-se a redes WiFi. Entrando no modo Access Point...");
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(local_ip, gateway, subnet);
    WiFi.softAP(ap_ssid, ap_password);
    Serial.printf("Modo AP iniciado. Endereço IP: %s\n", WiFi.softAPIP().toString().c_str());
}
