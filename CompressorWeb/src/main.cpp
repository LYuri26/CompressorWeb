// -------------------------------------------------------------------------
// Inclusão de Bibliotecas
// -------------------------------------------------------------------------
#include <WiFi.h>              // Inclui a biblioteca para funcionalidades de WiFi, permitindo a conexão à redes sem fio.
#include <ESPAsyncWebServer.h> // Inclui a biblioteca para criar um servidor web assíncrono, permitindo respostas rápidas a requisições HTTP.
#include <SPIFFS.h>            // Inclui a biblioteca para acessar o sistema de arquivos SPIFFS, usado para armazenar e acessar arquivos no ESP32.

// -------------------------------------------------------------------------
// Cabeçalhos Personalizados
// -------------------------------------------------------------------------
#include "index.h"           // Inclui o cabeçalho que configura a página inicial do servidor web.
#include "autenticador.h"    // Inclui o cabeçalho responsável pela autenticação dos usuários.
#include "dashboard.h"       // Inclui o cabeçalho que configura a página do dashboard.
#include "ligadesliga.h"     // Inclui o cabeçalho com as funções de ligar e desligar dispositivos.
#include "creditos.h"        // Inclui o cabeçalho para a página de créditos.
#include "umidade.h"         // Inclui o cabeçalho para a página que exibe informações de umidade.
#include "pressao.h"         // Inclui o cabeçalho para a página de monitoramento do nível de óleo.
#include "wificonexao.h"     // Inclui o cabeçalho para a configuração da conexão WiFi.
#include "paginaserro.h"     // Inclui o cabeçalho que define as páginas de erro.
#include "tempo.h"           // Inclui o cabeçalho para a configuração e atualização de tempo e horário.
#include "wifigerenciador.h" // Inclui o cabeçalho para o gerenciamento de redes WiFi.
#include "manutencao.h"      // Inclui o cabeçalho para o gerenciamento do modo de manutenção do sistema.

// -------------------------------------------------------------------------
// Configuração do Servidor Web
// -------------------------------------------------------------------------
AsyncWebServer server(80); // Cria um objeto do servidor web que escuta requisições na porta 80, a porta padrão para HTTP.

// -------------------------------------------------------------------------
// Declarações de Funções
// -------------------------------------------------------------------------
void setupSPIFFS();                                          // Declaração da função para inicializar o sistema de arquivos SPIFFS.
void setupServer();                                          // Declaração da função para configurar o servidor web e suas rotas.
void configureRoutes();                                      // Declaração da função para configurar as rotas de requisição do servidor.
bool isAuthenticated(AsyncWebServerRequest *request);        // Declaração da função para verificar se uma requisição está autenticada.
void redirectToAccessDenied(AsyncWebServerRequest *request); // Declaração da função para redirecionar requisições não autorizadas para uma página de acesso negado.

// -------------------------------------------------------------------------
// Variáveis de Controle de Reconexão WiFi
// -------------------------------------------------------------------------
unsigned long lastReconnectAttempt = 0; // Armazena o tempo da última tentativa de reconexão WiFi em milissegundos.
int reconnectAttempts = 0;              // Contador que registra o número de tentativas de reconexão WiFi.

const int MAX_RECONNECT_ATTEMPTS = 10; // Define o número máximo de tentativas de reconexão WiFi antes de entrar no modo AP.

// Definição dos intervalos de tempo para reconexão e atualização
const unsigned long RECONNECT_INTERVAL = 1000; // Define o intervalo de tempo entre tentativas de reconexão WiFi (1 segundo).
const unsigned long UPDATE_INTERVAL = 300000;  // Define o intervalo de tempo para atualização de tempo e status (5 minutos).
const unsigned long RESTART_TIME = 60000;      // Define o tempo de espera após verificar o horário de reinicialização (1 minuto).

// Variáveis para controle de tempo
unsigned long lastUpdate = 0;              // Armazena o tempo da última atualização do tempo e status.
unsigned long lastCompressorUpdate = 0;    // Armazena o tempo da última atualização do status do compressor.

