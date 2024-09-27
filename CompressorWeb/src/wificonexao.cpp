#include <WiFi.h>   // Biblioteca para gerenciar a conexão Wi-Fi
#include <SPIFFS.h> // Biblioteca para gerenciar o sistema de arquivos SPIFFS
#include <vector>   // Biblioteca para usar vetores

// -------------------------------------------------------------------------
// Configurações Globais
// -------------------------------------------------------------------------
const char *ssid = "";                 // SSID da rede Wi-Fi para conexão (deixe vazio se não houver rede pré-configurada)
const char *password = "";             // Senha da rede Wi-Fi para conexão (deixe vazio se não houver rede pré-configurada)
const char *ap_ssid = "EnergizAR";     // SSID do Access Point (nome da rede gerada pelo dispositivo)
const char *ap_password = "12345678";  // Senha do Access Point (senha para se conectar à rede gerada pelo dispositivo)

IPAddress local_ip(192, 168, 26, 7);   // IP local do Access Point
IPAddress gateway(192, 168, 26, 1);    // Gateway do Access Point
IPAddress subnet(255, 255, 255, 0);    // Máscara de sub-rede do Access Point

const unsigned long scanInterval = 5000; // Intervalo entre escaneamentos
unsigned long lastScan = 0;              // Armazena o tempo do último escaneamento

bool scanning = false; // Indica se o escaneamento de redes está em andamento
bool isAPMode = false; // Indica se o dispositivo está no modo Access Point
bool scanDone = false; // Indica se o escaneamento foi concluído
int scanResults = 0;   // Número de redes encontradas

// -------------------------------------------------------------------------
// Função para Configurar o Modo AP
// -------------------------------------------------------------------------
void setupAP()
{
    WiFi.mode(WIFI_AP_STA);                       // Modo Access Point e Estação
    WiFi.softAPConfig(local_ip, gateway, subnet); // Configuração IP
    if (WiFi.softAP(ap_ssid, ap_password))        // Inicia Access Point
    {
        Serial.print("Modo AP iniciado. Endereço IP: ");
        Serial.println(WiFi.softAPIP());
        Serial.print("Nome da rede AP: ");
        Serial.println(ap_ssid);
        isAPMode = true;
    }
    else
    {
        Serial.println("Falha ao iniciar o Access Point.");
    }
}

// -------------------------------------------------------------------------
// Função para Conectar ao Wi-Fi
// -------------------------------------------------------------------------
void connectToWiFi(const char *ssid, const char *password)
{
    if (!ssid || !password || strlen(ssid) == 0 || strlen(password) == 0)
    {
        Serial.println("SSID ou senha inválidos.");
        return;
    }

    Serial.print("Tentando conectar ao WiFi: ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);

    int attempts = 0;
    const int maxAttempts = 15; // Tentativas aumentadas para melhor conectividade

    while (WiFi.status() != WL_CONNECTED && attempts < maxAttempts)
    {
        delay(1000);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println();
        Serial.print("Conexão feita! Endereço IP: ");
        Serial.println(WiFi.localIP());
    }
    else
    {
        Serial.println();
        Serial.println("Conexão falhou. Continuando no modo AP.");
        setupAP();  // Se a conexão falhar, ativa o modo Access Point.
    }
}

// -------------------------------------------------------------------------
// Função para Iniciar o Escaneamento de Redes Wi-Fi
// -------------------------------------------------------------------------
void startScanWiFiNetworks()
{
    if (!scanning && millis() - lastScan > scanInterval)
    {
        scanning = true;
        WiFi.scanNetworks(true); // Inicia o escaneamento de redes
        lastScan = millis();
    }
}

// -------------------------------------------------------------------------
// Função para Obter os Resultados do Escaneamento
// -------------------------------------------------------------------------
String getScanResults()
{
    int n = WiFi.scanComplete(); // Obtém o estado do escaneamento
    if (n == -2 || n == -1)
    {
        return "[]"; // Escaneamento ainda não concluído ou em andamento
    }
    else if (n == 0)
    {
        return "[]"; // Nenhuma rede encontrada
    }
    else
    {
        String json = "[";
        for (int i = 0; i < n; i++)
        {
            json += "{\"ssid\":\"" + WiFi.SSID(i) + "\"}";
            if (i < n - 1)
                json += ",";
        }
        json += "]";
        scanning = false;
        return json;
    }
}

// -------------------------------------------------------------------------
// Função para Carregar Redes Wi-Fi Salvas e Tentar Conexão
// -------------------------------------------------------------------------
void loadSavedWiFiNetworks()
{
    File file = SPIFFS.open("/wifiredes.txt", FILE_READ);
    if (!file)
    {
        Serial.println("Erro ao abrir o arquivo de redes Wi-Fi.");
        return;
    }

    String content = file.readString();
    file.close();

    String ssid;
    String password;
    std::vector<String> networks;

    int start = 0;
    while (start < content.length())
    {
        int end = content.indexOf('\n', start);
        if (end == -1)
        {
            end = content.length();
        }
        String line = content.substring(start, end);
        networks.push_back(line);
        start = end + 1;
    }

    for (int i = networks.size() - 1; i >= 0; i--)
    {
        String line = networks[i];
        int commaIndex = line.indexOf(',');
        if (commaIndex != -1)
        {
            ssid = line.substring(0, commaIndex);
            password = line.substring(commaIndex + 1);
            connectToWiFi(ssid.c_str(), password.c_str());
            if (WiFi.status() == WL_CONNECTED)
            {
                return; // Sai se conectado
            }
        }
    }
    setupAP(); // Continua no modo AP se não conectar a nenhuma rede
}
