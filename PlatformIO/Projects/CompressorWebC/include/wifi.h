#ifndef WIFI_H
#define WIFI_H

#include <WiFi.h>

// Função para conectar ao WiFi
void connectToWiFi();

// Função auxiliar para tentar conectar a uma rede WiFi
bool tryConnectToWiFi(const char *ssid, const char *password);

#endif // WIFI_H
