// -------------------------------------------------------------------------
// Inclusão de Bibliotecas
// -------------------------------------------------------------------------
#include <WiFi.h>              // Biblioteca para gerenciar a conexão Wi-Fi no ESP32. Fornece funções para conectar-se a redes Wi-Fi e gerenciar conexões.
#include <ESPAsyncWebServer.h> // Biblioteca para criar um servidor web assíncrono no ESP32, que permite manipulação de requisições HTTP de forma não bloqueante.
#include <SPIFFS.h>            // Biblioteca para acessar o sistema de arquivos SPIFFS (SPI Flash File System), que permite armazenar e acessar arquivos no flash do ESP32.

// -------------------------------------------------------------------------
// Cabeçalhos Personalizados
// -------------------------------------------------------------------------
#include "index.h"           // Inclui o cabeçalho que define funções para configurar a página inicial do servidor web.
#include "autenticador.h"    // Inclui o cabeçalho que define funções para autenticação de usuários.
#include "dashboard.h"       // Inclui o cabeçalho para funções relacionadas ao painel de controle do servidor.
#include "ligadesliga.h"     // Inclui o cabeçalho para funções de ligar e desligar dispositivos.
#include "creditos.h"        // Inclui o cabeçalho para funções relacionadas à exibição de créditos ou informações de autoria.
#include "umidade.h"         // Inclui o cabeçalho para funções relacionadas à leitura de umidade.
#include "pressao.h"         // Inclui o cabeçalho para funções relacionadas à leitura de pressão.
#include "wificonexao.h"     // Inclui o cabeçalho para funções de conexão Wi-Fi.
#include "paginaserro.h"     // Inclui o cabeçalho para funções que geram páginas de erro personalizadas.
#include "tempo.h"           // Inclui o cabeçalho para funções relacionadas à gestão de tempo e data.
#include "wifigerenciador.h" // Inclui o cabeçalho para funções de gerenciamento de redes Wi-Fi.
#include "manutencao.h"      // Inclui o cabeçalho para funções de manutenção do sistema.

// -------------------------------------------------------------------------
// Configuração do Servidor Web
// -------------------------------------------------------------------------
AsyncWebServer server(80); // Cria uma instância do servidor web assíncrono na porta 80 (padrão para HTTP).

// -------------------------------------------------------------------------
// Declarações de Funções
// -------------------------------------------------------------------------
void setupSPIFFS();                                          // Declara a função para configurar o sistema de arquivos SPIFFS.
void setupServer();                                          // Declara a função para configurar o servidor web.
void configureRoutes();                                      // Declara a função para configurar as rotas do servidor web.
bool isAuthenticated(AsyncWebServerRequest *request);        // Declara a função para verificar se a requisição foi autenticada.
void redirectToAccessDenied(AsyncWebServerRequest *request); // Declara a função para redirecionar para uma página de acesso negado.
void updateTime();                                           // Declara a função para atualizar o tempo.
void setupTimeClient();                                      // Declara a função para configurar o cliente de tempo.
void setupManutencao();                                      // Declara a função para configurar o modo de manutenção.
void connectToWiFi(const char *ssid, const char *password);  // Declara a função para conectar-se a uma rede Wi-Fi.
void setupAP();                                              // Declara a função para configurar o modo Access Point (AP).
void loadSavedWiFiNetworks();                                // Declara a função para carregar redes Wi-Fi salvas.
void atualizarEstadoManutencao();                            // Declara a função para atualizar o estado de manutenção.

// -------------------------------------------------------------------------
// Variáveis de Controle de Reconexão WiFi
// -------------------------------------------------------------------------
unsigned long lastReconnectAttempt = 0;        // Armazena o timestamp da última tentativa de reconexão Wi-Fi.
int reconnectAttempts = 0;                     // Conta o número de tentativas de reconexão Wi-Fi.
const int MAX_RECONNECT_ATTEMPTS = 10;         // Define o número máximo de tentativas de reconexão antes de entrar em modo AP.
const unsigned long RECONNECT_INTERVAL = 1000; // Intervalo entre tentativas de reconexão (em milissegundos).
const unsigned long UPDATE_INTERVAL = 300000;  // Intervalo para atualizar o tempo e status dos motores (em milissegundos).
const unsigned long RESTART_TIME = 60000;      // Tempo para reiniciar o sistema em milissegundos.

