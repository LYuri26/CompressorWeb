#ifndef WIFICONEXAO_H
#define WIFICONEXAO_H

#include <WiFi.h>

// Declaração das variáveis de configuração WiFi
extern const char *ssid1;
extern const char *password1;
extern const char *ssid2;
extern const char *password2;

// Declaração da função para conectar ao WiFi
void connectToWiFi();

// Declaração da função auxiliar para tentar conectar a uma rede WiFi
bool tryConnectToWiFi(const char *ssid, const char *password, bool useStaticIP);

#endif // WIFICONEXAO_H
