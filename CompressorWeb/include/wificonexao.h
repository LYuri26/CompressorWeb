#ifndef WIFICONEXAO_H // Verifica se o símbolo WIFICONEXAO_H não foi definido
#define WIFICONEXAO_H // Define o símbolo WIFICONEXAO_H para evitar inclusão múltipla do arquivo

#include <WiFi.h>              // Inclui a biblioteca WiFi.h para gerenciamento de conexões WiFi
#include <ESPAsyncWebServer.h> // Inclui a biblioteca ESPAsyncWebServer para criar um servidor web assíncrono

// -------------------------------------------------------------------------
// Declaração de Variáveis de Configuração WiFi
// -------------------------------------------------------------------------

/**
 * Nome da rede WiFi.
 *
 * @note Esta variável armazena o SSID (nome da rede WiFi) ao qual o dispositivo deve se conectar.
 *
 * A variável `ssid` é uma constante que representa o nome da rede WiFi. O tipo `const char*` é usado para strings em C/C++, onde `const` indica que o conteúdo não pode ser alterado.
 */
extern const char *ssid;

/**
 * Senha da rede WiFi.
 *
 * @note Esta variável armazena a senha necessária para conectar-se à rede WiFi especificada pelo SSID.
 *
 * A variável `password` é uma constante que representa a senha da rede WiFi. Assim como `ssid`, o tipo `const char*` é utilizado para garantir que a senha não seja alterada acidentalmente.
 */
extern const char *password;

// -------------------------------------------------------------------------
// Declaração da Variável de Estado
// -------------------------------------------------------------------------

/**
 * Indica se o dispositivo está no modo Access Point (AP).
 *
 * @note Esta variável é usada para verificar o estado atual do dispositivo, se está no modo AP (ponto de acesso) ou como cliente WiFi.
 *
 * A variável `isAPMode` é um booleano (`bool`) que armazena o estado do dispositivo. Se `true`, o dispositivo está no modo AP; se `false`, está conectado a uma rede WiFi como cliente.
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
 * @note Esta função tenta estabelecer uma conexão com a rede WiFi especificada. O tipo `const char*` é utilizado para passar strings (SSID e senha) como argumentos.
 */
void connectToWiFi(const char *ssid, const char *password);

/**
 * Entra no modo Access Point (AP).
 *
 * @note Esta função configura o dispositivo para criar uma rede WiFi própria, permitindo que outros dispositivos se conectem a ele. Isso é útil para configuração inicial ou para situações em que o dispositivo precisa criar uma rede própria para interação com outros dispositivos.
 */
void setupAP();

/**
 * Carrega redes WiFi salvas na memória.
 *
 * @note Esta função lê as redes WiFi que foram salvas anteriormente e as armazena para uso futuro. A função é útil para reestabelecer conexões automáticas a redes conhecidas após reinicializações ou desconexões.
 */
void loadSavedWiFiNetworks();

/**
 * Verifica os resultados do escaneamento de redes WiFi.
 *
 * @note Esta função analisa os resultados obtidos após o escaneamento de redes WiFi para fornecer informações sobre as redes disponíveis. Pode ser utilizada para mostrar uma lista de redes WiFi disponíveis para o usuário selecionar.
 */
void checkScanResults();

#endif // WIFICONEXAO_H  // Fim da diretiva de inclusão condicional para o símbolo WIFICONEXAO_H
