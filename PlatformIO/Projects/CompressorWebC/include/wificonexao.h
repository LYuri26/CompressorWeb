#ifndef WIFICONEXAO_H
#define WIFICONEXAO_H

#include <WebServer.h>
#include <RTClib.h>
#include <WiFi.h>

// Declaração das credenciais de WiFi
extern const char *ssid1;
extern const char *password1;
extern const char *ssid2;
extern const char *password2;
extern const char *ap_ssid;
extern const char *ap_password;

// Declaração das variáveis de rede
extern IPAddress local_IP;
extern IPAddress gateway;
extern IPAddress subnet;
extern IPAddress primaryDNS;
extern IPAddress secondaryDNS;

// Declaração dos objetos globais
extern WebServer server;
extern RTC_DS3231 rtc;

// Declaração das funções de conexão WiFi
void connectToWiFi();
bool tryConnectToWiFi(const char *ssid, const char *password, bool useStaticIP);

#endif // WIFICONEXAO_H
