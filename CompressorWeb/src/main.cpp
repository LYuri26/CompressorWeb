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
#include "sobrecarga.h"

AsyncWebServer server(80);

// Declarações das funções
void setupServer();
void configureRoutes();
void redirectToAccessDenied(AsyncWebServerRequest *request);

// Variáveis globais para controle de reconexão Wi-Fi
unsigned long lastReconnectAttempt = 0;
int reconnectAttempts = 0;
const int MAX_RECONNECT_ATTEMPTS = 20;
const unsigned long RECONNECT_INTERVAL = 5000;
const unsigned long UPDATE_INTERVAL = 300000; // Intervalo de atualização do tempo e status dos motores
const unsigned long RESTART_TIME = 60000;     // Tempo para reinicialização em caso de falha

unsigned long lastUpdate = 0;           // Última vez que o tempo foi atualizado
unsigned long lastCompressorUpdate = 0; // Última vez que o status dos compressores foi atualizado

void setup()
{
    Serial.begin(115200); // Inicializa a comunicação serial

    // Inicializa o sistema de arquivos SPIFFS
    if (!SPIFFS.begin(true))
    {
        Serial.println("Falha ao iniciar o sistema de arquivos SPIFFS");
        return;
    }
    Serial.println("SPIFFS inicializado com sucesso.");

    // Configura o modo AP (Access Point) e carrega redes Wi-Fi salvas
    setupAP();
    loadSavedWiFiNetworks();

    // Configura o servidor web e as rotas
    setupServer();

    // Configura o cliente de tempo
    setupTimeClient();

    // Configura o botão de manutenção
    setupManutencao();

    // Configura o sistema de ligar/desligar motores
    setupLigaDesliga(server);

    // Configura a página do dashboard
    setupDashboardPage(server);

    // Configura o sistema de sobrecarga
    setupSobrecarga();

    Serial.println("Sistema inicializado com sucesso.");
}

void loop()
{
    unsigned long currentMillis = millis();

    // Atualiza o tempo a cada intervalo definido
    if (currentMillis - lastUpdate >= UPDATE_INTERVAL)
    {
        updateTime();
        lastUpdate = currentMillis;
    }

    // Atualiza o status dos motores a cada intervalo definido
    if (currentMillis - lastCompressorUpdate >= UPDATE_INTERVAL)
    {
        updateMotorStatus();      // Atualiza o status dos motores
        atualizarEstadoMotores(); // Verifica se os motores devem ser desligados (manutenção ou horário)
        lastCompressorUpdate = currentMillis;
    }

    // Atualiza o estado de manutenção
    atualizarEstadoManutencao();

    // Verifica o status dos compressores e atualiza automaticamente os arquivos
    monitorarStatusCompressores();

    // Monitora as entradas de sobrecarga
    monitorarSobrecarga();

    // Verifica a conexão Wi-Fi e tenta reconectar, se necessário
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

    // Pequeno delay para evitar leituras excessivas
    delay(100);
}

void setupServer()
{
    Serial.println("Configurando o servidor...");

    // Configura as páginas do servidor
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

    // Configura as rotas do servidor
    configureRoutes();

    // Inicia o servidor
    server.begin();
    Serial.println("Servidor iniciado");
}

void configureRoutes()
{
    // Rota para o login
    server.on("/login", HTTP_POST, [](AsyncWebServerRequest *request)
              { handleLogin(request); });

    // Rota para o logout
    server.on("/logout", HTTP_GET, [](AsyncWebServerRequest *request)
              { handleLogout(request); });

    // Rota para o dashboard
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

    // Rota para alternar o estado dos motores
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

    // Rota para verificar autenticação
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