// Variáveis para controle de tempo
unsigned long lastUpdate = 0;           // Armazena o timestamp da última atualização de tempo.
unsigned long lastCompressorUpdate = 0; // Armazena o timestamp da última atualização do status do compressor.

// -------------------------------------------------------------------------
// Função de Setup
// -------------------------------------------------------------------------
void setup()
{
    Serial.begin(115200);    // Inicializa a comunicação serial com a taxa de transmissão de 115200 bps (bits por segundo).
    SPIFFS.begin(true);      // Inicializa o sistema de arquivos SPIFFS. O parâmetro 'true' indica que deve formatar o sistema se falhar ao iniciar.
    setupAP();               // Configura e inicia o modo Access Point (AP) para configuração inicial ou quando a conexão Wi-Fi falhar.
    loadSavedWiFiNetworks(); // Tenta conectar-se a redes Wi-Fi salvas previamente.
    setupServer();           // Configura o servidor web e define as rotas para as páginas e ações.
    setupTimeClient();       // Configura o cliente de tempo para sincronizar a hora com um servidor NTP (Network Time Protocol).
    setupManutencao();       // Configura o modo de manutenção para o sistema.
}

// -------------------------------------------------------------------------
// Função de Loop
// -------------------------------------------------------------------------
void loop()
{
    unsigned long currentMillis = millis(); // Obtém o tempo atual em milissegundos desde que o programa começou (milissegundos desde o boot).

    // Atualiza o tempo periodicamente
    if (currentMillis - lastUpdate >= UPDATE_INTERVAL) // Verifica se o intervalo de atualização foi atingido
    {
        updateTime();               // Chama a função para atualizar o tempo
        lastUpdate = currentMillis; // Atualiza o timestamp da última atualização
    }

    // Atualiza o status dos motores periodicamente
    if (currentMillis - lastCompressorUpdate >= UPDATE_INTERVAL) // Verifica se o intervalo de atualização foi atingido
    {
        updateMotorStatus();                  // Chama a função para atualizar o status dos motores
        lastCompressorUpdate = currentMillis; // Atualiza o timestamp da última atualização do compressor
    }

    atualizarEstadoManutencao(); // Chama a função para atualizar o estado de manutenção

    // Gerencia a conexão Wi-Fi
    if (!isAPMode && WiFi.status() != WL_CONNECTED) // Se não estiver em modo AP e a conexão Wi-Fi foi perdida
    {
        if (currentMillis - lastReconnectAttempt >= RECONNECT_INTERVAL) // Se o intervalo de reconexão foi atingido
        {
            lastReconnectAttempt = currentMillis;                           // Atualiza o timestamp da última tentativa de reconexão
            Serial.println("Conexão WiFi perdida. Tentando reconectar..."); // Imprime uma mensagem de status para o console serial
            connectToWiFi(ssid, password);                                  // Tenta reconectar à rede Wi-Fi com o SSID e senha fornecidos
            reconnectAttempts++;                                            // Incrementa o número de tentativas de reconexão
            if (reconnectAttempts >= MAX_RECONNECT_ATTEMPTS)                // Se o número máximo de tentativas for alcançado
            {
                Serial.println("Número máximo de tentativas de reconexão alcançado. Entrando em modo AP."); // Imprime uma mensagem de status
                setupAP();                                                                                  // Configura o modo Access Point (AP) para permitir configuração manual ou conexão de dispositivos
                reconnectAttempts = 0;                                                                      // Redefine o contador de tentativas de reconexão
            }
        }
    }
    else
    {
        reconnectAttempts = 0; // Redefine o contador de tentativas de reconexão se estiver conectado ou em modo AP
    }
}

