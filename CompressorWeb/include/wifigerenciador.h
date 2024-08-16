#ifndef WIFIGERENCIADOR_H // Verifica se o símbolo WIFIGERENCIADOR_H não foi definido
#define WIFIGERENCIADOR_H // Define o símbolo WIFIGERENCIADOR_H para evitar inclusão múltipla do arquivo

#include <ESPAsyncWebServer.h> // Inclui a biblioteca ESPAsyncWebServer para criar um servidor web assíncrono

// -------------------------------------------------------------------------
// Funções para Gerenciamento de WiFi
// -------------------------------------------------------------------------

/**
 * Configura a página de gerenciamento de WiFi no servidor web.
 *
 * @param server Referência para o servidor web assíncrono.
 *
 * Esta função configura a rota e o conteúdo da página de gerenciamento de WiFi no servidor.
 */
void setupWiFiGerenciadorPage(AsyncWebServer &server);

/**
 * Conecta ao WiFi com o SSID e a senha fornecidos.
 *
 * @param ssid Nome da rede WiFi.
 * @param password Senha da rede WiFi.
 *
 * Esta função é responsável por conectar o dispositivo a uma rede WiFi específica usando o SSID e a senha fornecidos.
 */
void connectToWiFi(const char *ssid, const char *password);

/**
 * Entra no modo Access Point (AP).
 *
 * Este modo permite que o dispositivo crie uma rede WiFi própria.
 *
 * Esta função configura o dispositivo para operar como um ponto de acesso WiFi, permitindo que outros dispositivos se conectem diretamente a ele.
 */
void enterAPMode();

/**
 * Inicia o escaneamento de redes WiFi de forma assíncrona.
 *
 * Este método realiza uma varredura para encontrar redes WiFi disponíveis.
 *
 * A função escaneia redes WiFi disponíveis de forma assíncrona, permitindo que o processo de escaneamento não bloqueie a execução do código.
 */
void scanWiFiNetworksAsync();

/**
 * Processa os resultados do escaneamento de redes WiFi.
 *
 * Este método lida com os resultados obtidos após o escaneamento.
 *
 * A função processa e organiza os resultados encontrados durante o escaneamento das redes WiFi.
 */
void processScanResults();

/**
 * Obtém os resultados do escaneamento de redes WiFi.
 *
 * @return String contendo os resultados do escaneamento.
 *
 * Esta função retorna uma string com os resultados do escaneamento, geralmente formatada em JSON ou outro formato apropriado para a interface web.
 */
String getScanResults();

/**
 * Inicia o processo de escaneamento de redes WiFi.
 *
 * Este método inicia a varredura das redes WiFi disponíveis.
 *
 * Esta função inicia o processo de escaneamento das redes WiFi disponíveis, podendo ser usada para atualizar a lista de redes.
 */
void startScanWiFiNetworks();

// -------------------------------------------------------------------------
// Variáveis Externas
// -------------------------------------------------------------------------

/**
 * Flag indicando se o escaneamento de redes WiFi está em andamento.
 *
 * @note Esta variável é usada para verificar se o escaneamento está em progresso.
 *
 * A variável `scanning` é usada para indicar o estado atual do escaneamento de redes WiFi, ajudando a evitar iniciar múltiplos escaneamentos simultaneamente.
 */
extern bool scanning;

/**
 * Marca de tempo da última execução do escaneamento.
 *
 * @note Usada para controlar a frequência dos escaneamentos.
 *
 * A variável `lastScan` armazena o timestamp da última execução do escaneamento para ajudar a gerenciar a frequência dos escaneamentos e evitar escaneamentos excessivos.
 */
extern unsigned long lastScan;

/**
 * Intervalo entre escaneamentos (em milissegundos).
 *
 * @note Configurado para 5 segundos (5000 milissegundos).
 *
 * A variável `scanInterval` define o intervalo de tempo entre os escaneamentos em milissegundos, ajudando a controlar a frequência com que as redes WiFi são escaneadas.
 */
extern const unsigned long scanInterval;

#endif // WIFIGERENCIADOR_H  // Fim da diretiva de inclusão condicional para o símbolo WIFIGERENCIADOR_H
