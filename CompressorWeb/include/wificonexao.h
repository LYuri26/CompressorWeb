#ifndef WIFICONEXAO_H
#define WIFICONEXAO_H

#include <WiFi.h>
#include <ESPAsyncWebServer.h>

// Declaração das variáveis de configuração WiFi
extern const char *ssid;
extern const char *password;


// Declaração das variáveis de configuração do Access Point
extern const char* ssid;
extern const char* password;

void connectToWiFi(const char *ssid, const char *password);

// Declaração da variável de estado
extern bool isAPMode;

// Declaração das funções para conectar ao WiFi e modo Access Point
void enterAPMode(); // Adiciona a função para entrar no modo Access Point
void loadSavedWiFiNetworks();
void checkScanResults();


#endif // WIFICONEXAO_H