// -------------------------------------------------------------------------
// Funções Auxiliares
// -------------------------------------------------------------------------
void setupSPIFFS()
{
    if (!SPIFFS.begin(true)) // Tenta inicializar o sistema de arquivos SPIFFS
    {
        Serial.println("Falha ao iniciar o sistema de arquivos SPIFFS"); // Imprime uma mensagem de erro se a inicialização falhar
        return;                                                          // Sai da função se a inicialização falhar
    }
    Serial.println("SPIFFS inicializado com sucesso."); // Imprime uma mensagem de sucesso se a inicialização for bem-sucedida
}

void setupServer()
{
    Serial.println("Configurando o servidor..."); // Imprime uma mensagem de status indicando que a configuração do servidor está começando

    setupIndexPage(server);                // Configura a rota para a página inicial
    setupCreditosPage(server);             // Configura a rota para a página de créditos
    setupDashboardPage(server);            // Configura a rota para o painel de controle
    setupLigaDesliga(server);              // Configura a rota para ações de ligar e desligar dispositivos
    setupUmidadePage(server);              // Configura a rota para a página de umidade
    setupPressaoPage(server);              // Configura a rota para a página de pressão
    setupAcessoInvalidoPage(server);       // Configura a rota para a página de acesso inválido
    setupNotFoundPage(server);             // Configura a rota para a página de erro 404 (não encontrado)
    setupUsuarioJaLogadoPage(server);      // Configura a rota para a página de usuário já logado
    setupCredenciaisInvalidasPage(server); // Configura a rota para a página de credenciais inválidas
    setupWiFiGerenciadorPage(server);      // Configura a rota para a página de gerenciamento de Wi-Fi

    configureRoutes();                   // Configura as rotas e manipuladores de requisições para o servidor web
    server.begin();                      // Inicia o servidor web
    Serial.println("Servidor iniciado"); // Imprime uma mensagem indicando que o servidor foi iniciado com sucesso
}

void configureRoutes()
{
    server.on("/login", HTTP_POST, [](AsyncWebServerRequest *request) // Define a rota para a ação de login usando o método POST
              { handleLogin(request); });                             // Chama a função para processar a requisição de login

    server.on("/logout", HTTP_GET, [](AsyncWebServerRequest *request) // Define a rota para a ação de logout usando o método GET
              { handleLogout(request); });                            // Chama a função para processar a requisição de logout

    server.on("/dashboard", HTTP_GET, [](AsyncWebServerRequest *request) // Define a rota para a página do painel de controle usando o método GET
              {
        if (isAuthenticated(request)) { // Verifica se a requisição foi autenticada
            request->send(SPIFFS, "/dashboard", "text/html"); // Envia a página do painel de controle se autenticado
        } else {
            redirectToAccessDenied(request); // Redireciona para a página de acesso negado se não autenticado
        } });

    server.on("/toggle", HTTP_ANY, [](AsyncWebServerRequest *request) // Define a rota para a ação de alternar dispositivos usando qualquer método HTTP
              {
        if (isAuthenticated(request)) { // Verifica se a requisição foi autenticada
            handleToggleAction(server); // Chama a função para processar a ação de alternar dispositivos
        } else {
            redirectToAccessDenied(request); // Redireciona para a página de acesso negado se não autenticado
        } });

    server.on("/check-auth", HTTP_GET, [](AsyncWebServerRequest *request) // Define a rota para verificar a autenticação usando o método GET
              {
        if (isAuthenticated(request)) { // Verifica se a requisição foi autenticada
            request->send(200, "application/json", "{\"authenticated\":true}"); // Envia uma resposta JSON indicando que está autenticado
        } else {
            request->send(200, "application/json", "{\"authenticated\":false}"); // Envia uma resposta JSON indicando que não está autenticado
        } });
}

void redirectToAccessDenied(AsyncWebServerRequest *request)
{
    request->redirect("/acesso-invalido"); // Redireciona a requisição para a página de acesso negado
}
