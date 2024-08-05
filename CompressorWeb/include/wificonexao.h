#ifndef WIFICONEXAO_H
#define WIFICONEXAO_H

#include <WiFi.h>               // Inclui a biblioteca para conexão WiFi
#include <ESPAsyncWebServer.h> // Inclui a biblioteca para o servidor web assíncrono

// -------------------------------------------------------------------------
// Declaração de Variáveis de Configuração WiFi
// -------------------------------------------------------------------------

/**
 * Nome da rede WiFi.
 * 
 * @note Esta variável armazena o SSID da rede WiFi à qual o dispositivo deve se conectar.
 */
extern const char *ssid;

/**
 * Senha da rede WiFi.
 * 
 * @note Esta variável armazena a senha necessária para conectar-se à rede WiFi especificada pelo SSID.
 */
extern const char *password;

// -------------------------------------------------------------------------
// Declaração da Variável de Estado
// -------------------------------------------------------------------------

/**
 * Indica se o dispositivo está no modo Access Point (AP).
 * 
 * @note Esta variável é usada para verificar o estado atual do dispositivo (modo AP ou cliente WiFi).
 */
extern bool isAPMode;

// -------------------------------------------------------------------------
// Declaração de Funções para Conectar ao WiFi e Modo Access Point
// -------------------------------------------------------------------------

/**
 * Conecta ao WiFi usando o SSID e a senha fornecidos.
 * 
 * @param ssid Nome da rede WiFi.
 * @param password Senha da rede WiFi.
 * 
 * @note Esta função tenta estabelecer uma conexão com a rede WiFi especificada.
 */
void connectToWiFi(const char *ssid, const char *password);

/**
 * Entra no modo Access Point (AP).
 * 
 * @note Esta função configura o dispositivo para criar uma rede WiFi própria, permitindo que outros dispositivos se conectem a ele.
 */
void enterAPMode();

/**
 * Carrega redes WiFi salvas na memória.
 * 
 * @note Esta função lê as redes WiFi que foram salvas anteriormente e as armazena para uso futuro.
 */
void loadSavedWiFiNetworks();

/**
 * Verifica os resultados do escaneamento de redes WiFi.
 * 
 * @note Esta função analisa os resultados obtidos após o escaneamento de redes WiFi para fornecer informações sobre as redes disponíveis.
 */
void checkScanResults();

#endif // WIFICONEXAO_H
