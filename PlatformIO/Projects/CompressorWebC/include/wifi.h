#ifndef WIFI_H
#define WIFI_H

// Inclui a biblioteca WiFi.h, que fornece funções e classes para conectar e gerenciar conexões WiFi
#include <WiFi.h>

// Declaração da função para conectar ao WiFi
void connectToWiFi();

// Declaração da função auxiliar para tentar conectar a uma rede WiFi
bool tryConnectToWiFi(const char *ssid, const char *password);

#endif // WIFI_H
