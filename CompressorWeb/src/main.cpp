// -------------------------------------------------------------------------
// Inclusão de Bibliotecas
// -------------------------------------------------------------------------
#include <WiFi.h>              // Inclui a biblioteca para funcionalidades WiFi
#include <ESPAsyncWebServer.h> // Inclui a biblioteca para criar um servidor web assíncrono
#include <SPIFFS.h>            // Inclui a biblioteca para o sistema de arquivos SPIFFS

// -------------------------------------------------------------------------
// Cabeçalhos Personalizados
// -------------------------------------------------------------------------
#include "index.h"           // Cabeçalho para a configuração da página inicial
#include "autenticador.h"    // Cabeçalho para a autenticação
#include "dashboard.h"       // Cabeçalho para a página do dashboard
#include "ligadesliga.h"     // Cabeçalho para as funções de ligar e desligar
#include "creditos.h"        // Cabeçalho para a página de créditos
#include "umidade.h"         // Cabeçalho para a página de umidade
#include "oleo.h"            // Cabeçalho para a página de nível de óleo
#include "wificonexao.h"     // Cabeçalho para a configuração WiFi
#include "paginaserro.h"     // Cabeçalho para as páginas de erro
#include "tempo.h"           // Cabeçalho para a configuração do tempo
#include "wifigerenciador.h" // Cabeçalho para o gerenciamento do WiFi
#include "manutencao.h"

// -------------------------------------------------------------------------
// Configuração do Servidor Web
// -------------------------------------------------------------------------
AsyncWebServer server(80); // Criação do objeto do servidor web na porta 80 (HTTP)

// -------------------------------------------------------------------------
// Declarações de Funções
// -------------------------------------------------------------------------
void setupSPIFFS();                                          // Função para inicializar o sistema de arquivos SPIFFS
void setupServer();                                          // Função para configurar o servidor e suas rotas
void configureRoutes();                                      // Função para configurar as rotas do servidor
bool isAuthenticated(AsyncWebServerRequest *request);        // Função para verificar se a requisição está autenticada
void redirectToAccessDenied(AsyncWebServerRequest *request); // Função para redirecionar para a página de acesso negado

// -------------------------------------------------------------------------
// Variáveis de Controle de Reconexão WiFi
// -------------------------------------------------------------------------
unsigned long lastReconnectAttempt = 0; // Armazena o tempo da última tentativa de reconexão
int reconnectAttempts = 0;              // Contador de tentativas de reconexão

const int MAX_RECONNECT_ATTEMPTS = 10; // Número máximo de tentativas de reconexão

// Definição dos intervalos
const unsigned long RECONNECT_INTERVAL = 1000; // Intervalo para reconexão WiFi (1 segundo)
const unsigned long UPDATE_INTERVAL = 300000;  // Intervalo para atualização do tempo e status do compressor (5 minutos)
const unsigned long RESTART_TIME = 60000;      // Tempo para aguardar após verificar horário de reinicialização (1 minuto)

// Variáveis para controle de tempo
unsigned long lastUpdate = 0;

// -------------------------------------------------------------------------
// Função de Configuração Inicial
// -------------------------------------------------------------------------
void setup()
{
    // Inicia a comunicação serial para depuração
    Serial.begin(115200);
    Serial.println("Iniciando o setup..."); // Mensagem indicando o início do setup

    // Configura o ESP32 para o modo Station (cliente) e aguarda um pouco
    WiFi.mode(WIFI_STA); // Define o modo WiFi como Station
    delay(1000);         // Aguarda 1 segundo

    // Inicializa o sistema de arquivos SPIFFS e tenta conectar às redes salvas
    setupSPIFFS();           // Chama a função para inicializar o SPIFFS
    loadSavedWiFiNetworks(); // Função para carregar redes WiFi salvas (não definida no código fornecido)

    // Configura o servidor web e inicializa o cliente NTP
    setupServer();     // Chama a função para configurar o servidor e suas rotas
    setupTimeClient(); // Função para configurar o cliente NTP (não definida no código fornecido)
    setupManutencao(); // Adiciona a configuração do botão de manutenção
}

