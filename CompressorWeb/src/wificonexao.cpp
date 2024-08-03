#include <WiFi.h>
#include <EEPROM.h>
#include "wificonexao.h"

// Protótipo da função startAccessPoint()
void startAccessPoint();

// Configurações do modo Access Point
const char *ap_ssid = "CompressorWeb";
const char *ap_password = "12345678";

// Configuração do IP fixo para o modo AP
IPAddress local_ip(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

// Credenciais padrão
const char *default_ssid = "LenonClaro_2.4G";
const char *default_password = "13539406670";

// Função para conectar-se ao WiFi
void connectToWiFi() {
    EEPROM.begin(512);
    String ssid = readStringFromEEPROM(0);
    String password = readStringFromEEPROM(100);

    // Tenta conectar com as credenciais salvas
    if (ssid.length() > 0 && password.length() > 0) {
        Serial.printf("Tentando conectar à rede WiFi salva: %s\n", ssid.c_str());
        if (tryConnectToWiFi(ssid.c_str(), password.c_str())) {
            return; // Conectado com sucesso
        }
    }

    // Se as credenciais salvas falharem, tenta conectar com as credenciais padrão
    Serial.println("Falha ao conectar com credenciais salvas ou nenhuma rede salva encontrada. Tentando conectar com credenciais padrão...");
    if (tryConnectToWiFi(default_ssid, default_password)) {
        return; // Conectado com sucesso
    }

    // Se falhar ao conectar com as credenciais padrão, entra no modo Access Point
    Serial.println("Falha ao conectar com credenciais padrão. Entrando no modo Access Point...");
    startAccessPoint();
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
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(local_ip, gateway, subnet);
    WiFi.softAP(ap_ssid, ap_password);
    Serial.printf("Modo AP iniciado. Endereço IP: %s\n", WiFi.softAPIP().toString().c_str());
}

// Função para ler uma string da EEPROM
String readStringFromEEPROM(int startAddress) {
    String str = "";
    char c = EEPROM.read(startAddress);
    while (c != '\0') {
        str += c;
        startAddress++;
        c = EEPROM.read(startAddress);
    }
    return str;
}
