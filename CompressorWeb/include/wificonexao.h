#ifndef WIFICONEXAO_H
#define WIFICONEXAO_H

#include <WiFi.h>
#include <ESPAsyncWebServer.h>

// Declaração das variáveis de configuração WiFi
extern const char *ssid1;
extern const char *password1;
extern const char *ssid2;
extern const char *password2;

// Declaração das variáveis de configuração do Access Point
extern const char *ap_ssid;
extern const char *ap_password;

// Declaração da variável de estado
extern bool isAPMode;

// Declaração da função para conectar ao WiFi
void connectToWiFi();
void setupServer();
// Declaração da função auxiliar para tentar conectar a uma rede WiFi
bool tryConnectToWiFi(const char *ssid, const char *password);
extern AsyncWebServer server;
#endif // WIFICONEXAO_H
