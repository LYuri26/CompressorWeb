#ifndef WIFICONEXAO_H
#define WIFICONEXAO_H

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

// -------------------------------------------------------------------------
// 1. Definições e Constantes de Rede
// -------------------------------------------------------------------------
#define MAX_SSID_LEN 32
#define MAX_PASS_LEN 64
#define WIFI_RECONNECT_TIMEOUT_MS 15000
#define WIFI_SCAN_INTERVAL_MS 5000

// -------------------------------------------------------------------------
// 2. Enumeração da Máquina de Estados de Conectividade
// -------------------------------------------------------------------------
typedef enum
{
    WIFI_STATE_IDLE,
    WIFI_STATE_CONNECTING,
    WIFI_STATE_CONNECTED,
    WIFI_STATE_DISCONNECTED,
    WIFI_STATE_FAILED,
    WIFI_STATE_AP_MODE
} wifi_system_state_t;

// -------------------------------------------------------------------------
// 3. Declaração de Variáveis Globais de Configuração e Estado
// -------------------------------------------------------------------------

/**
 * Ponteiro constante para o SSID atualmente selecionado para conexão.
 */
extern const char *ssid;

/**
 * Ponteiro constante para a senha da rede sem fio selecionada.
 */
extern const char *password;

/**
 * Flag booleana que indica se o ESP32 está operando em Modo Ponto de Acesso (AP).
 */
extern bool isAPMode;

/**
 * Flag booleana indicando se há uma transição de conexão em andamento.
 */
extern bool wifiConectando;

/**
 * Flag que sinaliza se uma varredura de redes sem fio está em execução.
 */
extern bool scanning;

/**
 * Timestamp do último escaneamento de redes sem fio executado.
 */
extern unsigned long lastScan;

/**
 * Intervalo configurado entre escaneamentos de redes Wi-Fi (ms).
 */
extern const unsigned long scanInterval;

/**
 * Quantidade de redes sem fio encontradas no último escaneamento.
 */
extern int scanResults;

/**
 * Configurações de endereçamento estático para o modo Access Point.
 */
extern IPAddress local_ip;
extern IPAddress gateway;
extern IPAddress subnet;

// -------------------------------------------------------------------------
// 4. Declaração de Funções de Conectividade e Operação Não-Bloqueante
// -------------------------------------------------------------------------

void setupAP();
void enterAPMode();
void connectToWiFi(const char *ssid, const char *password);
void processarConexaoWiFi();
void desconectarWiFi();
void loadSavedWiFiNetworks();
void startScanWiFiNetworks();
void checkScanResults();
String getScanResults();

// -------------------------------------------------------------------------
// 5. Funções de Diagnóstico e Métricas de Rede
// -------------------------------------------------------------------------
bool isWiFiConnected();
int getWiFiSignalQuality();
String getWiFiStatusJSON();
const char *getWiFiStateString();

#endif // WIFICONEXAO_H