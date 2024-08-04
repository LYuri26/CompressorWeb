#ifndef WIFIGERENCIADOR_H
#define WIFIGERENCIADOR_H

#include <ESPAsyncWebServer.h>

void setupWiFiManagementPage(AsyncWebServer& server);


// Função para conectar ao WiFi
void connectToWiFi(const char *ssid, const char *password);

// Função para entrar no modo Access Point
void enterAPMode();

// Função para escanear redes WiFi (assíncrono)
void scanWiFiNetworksAsync();

// Função para processar os resultados do escaneamento
void processScanResults();

// Função para obter os resultados do escaneamento
String getScanResults();

void startScanWiFiNetworks();

extern bool scanning;
extern unsigned long lastScan;
extern const unsigned long scanInterval; // Intervalo de escaneamento (5 segundos)


#endif // WIFIGERENCIADOR_H
