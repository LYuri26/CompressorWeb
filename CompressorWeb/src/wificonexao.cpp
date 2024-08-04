#include <WiFi.h>
#include "wificonexao.h"
#include <SPIFFS.h>
#include <vector>

bool scanning = false;
unsigned long lastScan = 0;
const unsigned long scanInterval = 5000; // Intervalo de escaneamento (5 segundos)

// Defina as variáveis ssid e password
const char* ssid = "";
const char* password = "";

const char *ap_ssid = "CompressorWeb";
const char *ap_password = "12345678";

IPAddress local_ip(192, 168, 4, 1); // Endereço IP do Access Point
IPAddress gateway(192, 168, 4, 1);  // Gateway (normalmente o mesmo IP)
IPAddress subnet(255, 255, 255, 0); // Máscara de sub-rede

bool isAPMode = false;
bool scanDone = false;
int scanResults = 0;

void connectToWiFi(const char *ssid, const char *password) {
    if (!ssid || !password || strlen(ssid) == 0 || strlen(password) == 0) {
        Serial.println("SSID ou senha inválidos.");
        enterAPMode();
        return;
    }

    Serial.println("Tentando conectar ao WiFi...");
    WiFi.begin(ssid, password);
    int attempts = 0;
    const int maxAttempts = 5;

    while (WiFi.status() != WL_CONNECTED && attempts < maxAttempts) {
        delay(1000);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println();
        Serial.print("Conectado com sucesso! Endereço IP: ");
        Serial.println(WiFi.localIP());
        isAPMode = false;
    } else {
        Serial.println();
        Serial.println("Falha ao conectar-se ao WiFi");
        enterAPMode();
    }
}

void enterAPMode() {
    Serial.println("Entrando no modo Access Point...");
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(local_ip, gateway, subnet);
    WiFi.softAP(ap_ssid, ap_password);
    Serial.print("Modo AP iniciado. Endereço IP: ");
    Serial.println(WiFi.softAPIP());

    isAPMode = true;
}

void startScanWiFiNetworks() {
    if (!scanning && millis() - lastScan > scanInterval) {
        scanning = true;
        WiFi.scanNetworks(true);
        lastScan = millis();
    }
}

String getScanResults() {
    int n = WiFi.scanComplete();
    if (n == -2) { // Scan not started
        return "[]";
    } else if (n == -1) { // Scan in progress
        return "[]";
    } else if (n == 0) { // No networks found
        return "[]";
    } else { // Networks found
        String json = "[";
        for (int i = 0; i < n; i++) {
            json += "{\"ssid\":\"" + WiFi.SSID(i) + "\"}";
            if (i < n - 1) json += ",";
        }
        json += "]";
        scanning = false;
        return json;
    }
}

void checkScanResults() {
    if (WiFi.scanComplete() != WIFI_SCAN_RUNNING) {
        scanDone = true;
        scanResults = WiFi.scanComplete();
        scanning = false;
    }
}

void loadSavedWiFiNetworks() {
    File file = SPIFFS.open("/wifiredes.txt", FILE_READ);
    if (!file) {
        Serial.println("Erro ao abrir o arquivo de redes Wi-Fi");
        return;
    }

    String content = file.readString();
    file.close();

    String ssid;
    String password;

    int start = 0;
    while (start < content.length()) {
        int end = content.indexOf('\n', start);
        if (end == -1) {
            end = content.length();
        }
        String line = content.substring(start, end);
        int commaIndex = line.indexOf(',');
        if (commaIndex != -1) {
            ssid = line.substring(0, commaIndex);
            password = line.substring(commaIndex + 1);
            connectToWiFi(ssid.c_str(), password.c_str());
            if (WiFi.status() == WL_CONNECTED) {
                return; // Conectado com sucesso, saia da função
            }
        }
        start = end + 1;
    }
}
