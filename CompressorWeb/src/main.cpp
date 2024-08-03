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
#include "wificonexao.h"
#include "paginaserro.h"
#include "tempo.h"

// Cria um objeto servidor web na porta 80 (HTTP)
AsyncWebServer server(80);

// Declaração de funções
void setupSPIFFS();
void setupServer();
void configureRoutes();
bool isAuthenticated(AsyncWebServerRequest *request);
void redirectToAccessDenied(AsyncWebServerRequest *request);

void setup() {
    Serial.begin(115200);
    Serial.println("Iniciando o setup...");

    connectToWiFi();
    setupSPIFFS();
    setupServer();
    setupTimeClient();
}

void loop() {
    updateTime();
    delay(1000);

    // Reconecta se a conexão WiFi for perdida
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Conexão WiFi perdida. Tentando reconectar...");
        connectToWiFi();
    }
}

void setupSPIFFS() {
    if (!SPIFFS.begin(true)) {
        Serial.println("Falha ao iniciar o sistema de arquivos SPIFFS");
    }
}

void setupServer() {
    Serial.println("Configurando o servidor...");

    // Configura as páginas e rotas do servidor
    setupIndexPage(server);
    setupCreditosPage(server);
    setupDashboardPage(server);
    setupLigaDesliga(server);
    setupUmidadePage(server);
    setupOleoPage(server);
    setupAcessoInvalidoPage(server);
    setupNotFoundPage(server);
    setupUsuarioJaLogadoPage(server);
    setupCredenciaisInvalidasPage(server);

    configureRoutes();
    server.begin();
    Serial.println("Servidor iniciado");
}

void configureRoutes() {
    server.on("/login", HTTP_POST, [](AsyncWebServerRequest *request) { handleLogin(request); });
    server.on("/logout", HTTP_GET, [](AsyncWebServerRequest *request) { handleLogout(request); });

    // Rota protegida: Dashboard
    server.on("/dashboard", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (isAuthenticated(request)) {
            request->send(SPIFFS, "/dashboard", "text/html");
        } else {
            redirectToAccessDenied(request);
        }
    });

    // Rota protegida: Toggle
    server.on("/toggle", HTTP_ANY, [](AsyncWebServerRequest *request) {
        if (isAuthenticated(request)) {
            handleToggleAction(server);
        } else {
            redirectToAccessDenied(request);
        }
    });

    // Rota para verificar autenticação
    server.on("/check-auth", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "application/json", "{\"authenticated\":" + String(isAuthenticated(request) ? "true" : "false") + "}");
    });
}

void redirectToAccessDenied(AsyncWebServerRequest *request) {
    request->redirect("/acesso-invalido");
}
