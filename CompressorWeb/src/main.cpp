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
    Serial.begin(115200);                   // Inicia a comunicação serial com velocidade de 115200 bps
    Serial.println("Iniciando o setup..."); // Imprime uma mensagem na serial

    connectToWiFi(); // Chama a função para conectar ao WiFi
    setupSPIFFS();   // Inicializa o sistema de arquivos SPIFFS
    setupServer();   // Chama a função para configurar o servidor

    setupTimeClient(); // Inicializa o cliente NTP
}

void loop() {
    // O servidor assíncrono lida com as requisições automaticamente
    updateTime();
    delay(1000);

    // Reconecta se a conexão WiFi for perdida
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Conexão WiFi perdida. Tentando reconectar..."); // Imprime uma mensagem na serial
        connectToWiFi();                                                // Tenta reconectar ao WiFi
    }
}

void setupSPIFFS() {
    if (!SPIFFS.begin(true)) {
        Serial.println("Falha ao iniciar o sistema de arquivos SPIFFS"); // Imprime uma mensagem de erro na serial
        return;                                                          // Sai da função se falhar ao iniciar o SPIFFS
    }
}

void setupServer() {
    Serial.println("Configurando o servidor..."); // Imprime uma mensagem na serial

    // Configura as páginas e rotas do servidor
    setupIndexPage(server);            // Configura a página de login
    setupCreditosPage(server);         // Configura a página de créditos
    setupDashboardPage(server);        // Configura a página do dashboard
    setupLigaDesliga(server);          // Configura a página de ligar/desligar
    setupUmidadePage(server);          // Configura a página de umidade
    setupOleoPage(server);             // Configura a página de nível de óleo
    setupAcessoInvalidoPage(server);   // Configura a página de acesso inválido
    setupNotFoundPage(server);         // Configura a página de erro 404
    setupUsuarioJaLogadoPage(server);  // Configura a página de usuário já logado
    setupCredenciaisInvalidasPage(server); // Configura a página de credenciais inválidas

    // Configura as rotas do servidor
    configureRoutes();

    // Inicia o servidor web
    server.begin();
    Serial.println("Servidor iniciado"); // Imprime uma mensagem na serial
}

void configureRoutes() {
    // Rota de login
    server.on("/login", HTTP_POST, [](AsyncWebServerRequest *request) { handleLogin(request); });

    // Rota de logout
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
            handleToggleAction(server); // Chama a função para lidar com a ação de ligar/desligar
        } else {
            redirectToAccessDenied(request);
        }
    });

    // Rota para verificar autenticação
    server.on("/check-auth", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (isAuthenticated(request)) {
            request->send(200, "application/json", "{\"authenticated\":true}");
        } else {
            request->send(200, "application/json", "{\"authenticated\":false}");
        }
    });
}

void redirectToAccessDenied(AsyncWebServerRequest *request) {
    request->redirect("/acesso-invalido"); // Redireciona para a página de acesso inválido
}
