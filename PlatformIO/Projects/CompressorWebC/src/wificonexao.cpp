#include <WiFi.h>
// Inclui a biblioteca WiFi.h que fornece funções para se conectar a redes WiFi

// Declaração do protótipo da função
// Esta linha declara a função tryConnectToWiFi antes de ser definida, permitindo que seja chamada antes da definição
bool tryConnectToWiFi(const char *ssid, const char *password, bool useStaticIP);

const char *ssid1 = "CFPFR_WIFI";
const char *password1 = "#CFP-Ur@107!";
const char *ssid2 = "LenonClaro_2.4G";
const char *password2 = "13539406670";
// Declaração das variáveis que armazenam os SSIDs e senhas das redes WiFi

// Configurações de IP fixo
IPAddress local_IP(10,107,0,47);
IPAddress gateway(10,107, 0, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8); // Google DNS
IPAddress secondaryDNS(8, 8, 4, 4); // Google DNS
// Declaração das variáveis de configuração de IP fixo, gateway, máscara de sub-rede e DNS

void connectToWiFi()
{
    Serial.println("Tentando conectar ao WiFi...");
    // Imprime uma mensagem informando que está tentando se conectar ao WiFi

    // Tenta conectar à primeira rede WiFi, se falhar, tenta a segunda
    bool connected = tryConnectToWiFi(ssid1, password1, true); // Tenta com IP fixo
    if (!connected)
    {
        Serial.println("Tentando conectar à rede WiFi de backup...");
        // Imprime uma mensagem informando que está tentando conectar à rede WiFi de backup
        connected = tryConnectToWiFi(ssid2, password2, true); // Tenta com IP fixo
    }

    // Se a conexão falhar, tenta novamente até conseguir, agora com IP dinâmico
    while (!connected)
    {
        Serial.println("Falha ao conectar-se ao WiFi com IP fixo. Tentando novamente com IP dinâmico...");
        // Imprime uma mensagem informando que a conexão com IP fixo falhou e que tentará novamente com IP dinâmico
        delay(5000); // Aguarda 5 segundos antes de tentar novamente
        connected = tryConnectToWiFi(ssid1, password1, false); // Tenta com IP dinâmico
        if (!connected)
        {
            connected = tryConnectToWiFi(ssid2, password2, false); // Tenta com IP dinâmico
        }
    }
}

bool tryConnectToWiFi(const char *ssid, const char *password, bool useStaticIP)
{
    Serial.print("Conectando à rede WiFi: ");
    Serial.println(ssid);
    // Imprime uma mensagem informando a qual rede WiFi está tentando se conectar

    // Configura o WiFi para usar IP fixo se necessário
    if (useStaticIP)
    {
        bool configResult = WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);
        // Configura o IP fixo, gateway, máscara de sub-rede e DNS
        if (!configResult)
        {
            Serial.println("Falha ao configurar IP fixo. Tentando com IP dinâmico.");
            // Imprime uma mensagem informando que a configuração de IP fixo falhou e que tentará com IP dinâmico
        }
    }
    else
    {
        // Reseta a configuração de IP estático para usar IP dinâmico
        WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
        // Configura o WiFi para usar IP dinâmico
    }

    WiFi.begin(ssid, password);
    // Inicia a tentativa de conexão ao WiFi com o SSID e senha fornecidos

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20)
    {
        delay(1000);
        Serial.print(".");
        // Imprime um ponto a cada segundo durante a tentativa de conexão
        attempts++;
        // Incrementa o contador de tentativas
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println();
        Serial.print("Conectado com sucesso! Endereço IP: ");
        Serial.println(WiFi.localIP());
        // Imprime uma mensagem informando que a conexão foi bem-sucedida e mostra o endereço IP
        Serial.print("Tentativas de conexão: ");
        Serial.println(attempts);
        // Imprime o número de tentativas necessárias para conectar
        return true;
        // Retorna verdadeiro, indicando que a conexão foi bem-sucedida
    }
    else
    {
        Serial.println();
        Serial.println("Falha ao conectar-se ao WiFi");
        // Imprime uma mensagem informando que a conexão falhou
        Serial.print("Status: ");
        Serial.println(WiFi.status());
        // Imprime o status atual do WiFi
        Serial.print("Tentativas de conexão: ");
        Serial.println(attempts);
        // Imprime o número de tentativas de conexão realizadas
        return false;
        // Retorna falso, indicando que a conexão falhou
    }
}
