#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>
#include "index.h"
#include "dashboard.h"
#include "ligadesliga.h"
#include "creditos.h"
#include "umidade.h"
#include "oleo.h"
#include "logado.h"

// Definições das credenciais das redes WiFi
const char *ssid1 = "CFPFR_WIFI";
const char *password1 = "#CFP-Ur@107!";
const char *ssid2 = "LenonClaro_2.4G";
const char *password2 = "13539406670";

// Definições do IP fixo
IPAddress local_IP(10, 107, 0, 47);
IPAddress gateway(10, 107, 0, 1);
IPAddress subnet(255, 255, 255, 0);

// Cria um objeto servidor web na porta 80
WebServer server(80); 

// Variável global para rastrear o estado de login
bool isLoggedIn = false;

// Declarações das funções
void connectToWiFi();
bool tryConnectToWiFi(const char *ssid, const char *password, bool useFixedIP = false);
void setupServer();
void handleLogin();         
void checkAuthentication(); 

// Declaração das funções externas
void handleInvalidCredentials();  
void handleUserAlreadyLoggedIn(); 

void setup()
{
    Serial.begin(115200);
    Serial.println("Iniciando o setup...");

    connectToWiFi();
    setupServer();
}

void loop()
{
    server.handleClient();
    updateCompressorStatus(); 
}

void connectToWiFi()
{
    Serial.println("Tentando conectar ao WiFi...");

    bool connected = tryConnectToWiFi(ssid1, password1, true);

    if (!connected)
    {
        Serial.println("Tentando conectar à rede WiFi sem IP fixo...");
        connected = tryConnectToWiFi(ssid1, password1);
    }

    if (!connected)
    {
        Serial.println("Tentando conectar à rede WiFi de backup...");
        connected = tryConnectToWiFi(ssid2, password2);
    }

    if (connected)
    {
        Serial.println("Conectado com sucesso!");
    }
    else
    {
        Serial.println("Falha ao conectar-se ao WiFi.");
    }
}

bool tryConnectToWiFi(const char *ssid, const char *password, bool useFixedIP)
{
    Serial.print("Conectando à rede WiFi: ");
    Serial.println(ssid);

    if (useFixedIP)
    {
        if (!WiFi.config(local_IP, gateway, subnet))
        {
            Serial.println("Falha ao configurar IP fixo.");
            return false;
        }
    }

    unsigned long startAttemptTime = millis();
    WiFi.begin(ssid, password);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20)
    {
        delay(1000);
        Serial.print(".");
        attempts++;
    }

    unsigned long endAttemptTime = millis();
    unsigned long connectionTime = (endAttemptTime - startAttemptTime) / 1000;

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println();
        Serial.print("Conectado com sucesso! Endereço IP: ");
        Serial.println(WiFi.localIP());
        Serial.print("Tentativas de conexão: ");
        Serial.println(attempts);
        Serial.print("Tempo total de conexão: ");
        Serial.print(connectionTime);
        Serial.println(" segundos");
        return true;
    }
    else
    {
        Serial.println();
        Serial.println("Falha ao conectar-se ao WiFi");
        Serial.print("Status: ");
        Serial.println(WiFi.status());
        Serial.print("Tentativas de conexão: ");
        Serial.println(attempts);
        return false;
    }
}

void setupServer()
{
    Serial.println("Configurando o servidor...");

    if (!SPIFFS.begin(true))
    {
        Serial.println("Falha ao iniciar o sistema de arquivos SPIFFS");
        return;
    }

    // Configura as páginas e funcionalidades do servidor
    setupIndexPage(server);
    setupCreditosPage(server);
    setupDashboardPage(server);
    setupLigaDesliga(server);
    setupUmidadePage(server);
    setupOleoPage(server);
    handleToggleAction(server);

    server.on("/login", HTTP_POST, []()
              { handleLogin(); });

    // Adiciona a rota para logout
    server.on("/logout", HTTP_GET, []()
              {
        isLoggedIn = false; 
        server.sendHeader("Location", "/");
        server.send(302, "text/plain", ""); });

    server.begin();
    Serial.println("Servidor iniciado");
}

void handleLogin()
{
    String username = server.arg("username");
    String password = server.arg("password");

    if (username == "admin" && password == "admin123")
    {
        if (!isLoggedIn)
        {
            isLoggedIn = true;
            server.sendHeader("Location", "/dashboard");
            server.send(302, "text/plain", "");
        }
        else
        {
            handleUserAlreadyLoggedIn(); 
        }
    }
    else
    {
        handleInvalidCredentials(); 
    }
}

void checkAuthentication()
{
    if (!isLoggedIn)
    {
        server.sendHeader("Location", "/");
        server.send(302, "text/plain", ""); 
    }
}
