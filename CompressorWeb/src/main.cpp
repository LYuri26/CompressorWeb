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

void setup() {
    Serial.begin(115200);                   // Inicia a comunicação serial com velocidade de 115200 bps
    Serial.println("Iniciando o setup..."); // Imprime uma mensagem na serial

    connectToWiFi(); // Chama a função para conectar ao WiFi
    setupServer();   // Chama a função para configurar o servidor

    setupTimeClient(); // Inicializa o cliente NTP
}

void loop() {
    // O servidor assíncrono lida com as requisições automaticamente
    updateTime(); // Atualiza a hora e imprime a hora atual

    // Reconecta se a conexão WiFi for perdida
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Conexão WiFi perdida. Tentando reconectar..."); // Imprime uma mensagem na serial
        connectToWiFi();                                                // Tenta reconectar ao WiFi
    }
}

void setupServer() {
    Serial.println("Configurando o servidor..."); // Imprime uma mensagem na serial

    if (!SPIFFS.begin(true)) {
        Serial.println("Falha ao iniciar o sistema de arquivos SPIFFS"); // Imprime uma mensagem de erro na serial
        return;                                                          // Sai da função se falhar ao iniciar o SPIFFS
    }

    // Configura as páginas e rotas do servidor
    setupIndexPage(server); // Configura a página de login
    setupCreditosPage(server); // Configura a página de créditos
    setupDashboardPage(server); // Configura a página do dashboard
    setupLigaDesliga(server); // Configura a página de ligar/desligar
    setupUmidadePage(server); // Configura a página de umidade
    setupOleoPage(server); // Configura a página de nível de óleo
    setupAcessoInvalidoPage(server); // Configura a página de acesso inválido
    setupNotFoundPage(server); // Configura a página de erro 404
    setupUsuarioJaLogadoPage(server); // Configura a página de usuário já logado
    setupCredenciaisInvalidasPage(server); // Configura a página de credenciais inválidas

    // Configura as rotas do servidor
    server.on("/login", HTTP_POST, [](AsyncWebServerRequest *request) { handleLogin(request); });
    server.on("/logout", HTTP_POST, [](AsyncWebServerRequest *request) { handleLogout(request); });

    handleToggleAction(server); // Configura a manipulação de ações de ligar/desligar

    server.begin();                      // Inicia o servidor web
    Serial.println("Servidor iniciado"); // Imprime uma mensagem na serial
}
