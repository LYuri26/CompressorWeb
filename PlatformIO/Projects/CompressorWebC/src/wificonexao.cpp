#include <WiFi.h>
// Inclui a biblioteca WiFi.h que fornece funções para se conectar a redes WiFi

// Declaração do protótipo da função
// A função tryConnectToWiFi é declarada aqui para que possa ser utilizada em outras partes do código
bool tryConnectToWiFi(const char *ssid, const char *password, bool useStaticIP);

// Declaração das variáveis para armazenar os SSIDs e senhas das redes WiFi
const char *ssid1 = "CFPFR_WIFI";
const char *password1 = "#CFP-Ur@107!";
const char *ssid2 = "LenonClaro_2.4G";
const char *password2 = "13539406670";

// Configurações de IP fixo (comentadas para usar IP dinâmico por padrão)
// Estas linhas são comentadas para usar IP dinâmico por padrão
/*
IPAddress local_IP(10, 107, 0, 47); // IP fixo a ser utilizado
IPAddress gateway(192, 168, 0, 1); // Gateway padrão
IPAddress subnet(255, 255, 255, 0); // Máscara de sub-rede
IPAddress primaryDNS(8, 8, 8, 8); // DNS primário (Google DNS)
IPAddress secondaryDNS(8, 8, 4, 4); // DNS secundário (Google DNS)
*/

void connectToWiFi()
{
    Serial.println("Tentando conectar ao WiFi...");
    // Imprime uma mensagem indicando que o processo de conexão ao WiFi está começando

    // Tenta conectar à primeira rede WiFi com IP dinâmico
    bool connected = tryConnectToWiFi(ssid1, password1, false); // Tenta com IP dinâmico
    if (!connected)
    {
        Serial.println("Tentando conectar à rede WiFi de backup...");
        // Imprime uma mensagem indicando que a conexão com a primeira rede falhou e que está tentando a rede de backup
        connected = tryConnectToWiFi(ssid2, password2, false); // Tenta com IP dinâmico
    }

    // Se a conexão falhar, tenta novamente até conseguir, sempre com IP dinâmico
    while (!connected)
    {
        Serial.println("Falha ao conectar-se ao WiFi. Tentando novamente...");
        // Imprime uma mensagem indicando que a conexão falhou e que tentará novamente
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
    // Imprime uma mensagem informando qual rede WiFi está sendo tentada

    // Configura o WiFi para usar IP fixo se necessário
    if (useStaticIP)
    {
        // Configura o IP fixo, gateway, máscara de sub-rede e DNS (descomente as linhas abaixo para usar IP fixo)
        /*
        bool configResult = WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);
        if (!configResult)
        {
            Serial.println("Falha ao configurar IP fixo. Tentando com IP dinâmico.");
            // Imprime uma mensagem indicando que a configuração do IP fixo falhou
        }
        */
    }
    else
    {
        // Reseta a configuração de IP estático para usar IP dinâmico
        WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
        // INADDR_NONE indica que o WiFi deve usar configurações de IP dinâmico
    }

    WiFi.begin(ssid, password);
    // Inicia a tentativa de conexão ao WiFi com o SSID e senha fornecidos

    int attempts = 0; // Contador de tentativas de conexão
    while (WiFi.status() != WL_CONNECTED && attempts < 20)
    {
        delay(1000); // Aguarda 1 segundo entre as tentativas
        Serial.print("."); // Imprime um ponto a cada segundo durante a tentativa de conexão
        attempts++; // Incrementa o contador de tentativas
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        // Se a conexão for bem-sucedida
        Serial.println(); // Imprime uma nova linha
        Serial.print("Conectado com sucesso! Endereço IP: ");
        Serial.println(WiFi.localIP()); // Imprime o endereço IP atribuído pelo roteador
        Serial.print("Tentativas de conexão: ");
        Serial.println(attempts); // Imprime o número de tentativas necessárias para conectar
        return true; // Retorna verdadeiro, indicando sucesso na conexão
    }
    else
    {
        // Se a conexão falhar
        Serial.println(); // Imprime uma nova linha
        Serial.println("Falha ao conectar-se ao WiFi"); // Imprime uma mensagem indicando falha na conexão
        Serial.print("Status: ");
        Serial.println(WiFi.status()); // Imprime o status atual do WiFi
        Serial.print("Tentativas de conexão: ");
        Serial.println(attempts); // Imprime o número de tentativas realizadas
        return false; // Retorna falso, indicando falha na conexão
    }
}
