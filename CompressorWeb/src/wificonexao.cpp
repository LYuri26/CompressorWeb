#include <WiFi.h>
#include "wificonexao.h"

// Configurações do modo Access Point
const char *ap_ssid = "CompressorWeb";
const char *ap_password = "12345678";

// Configurações de rede WiFi
const char *ssid1 = "CFPFR_WIFI";
const char *password1 = "#CFP-Ur@107!";
const char *ssid2 = "LenonClaro_2.4G";
const char *password2 = "13539406670l";

// Configuração do IP fixo para o modo AP
IPAddress local_ip(192, 168, 4, 1);  // Endereço IP do Access Point
IPAddress gateway(192, 168, 4, 1);   // Gateway (normalmente o mesmo IP)
IPAddress subnet(255, 255, 255, 0); // Máscara de sub-rede

// Variável global para indicar se está no modo AP
bool isAPMode = false;

// Função para configurar a conexão WiFi
void connectToWiFi() {
    Serial.println("Tentando conectar ao WiFi...");

    // Tenta conectar à primeira rede WiFi
    if (tryConnectToWiFi(ssid1, password1)) {
        isAPMode = false;
        return;
    }

    // Se a conexão falhar, tenta conectar à segunda rede WiFi
    Serial.println("Tentando conectar à rede WiFi de backup...");
    if (tryConnectToWiFi(ssid2, password2)) {
        isAPMode = false;
        return;
    }

    // Se ambas as tentativas falharem, entra no modo Access Point
    Serial.println("Falha ao conectar-se a redes WiFi. Entrando no modo Access Point...");
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(local_ip, gateway, subnet); // Configura IP fixo
    WiFi.softAP(ap_ssid, ap_password); // Nome e senha do Access Point
    Serial.print("Modo AP iniciado. Endereço IP: ");
    Serial.println(WiFi.softAPIP());
    
    isAPMode = true; // Atualiza o estado para modo AP

    // Não é necessário configurar o servidor HTTP aqui, faça isso no main.cpp
}

// Função para tentar conectar-se a uma rede WiFi
bool tryConnectToWiFi(const char *ssid, const char *password) {
    Serial.print("Conectando à rede WiFi: ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);
    int attempts = 0;

    // Tenta se conectar até 20 vezes
    while (WiFi.status() != WL_CONNECTED && attempts < 2) {
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
