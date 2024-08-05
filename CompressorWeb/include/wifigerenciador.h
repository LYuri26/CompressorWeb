#ifndef WIFIGERENCIADOR_H
#define WIFIGERENCIADOR_H

#include <ESPAsyncWebServer.h>  // Inclui a biblioteca para o servidor web assíncrono

// -------------------------------------------------------------------------
// Funções para Gerenciamento de WiFi
// -------------------------------------------------------------------------

/**
 * Configura a página de gerenciamento de WiFi no servidor web.
 * 
 * @param server Referência para o servidor web assíncrono.
 */
void setupWiFiManagementPage(AsyncWebServer& server);

/**
 * Conecta ao WiFi com o SSID e a senha fornecidos.
 * 
 * @param ssid Nome da rede WiFi.
 * @param password Senha da rede WiFi.
 */
void connectToWiFi(const char *ssid, const char *password);

/**
 * Entra no modo Access Point (AP).
 * Este modo permite que o dispositivo crie uma rede WiFi própria.
 */
void enterAPMode();

/**
 * Inicia o escaneamento de redes WiFi de forma assíncrona.
 * Este método realiza uma varredura para encontrar redes WiFi disponíveis.
 */
void scanWiFiNetworksAsync();

/**
 * Processa os resultados do escaneamento de redes WiFi.
 * Este método lida com os resultados obtidos após o escaneamento.
 */
void processScanResults();

/**
 * Obtém os resultados do escaneamento de redes WiFi.
 * 
 * @return String contendo os resultados do escaneamento.
 */
String getScanResults();

/**
 * Inicia o processo de escaneamento de redes WiFi.
 * Este método inicia a varredura das redes WiFi disponíveis.
 */
void startScanWiFiNetworks();

// -------------------------------------------------------------------------
// Variáveis Externas
// -------------------------------------------------------------------------

/**
 * Flag indicando se o escaneamento de redes WiFi está em andamento.
 * 
 * @note Esta variável é usada para verificar se o escaneamento está em progresso.
 */
extern bool scanning;

/**
 * Marca de tempo da última execução do escaneamento.
 * 
 * @note Usada para controlar a frequência dos escaneamentos.
 */
extern unsigned long lastScan;

/**
 * Intervalo entre escaneamentos (em milissegundos).
 * 
 * @note Configurado para 5 segundos (5000 milissegundos).
 */
extern const unsigned long scanInterval;

#endif // WIFIGERENCIADOR_H