// -------------------------------------------------------------------------
// Função de Loop Principal
// -------------------------------------------------------------------------
void loop()
{
    unsigned long currentMillis = millis();

    // Atualiza o tempo e o status do compressor a cada intervalo definido
    if (currentMillis - lastUpdate >= UPDATE_INTERVAL)
    {
        updateTime();             // Atualiza o tempo
        lastUpdate = currentMillis;
    }

    // Se o compressor está ligado, execute a atualização do status repetidamente
    if (compressorLigado)
    {
        updateCompressorStatus(); // Atualiza o status do compressor
    }

    // Verifica o estado do botão de manutenção
    atualizarEstadoManutencao();

    // Se o sistema está em modo Station e não em modo AP, verifique a conexão WiFi
    if (!isAPMode)
    {
        if (WiFi.status() != WL_CONNECTED)
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
                    enterAPMode();
                    reconnectAttempts = 0;
                }
            }
        }
        else
        {
            reconnectAttempts = 0; // Resetar tentativas de reconexão se conectado
        }
    }
    else
    {
        reconnectAttempts = 0; // Resetar tentativas de reconexão se em modo AP
    }
}

// -------------------------------------------------------------------------
// Funções Auxiliares
// -------------------------------------------------------------------------
/**
 * Inicializa o sistema de arquivos SPIFFS.
 */
void setupSPIFFS()
{
    if (!SPIFFS.begin(true)) // Tenta iniciar o SPIFFS
    {
        Serial.println("Falha ao iniciar o sistema de arquivos SPIFFS"); // Mensagem de erro se o SPIFFS não iniciar
        return;                                                          // Retorna da função se a inicialização falhar
    }
    Serial.println("SPIFFS inicializado com sucesso."); // Mensagem de sucesso se o SPIFFS iniciar corretamente
}

/**
 * Configura o servidor e suas rotas.
 */
void setupServer()
{
    Serial.println("Configurando o servidor..."); // Mensagem indicando o início da configuração do servidor

    // Configura as páginas e rotas do servidor
    setupIndexPage(server);                // Configura a página inicial
    setupCreditosPage(server);             // Configura a página de créditos
    setupDashboardPage(server);            // Configura a página do dashboard
    setupLigaDesliga(server);              // Configura as funções de ligar e desligar
    setupUmidadePage(server);              // Configura a página de umidade
    setupOleoPage(server);                 // Configura a página de nível de óleo
    setupAcessoInvalidoPage(server);       // Configura a página de acesso inválido
    setupNotFoundPage(server);             // Configura a página de não encontrado
    setupUsuarioJaLogadoPage(server);      // Configura a página de usuário já logado
    setupCredenciaisInvalidasPage(server); // Configura a página de credenciais inválidas
    setupWiFiManagementPage(server);       // Configura a página de gerenciamento WiFi

    // Configura as rotas protegidas e públicas
    configureRoutes(); // Chama a função para configurar as rotas

    // Inicia o servidor web
    server.begin();                      // Inicia o servidor web
    Serial.println("Servidor iniciado"); // Mensagem indicando que o servidor foi iniciado
}

/**
 * Configura as rotas do servidor web.
 */
void configureRoutes()
{
    // Rota de login
    server.on("/login", HTTP_POST, [](AsyncWebServerRequest *request)
              { handleLogin(request); }); // Função para lidar com a requisição de login

    // Rota de logout
    server.on("/logout", HTTP_GET, [](AsyncWebServerRequest *request)
              { handleLogout(request); }); // Função para lidar com a requisição de logout

    // Rota protegida: Dashboard
    server.on("/dashboard", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        if (isAuthenticated(request)) { // Verifica se a requisição está autenticada
            request->send(SPIFFS, "/dashboard", "text/html"); // Envia a página do dashboard
        } else {
            redirectToAccessDenied(request); // Redireciona para a página de acesso negado
        } });

    // Rota protegida: Toggle
    server.on("/toggle", HTTP_ANY, [](AsyncWebServerRequest *request)
              {
        if (isAuthenticated(request)) { // Verifica se a requisição está autenticada
            handleToggleAction(server); // Função para lidar com a ação de toggle
        } else {
            redirectToAccessDenied(request); // Redireciona para a página de acesso negado
        } });

    // Rota para verificar autenticação
    server.on("/check-auth", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        if (isAuthenticated(request)) { // Verifica se a requisição está autenticada
            request->send(200, "application/json", "{\"authenticated\":true}"); // Responde com JSON indicando autenticação bem-sucedida
        } else {
            request->send(200, "application/json", "{\"authenticated\":false}"); // Responde com JSON indicando autenticação falha
        } });
}

/**
 * Redireciona para a página de acesso inválido.
 *
 * @param request A requisição que resultou em acesso não autorizado.
 */
void redirectToAccessDenied(AsyncWebServerRequest *request)
{
    request->redirect("/acesso-invalido"); // Redireciona para a página de acesso inválido
}
