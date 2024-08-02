#include "serverSetup.h"
#include <WebServer.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include "index.h"
#include "dashboard.h"
#include "ligadesliga.h"
#include "creditos.h"
#include "umidade.h"
#include "oleo.h"
#include "wificonexao.h"
#include "autenticador.h"
#include "paginaserro.h"

// Declaração do WebServer
extern WebServer server;

void setupServer()
{
    Serial.println("Configurando o servidor...");

    if (!SPIFFS.begin(true))
    {
        Serial.println("Falha ao iniciar o sistema de arquivos SPIFFS");
        return;
    }

    // Configure as páginas e as rotas
    setupIndexPage(server);
    setupCreditosPage(server);
    setupDashboardPage(server);
    setupLigaDesliga(server);
    setupUmidadePage(server);
    setupOleoPage(server);

    // Configura as páginas de erro
    setupAcessoInvalidoPage(server);
    setupNotFoundPage(server);
    setupUsuarioJaLogadoPage(server);
    setupCredenciaisInvalidasPage(server);

    server.on("/login", HTTP_POST, handleLogin);
    server.on("/logout", HTTP_GET, handleLogout);

    server.begin();
    Serial.println("Servidor iniciado");
}
