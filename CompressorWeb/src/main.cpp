#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include "index.h"
#include "autenticador.h"
#include "dashboard.h"
#include "ligadesliga.h"
#include "creditos.h"
#include "umidade.h"
#include "oleo.h"
#include "wificonexao.h" // Inclua o cabeçalho com as declarações
#include "paginaserro.h"
#include "tempo.h"
#include "wifigerenciador.h"

// Cria um objeto servidor web na porta 80 (HTTP)
AsyncWebServer server(80);

// Declaração de funções
void setupSPIFFS();
void setupServer();
void configureRoutes();
bool isAuthenticated(AsyncWebServerRequest *request);
void redirectToAccessDenied(AsyncWebServerRequest *request);

unsigned long lastReconnectAttempt = 0;
const unsigned long reconnectInterval = 1800000; // 30 minutos

void setup()
{
    Serial.begin(115200); // Inicia a comunicação serial com 115200 bps
    Serial.println("Iniciando o setup...");

    WiFi.mode(WIFI_STA); // Configura o ESP32 para o modo Station
    delay(1000);         // Aguarda um pouco

    setupSPIFFS();           // Inicializa o sistema de arquivos SPIFFS
    loadSavedWiFiNetworks(); // Tenta conectar às redes salvas

    setupServer();     // Configura o servidor
    setupTimeClient(); // Inicializa o cliente NTP
}

void loop()
{
    // O servidor assíncrono lida com as requisições automaticamente
    if (!isAPMode)
    {
        delay(1000);

        // Reconecta se a conexão WiFi for perdida e tenta reconectar a cada 30 minutos
        if (WiFi.status() != WL_CONNECTED)
        {
            unsigned long currentMillis = millis();
            if (currentMillis - lastReconnectAttempt >= reconnectInterval)
            {
                lastReconnectAttempt = currentMillis;
                Serial.println("Conexão WiFi perdida. Tentando reconectar..."); // Imprime uma mensagem na serial

                connectToWiFi(ssid, password); // Tenta reconectar ao WiFi
                if (WiFi.status() != WL_CONNECTED)
                {
                    Serial.println("WiFi desconectado, entrando em modo AP");
                    enterAPMode();
                }
            }
        }
    }
    delay(1000);
    updateTime();
}

void setupSPIFFS()
{
    if (!SPIFFS.begin(true))
    {
        Serial.println("Falha ao iniciar o sistema de arquivos SPIFFS");
        return;
    }
    Serial.println("SPIFFS inicializado com sucesso.");
}
void setupServer()
{
    Serial.println("Configurando o servidor..."); // Imprime uma mensagem na serial

    // Configura as páginas e rotas do servidor
    setupIndexPage(server);                // Configura a página de login
    setupCreditosPage(server);             // Configura a página de créditos
    setupDashboardPage(server);            // Configura a página do dashboard
    setupLigaDesliga(server);              // Configura a página de ligar/desligar
    setupUmidadePage(server);              // Configura a página de umidade
    setupOleoPage(server);                 // Configura a página de nível de óleo
    setupAcessoInvalidoPage(server);       // Configura a página de acesso inválido
    setupNotFoundPage(server);             // Configura a página de erro 404
    setupUsuarioJaLogadoPage(server);      // Configura a página de usuário já logado
    setupCredenciaisInvalidasPage(server); // Configura a página de credenciais inválidas
    setupWiFiManagementPage(server);       // Configura a página de gerenciamento Wi-Fi

    // Configura as rotas do servidor
    configureRoutes();

    // Inicia o servidor web
    server.begin();
    Serial.println("Servidor iniciado"); // Imprime uma mensagem na serial
}

void configureRoutes()
{
    // Rota de login
    server.on("/login", HTTP_POST, [](AsyncWebServerRequest *request)
              { handleLogin(request); });

    // Rota de logout
    server.on("/logout", HTTP_GET, [](AsyncWebServerRequest *request)
              { handleLogout(request); });

    // Rota protegida: Dashboard
    server.on("/dashboard", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        if (isAuthenticated(request)) {
            request->send(SPIFFS, "/dashboard", "text/html");
        } else {
            redirectToAccessDenied(request);
        } });

    // Rota protegida: Toggle
    server.on("/toggle", HTTP_ANY, [](AsyncWebServerRequest *request)
              {
        if (isAuthenticated(request)) {
            handleToggleAction(server); // Chama a função para lidar com a ação de ligar/desligar
        } else {
            redirectToAccessDenied(request);
        } });

    // Rota para verificar autenticação
    server.on("/check-auth", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        if (isAuthenticated(request)) {
            request->send(200, "application/json", "{\"authenticated\":true}");
        } else {
            request->send(200, "application/json", "{\"authenticated\":false}");
        } });
}

void redirectToAccessDenied(AsyncWebServerRequest *request)
{
    request->redirect("/acesso-invalido"); // Redireciona para a página de acesso inválido
}