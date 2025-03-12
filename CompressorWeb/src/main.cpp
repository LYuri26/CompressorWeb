#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include "index.h"
#include "autenticador.h"
#include "dashboard.h"
#include "ligadesliga.h"
#include "creditos.h"
#include "umidade.h"
#include "pressao.h"
#include "wificonexao.h"
#include "paginaserro.h"
#include "tempo.h"
#include "wifigerenciador.h"
#include "manutencao.h"

AsyncWebServer server(80);

void setupSPIFFS();
void setupServer();
void configureRoutes();
bool isAuthenticated(AsyncWebServerRequest *request);
void redirectToAccessDenied(AsyncWebServerRequest *request);
void updateTime();
void setupTimeClient();
void setupManutencao();
void connectToWiFi(const char *ssid, const char *password);
void setupAP();
void loadSavedWiFiNetworks();
void atualizarEstadoManutencao();

unsigned long lastReconnectAttempt = 0;
int reconnectAttempts = 0;
const int MAX_RECONNECT_ATTEMPTS = 20;
const unsigned long RECONNECT_INTERVAL = 5000;
const unsigned long UPDATE_INTERVAL = 300000;
const unsigned long RESTART_TIME = 60000;

unsigned long lastUpdate = 0;
unsigned long lastCompressorUpdate = 0;

void setup()
{
    Serial.begin(115200);
    SPIFFS.begin(true);
    setupAP();
    loadSavedWiFiNetworks();
    setupServer();
    setupTimeClient();
    setupManutencao();
}

void loop()
{
    unsigned long currentMillis = millis();

    if (currentMillis - lastUpdate >= UPDATE_INTERVAL)
    {
        updateTime();
        lastUpdate = currentMillis;
    }

    if (currentMillis - lastCompressorUpdate >= UPDATE_INTERVAL)
    {
        updateMotorStatus();
        lastCompressorUpdate = currentMillis;
    }

    atualizarEstadoManutencao();

    if (!isAPMode && WiFi.status() != WL_CONNECTED)
    {
        if (currentMillis - lastReconnectAttempt >= RECONNECT_INTERVAL)
        {
            lastReconnectAttempt = currentMillis;
            Serial.println("Conexão WiFi perdida. Tentando reconectar...");
            connectToWiFi(ssid, password);
            reconnectAttempts++;
            if (reconnectAttempts >= MAX_RECONNECT_ATTEMPTS)
            {
                Serial.println("Número máximo de tentativas de reconexão alcançado. Entrando em modo AP.");
                setupAP();
                reconnectAttempts = 0;
            }
        }
    }
    else
    {
        reconnectAttempts = 0;
    }
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
    Serial.println("Configurando o servidor...");

    setupIndexPage(server);
    setupCreditosPage(server);
    setupDashboardPage(server);
    setupLigaDesliga(server);
    setupUmidadePage(server);
    setupPressaoPage(server);
    setupAcessoInvalidoPage(server);
    setupNotFoundPage(server);
    setupUsuarioJaLogadoPage(server);
    setupCredenciaisInvalidasPage(server);
    setupWiFiGerenciadorPage(server);

    configureRoutes();
    server.begin();
    Serial.println("Servidor iniciado");
}

void configureRoutes()
{
    server.on("/login", HTTP_POST, [](AsyncWebServerRequest *request)
              { handleLogin(request); });

    server.on("/logout", HTTP_GET, [](AsyncWebServerRequest *request)
              { handleLogout(request); });

    server.on("/dashboard", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        if (isAuthenticated(request))
        {
            request->send(SPIFFS, "/dashboard", "text/html");
        }
        else
        {
            redirectToAccessDenied(request);
        } });

    server.on("/toggle", HTTP_ANY, [](AsyncWebServerRequest *request)
              {
        if (isAuthenticated(request))
        {
            handleToggleAction(server);
        }
        else
        {
            redirectToAccessDenied(request);
        } });

    server.on("/check-auth", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        if (isAuthenticated(request))
        {
            request->send(200, "application/json", "{\"authenticated\":true}");
        }
        else
        {
            request->send(200, "application/json", "{\"authenticated\":false}");
        } });

    // Rota para obter o status dos motores
    server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        String statusJSON = "{";
        for (int i = 0; i < 3; i++)
        {
            String arquivoEstado = "/status" + String(i + 1) + ".txt";
            File file = SPIFFS.open(arquivoEstado, "r");

            String status = "OFF";
            if (file) {
                status = file.readStringUntil('\n');
                file.close();
            }

            statusJSON += "\"motor" + String(i + 1) + "\": \"" + status + "\"";
            if (i < 2) statusJSON += ", ";
        }
        statusJSON += "}";

        request->send(200, "application/json", statusJSON); });
}

void redirectToAccessDenied(AsyncWebServerRequest *request)
{
    request->redirect("/acesso-invalido");
}