void setup() {
    Serial.begin(115200);  // Inicializa a comunicação serial a uma taxa de 115200 bps para depuração e monitoramento.
    WiFi.mode(WIFI_AP_STA); // Configura o ESP32 para operar em modo dual (Access Point e Station simultaneamente).
    delay(1000);           // Aguarda 1 segundo para garantir que todas as inicializações sejam concluídas.
    setupSPIFFS();         // Chama a função para inicializar o sistema de arquivos SPIFFS.
    loadSavedWiFiNetworks(); // Carrega as redes WiFi salvas do sistema de arquivos.
    setupServer();         // Configura o servidor web e suas rotas.
    setupTimeClient();     // Configura o cliente NTP para sincronização do tempo.
    setupManutencao();     // Configura as funcionalidades de manutenção do sistema.
}

void loop() {
    static unsigned long lastReconnectAttempt = 0; // Variável estática para armazenar o tempo da última tentativa de reconexão, mantendo seu valor entre as execuções do loop.
    static unsigned long lastUpdate = 0;           // Variável estática para armazenar o tempo da última atualização de tempo e status.
    static unsigned long lastCompressorUpdate = 0; // Variável estática para armazenar o tempo da última atualização do status do compressor.

    unsigned long currentMillis = millis(); // Armazena o tempo atual em milissegundos desde que o ESP32 foi ligado.

    if (currentMillis - lastUpdate >= UPDATE_INTERVAL) { // Verifica se o intervalo de tempo para a atualização foi atingido.
        updateTime(); // Atualiza o tempo através de NTP ou outro método.
        lastUpdate = currentMillis; // Atualiza o marcador de tempo para a próxima atualização.
    }

    if (compressorLigado && !sistemaEmManutencao) { // Verifica se o compressor está ligado e se o sistema não está em manutenção.
        if (currentMillis - lastCompressorUpdate >= UPDATE_INTERVAL) { // Verifica se o intervalo para atualização do compressor foi atingido.
            updateCompressorStatus(); // Atualiza o status do compressor.
            lastCompressorUpdate = currentMillis; // Atualiza o marcador de tempo para a próxima atualização do compressor.
        }
    }

    atualizarEstadoManutencao(); // Atualiza o estado do modo de manutenção.

    if (!isAPMode) { // Verifica se o sistema não está em modo Access Point.
        if (WiFi.status() != WL_CONNECTED) { // Verifica se o WiFi não está conectado.
            if (currentMillis - lastReconnectAttempt >= RECONNECT_INTERVAL) { // Verifica se o intervalo de tempo para reconexão foi atingido.
                lastReconnectAttempt = currentMillis; // Atualiza o marcador de tempo para a próxima tentativa de reconexão.
                Serial.println("Conexão WiFi perdida. Tentando reconectar..."); // Imprime mensagem no serial indicando tentativa de reconexão.
                connectToWiFi(ssid, password); // Tenta reconectar ao WiFi usando SSID e senha armazenados.
                reconnectAttempts++; // Incrementa o contador de tentativas de reconexão.
                if (reconnectAttempts >= MAX_RECONNECT_ATTEMPTS) { // Verifica se o número máximo de tentativas de reconexão foi atingido.
                    Serial.println("Número máximo de tentativas de reconexão alcançado. Entrando em modo AP."); // Imprime mensagem no serial indicando mudança para modo AP.
                    setupAP(); // Configura o ESP32 para modo Access Point.
                    reconnectAttempts = 0; // Reseta o contador de tentativas de reconexão.
                }
            }
        }
    } else {
        reconnectAttempts = 0; // Reseta o contador de tentativas de reconexão se o sistema estiver em modo AP.
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
    if (!SPIFFS.begin(true)) // Tenta iniciar o sistema de arquivos SPIFFS com formatação em caso de falha.
    {
        Serial.println("Falha ao iniciar o sistema de arquivos SPIFFS"); // Imprime mensagem de erro no serial se a inicialização falhar.
        return; // Retorna da função, encerrando a execução do setupSPIFFS.
    }
    Serial.println("SPIFFS inicializado com sucesso."); // Imprime mensagem de sucesso no serial se o SPIFFS for inicializado corretamente.
}

/**
 * Configura o servidor e suas rotas.
 */
void setupServer()
{
    Serial.println("Configurando o servidor..."); // Imprime mensagem no serial indicando o início da configuração do servidor.

    // Configura as páginas e rotas do servidor web
    setupIndexPage(server);                // Configura a rota e o conteúdo da página inicial.
    setupCreditosPage(server);             // Configura a rota e o conteúdo da página de créditos.
    setupDashboardPage(server);            // Configura a rota e o conteúdo da página do dashboard.
    setupLigaDesliga(server);              // Configura as rotas e funcionalidades de ligar e desligar dispositivos.
    setupUmidadePage(server);              // Configura a rota e o conteúdo da página de umidade.
    setupPressaoPage(server);              // Configura a rota e o conteúdo da página de monitoramento do nível de óleo.
    setupAcessoInvalidoPage(server);       // Configura a rota e o conteúdo da página de acesso inválido.
    setupNotFoundPage(server);             // Configura a rota e o conteúdo da página de erro 404 (página não encontrada).
    setupUsuarioJaLogadoPage(server);      // Configura a rota e o conteúdo da página de usuário já logado.
    setupCredenciaisInvalidasPage(server); // Configura a rota e o conteúdo da página de credenciais inválidas.
    setupWiFiGerenciadorPage(server);      // Configura a rota e o conteúdo da página de gerenciamento WiFi.

    // Configura as rotas protegidas e públicas
    configureRoutes(); // Chama a função para configurar todas as rotas de requisição do servidor.

    // Inicia o servidor web
    server.begin();                      // Inicia o servidor web, tornando-o ativo e pronto para receber requisições.
    Serial.println("Servidor iniciado"); // Imprime mensagem no serial indicando que o servidor foi iniciado com sucesso.
}

/**
 * Configura as rotas do servidor web.
 */
void configureRoutes()
{
    // Rota para login
    server.on("/login", HTTP_POST, [](AsyncWebServerRequest *request)
              { handleLogin(request); }); // Configura a rota para tratar requisições POST de login com a função handleLogin.

    // Rota para logout
    server.on("/logout", HTTP_GET, [](AsyncWebServerRequest *request)
              { handleLogout(request); }); // Configura a rota para tratar requisições GET de logout com a função handleLogout.

    // Rota protegida: Dashboard
    server.on("/dashboard", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        if (isAuthenticated(request)) { // Verifica se a requisição está autenticada.
            request->send(SPIFFS, "/dashboard", "text/html"); // Se autenticado, envia a página do dashboard armazenada no SPIFFS.
        } else {
            redirectToAccessDenied(request); // Se não autenticado, redireciona para a página de acesso negado.
        } });

    // Rota protegida: Toggle
    server.on("/toggle", HTTP_ANY, [](AsyncWebServerRequest *request)
              {
        if (isAuthenticated(request)) { // Verifica se a requisição está autenticada.
            handleToggleAction(server); // Se autenticado, lida com a ação de ligar/desligar dispositivos.
        } else {
            redirectToAccessDenied(request); // Se não autenticado, redireciona para a página de acesso negado.
        } });

    // Rota para verificar autenticação
    server.on("/check-auth", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        if (isAuthenticated(request)) { // Verifica se a requisição está autenticada.
            request->send(200, "application/json", "{\"authenticated\":true}"); // Se autenticado, responde com JSON indicando sucesso.
        } else {
            request->send(200, "application/json", "{\"authenticated\":false}"); // Se não autenticado, responde com JSON indicando falha.
        } });
}

/**
 * Redireciona para a página de acesso inválido.
 *
 * @param request A requisição que resultou em acesso não autorizado.
 */
void redirectToAccessDenied(AsyncWebServerRequest *request)
{
    request->redirect("/acesso-invalido"); // Redireciona para a página de acesso negado caso o usuário não esteja autenticado.
}
