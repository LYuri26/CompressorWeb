#include <WiFi.h>        // Biblioteca para gerenciar a conexão Wi-Fi
#include <SPIFFS.h>      // Biblioteca para gerenciar o sistema de arquivos SPIFFS
#include <vector>        // Biblioteca para usar vetores (não utilizada aqui, mas incluída para referências futuras)
#include "wificonexao.h" // Inclui funções de conexão Wi-Fi personalizadas (não fornecido)

// -------------------------------------------------------------------------
// Configurações Globais
// -------------------------------------------------------------------------
const char *ssid = "";                 // SSID da rede Wi-Fi para conexão
const char *password = "";             // Senha da rede Wi-Fi para conexão
const char *ap_ssid = "CompressorWeb"; // SSID do Access Point
const char *ap_password = "12345678";  // Senha do Access Point

// Configurações do Access Point
IPAddress local_ip(192, 168, 26, 7); // IP local do Access Point
IPAddress gateway(192, 168, 4, 1);  // Gateway do Access Point
IPAddress subnet(255, 255, 255, 0); // Máscara de sub-rede do Access Point

// Variáveis para o gerenciamento de escaneamento de redes Wi-Fi
bool scanning = false;                   // Indica se o escaneamento está em andamento
unsigned long lastScan = 0;              // Tempo do último escaneamento
const unsigned long scanInterval = 5000; // Intervalo entre escaneamentos (em milissegundos)

bool isAPMode = false; // Indica se o dispositivo está no modo Access Point
bool scanDone = false; // Indica se o escaneamento foi concluído
int scanResults = 0;   // Número de redes encontradas

// -------------------------------------------------------------------------
// Função para Conectar ao Wi-Fi
// -------------------------------------------------------------------------
void connectToWiFi(const char *ssid, const char *password)
{
    // Verifica se o SSID e a senha são válidos
    if (!ssid || !password || strlen(ssid) == 0 || strlen(password) == 0)
    {
        Serial.println("SSID ou senha inválidos.");
        enterAPMode(); // Entra no modo Access Point se os dados forem inválidos
        return;
    }

    Serial.print("Tentando conectar ao WiFi: ");
    Serial.println(ssid);
    WiFi.begin(ssid, password); // Inicia a conexão com o Wi-Fi

    int attempts = 0;           // Contador de tentativas
    const int maxAttempts = 5; // Número máximo de tentativas de conexão

    // Tenta conectar ao Wi-Fi até o número máximo de tentativas
    while (WiFi.status() != WL_CONNECTED && attempts < maxAttempts)
    {
        delay(1000);       // Espera 1 segundo entre tentativas
        Serial.print("."); // Imprime um ponto para indicar que está tentando conectar
        attempts++;
    }

    // Verifica se a conexão foi bem-sucedida
    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println();
        Serial.print("Conexão feita! Endereço IP: ");
        Serial.println(WiFi.localIP()); // Imprime o IP local atribuído
        isAPMode = false;               // Desativa o modo Access Point
    }
    else
    {
        Serial.println();
        Serial.println("Conexão falha.");
        enterAPMode(); // Entra no modo Access Point se a conexão falhar
    }
}

// -------------------------------------------------------------------------
// Função para Entrar no Modo Access Point
// -------------------------------------------------------------------------
void enterAPMode()
{
    Serial.println("Entrando no modo Access Point...");
    WiFi.mode(WIFI_AP);                           // Configura o ESP32 para o modo Access Point
    WiFi.softAPConfig(local_ip, gateway, subnet); // Configura o IP, gateway e máscara
    WiFi.softAP(ap_ssid, ap_password);            // Inicia o Access Point com SSID e senha
    Serial.print("Modo AP iniciado. Endereço IP: ");
    Serial.println(WiFi.softAPIP()); // Imprime o IP do Access Point
    isAPMode = true;                 // Marca que o dispositivo está em modo Access Point
}

// -------------------------------------------------------------------------
// Função para Iniciar o Escaneamento de Redes Wi-Fi
// -------------------------------------------------------------------------
void startScanWiFiNetworks()
{
    // Inicia o escaneamento se não estiver em andamento e o intervalo entre escaneamentos tiver passado
    if (!scanning && millis() - lastScan > scanInterval)
    {
        scanning = true;         // Marca que o escaneamento está em andamento
        WiFi.scanNetworks(true); // Inicia o escaneamento de redes Wi-Fi
        lastScan = millis();     // Atualiza o tempo do último escaneamento
    }
}

// -------------------------------------------------------------------------
// Função para Obter os Resultados do Escaneamento
// -------------------------------------------------------------------------
String getScanResults()
{
    int n = WiFi.scanComplete(); // Obtém o número de redes encontradas
    if (n == -2 || n == -1)
    {
        // O escaneamento não foi iniciado ou está em andamento
        return "[]";
    }
    else if (n == 0)
    {
        // Nenhuma rede encontrada
        return "[]";
    }
    else
    {
        // Formata os resultados do escaneamento como um array JSON
        String json = "[";
        for (int i = 0; i < n; i++)
        {
            json += "{\"ssid\":\"" + WiFi.SSID(i) + "\"}";
            if (i < n - 1)
                json += ","; // Adiciona vírgula entre os itens, exceto no último
        }
        json += "]";
        scanning = false; // Marca que o escaneamento foi concluído
        return json;
    }
}

// -------------------------------------------------------------------------
// Função para Verificar se o Escaneamento foi Concluído
// -------------------------------------------------------------------------
void checkScanResults()
{
    if (WiFi.scanComplete() != WIFI_SCAN_RUNNING)
    {
        scanDone = true;                   // Marca que o escaneamento foi concluído
        scanResults = WiFi.scanComplete(); // Armazena o número de redes encontradas
        scanning = false;                  // Marca que o escaneamento foi concluído
    }
}

// -------------------------------------------------------------------------
// Função para Carregar Redes Wi-Fi Salvas e Tentar Conexão
// -------------------------------------------------------------------------
void loadSavedWiFiNetworks()
{
    // Abre o arquivo que contém redes Wi-Fi salvas
    File file = SPIFFS.open("/wifiredes.txt", FILE_READ);
    if (!file)
    {
        Serial.println("Erro ao abrir o arquivo de redes Wi-Fi");
        return;
    }

    // Lê o conteúdo do arquivo
    String content = file.readString();
    file.close(); // Fecha o arquivo após leitura

    String ssid;
    String password;
    std::vector<String> networks; // Vector para armazenar as redes

    int start = 0;
    while (start < content.length())
    {
        int end = content.indexOf('\n', start); // Encontra a posição do próximo caractere de nova linha
        if (end == -1)
        {
            end = content.length(); // Se não houver nova linha, vai até o fim do conteúdo
        }
        String line = content.substring(start, end); // Extrai a linha atual
        networks.push_back(line);                    // Adiciona a linha ao vector
        start = end + 1;                             // Move para o início da próxima linha
    }

    // Tenta conectar começando da última rede salva
    for (int i = networks.size() - 1; i >= 0; i--)
    {
        String line = networks[i];
        int commaIndex = line.indexOf(',');          // Encontra a posição da vírgula
        if (commaIndex != -1)
        {
            ssid = line.substring(0, commaIndex);          // Extrai o SSID
            password = line.substring(commaIndex + 1);     // Extrai a senha
            connectToWiFi(ssid.c_str(), password.c_str()); // Tenta conectar com o SSID e senha extraídos
            if (WiFi.status() == WL_CONNECTED)
            {
                return; // Se conectado, sai da função
            }
        }
    }
}