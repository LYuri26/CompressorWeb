#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>
#include "index.h"
#include "dashboard.h"
#include "ligadesliga.h"
#include "creditos.h"
#include "umidade.h"
#include "oleo.h"
#include "wifi.h"
#include "autenticador.h"  // Certifique-se de que o cabeçalho esteja incluído

// Cria um objeto servidor web na porta 80
WebServer server(80);

void setup();
void loop();
void setupServer();

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

    // Reconecta se a conexão WiFi for perdida
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("Conexão WiFi perdida. Tentando reconectar...");
        connectToWiFi();
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

    setupIndexPage(server);
    setupCreditosPage(server);
    setupDashboardPage(server);
    setupLigaDesliga(server);
    setupUmidadePage(server);
    setupOleoPage(server);
    handleToggleAction(server);

    server.on("/login", HTTP_POST, handleLogin);
    server.on("/logout", HTTP_GET, handleLogout); // Adiciona a rota de logout
    
    server.begin();
    Serial.println("Servidor iniciado");
}
