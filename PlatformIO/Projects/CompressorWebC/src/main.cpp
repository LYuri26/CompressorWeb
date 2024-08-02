#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>
#include "paginaserro.h"
#include "index.h"
#include "dashboard.h"
#include "ligadesliga.h"
#include "creditos.h"
#include "umidade.h"
#include "oleo.h"
#include "wificonexao.h"
#include "autenticador.h"

// Create a WebServer object on port 80
WebServer server(80);

// Function declarations
void setup();
void loop();
void setupServer();
void setupPages();
void setupHandlers();
void setupErrorPages();

// Setup function
void setup()
{
    Serial.begin(115200);
    Serial.println("Iniciando o setup...");
    connectToWiFi(); // Initialize WiFi connection
    setupServer();   // Setup the web server and pages
}

// Main loop function
void loop()
{
    server.handleClient();    // Handle incoming client requests
    updateCompressorStatus(); // Update the compressor status

    // Reconnect WiFi if connection is lost
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("Conexão WiFi perdida. Tentando reconectar...");
        connectToWiFi();
    }
}

// Setup the web server and related pages
void setupServer()
{
    Serial.println("Configurando o servidor...");

    if (!SPIFFS.begin(true))
    {
        Serial.println("Falha ao iniciar o sistema de arquivos SPIFFS");
        return;
    }

    setupPages();      // Configure the pages
    setupHandlers();   // Setup request handlers
    setupErrorPages(); // Setup error pages

    server.begin();
    Serial.println("Servidor iniciado");
}

// Setup all the page handlers
void setupPages()
{
    setupIndexPage(server);
    setupCreditosPage(server);
    setupDashboardPage(server);
    setupLigaDesliga(server);
    setupUmidadePage(server);
    setupOleoPage(server);
}

// Setup handlers for specific actions and login/logout
void setupHandlers()
{
    server.on("/login", HTTP_POST, handleLogin);
    server.on("/logout", HTTP_GET, handleLogout);
    handleToggleAction(server);
}

// Setup the error pages
void setupErrorPages()
{
    setupErrorPages(server);
}
