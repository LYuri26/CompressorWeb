/**
 * ============================================================================
 * PROJETO: EnergizAR - Sistema de Automação e Monitoramento ESP32
 * ARQUIVO: src/main.cpp
 * DESCRIÇÃO: Orquestrador principal do firmware industrial. Gerencia o ciclo de
 *            vida do hardware, o escalonamento cooperativo não-bloqueante de
 *            tarefas, watchdog de hardware, métricas de memória, roteamento
 *            REST assíncrono e proteções de intertravamento dos compressores.
 * ============================================================================
 *
 * ARQUITETURA DO LOOP PRINCIPAL (COOPERATIVO NÃO-BLOQUEANTE):
 * +--------------------------------------------------------------------------+
 * | loop()                                                                   |
 * |  |--> [Sub-ms] processarPulsosMotores()   (Desliga pulsos de 500 ms)     |
 * |  |--> [Sub-ms] processarConexaoWiFi()     (Máquina de estados Wi-Fi)     |
 * |  |--> [Sub-ms] atualizarEstadoManutencao() (Filtro da chave física)      |
 * |  |--> [ 100ms] monitorarSobrecarga()      (Relés térmicos)               |
 * |  |--> [ 200ms] verificarPressostato()     (Sensor de pressão de linha)   |
 * |  |--> [1000ms] monitorarStatusCompressores() (Feedback dos contatores)   |
 * |  |--> [5000ms] atualizarEstadoMotores()   (Sincronização e travas)       |
 * |  |--> [60 seg] updateTimers()             (Contagem de descanso)         |
 * |  |--> [ 5 min] updateTime()               (Sincronização SNTP)           |
 * |  +--> [ 5 seg] Alimentação do Watchdog de Hardware (TWDT)                |
 * +--------------------------------------------------------------------------+
 */

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <esp_task_wdt.h>
#include <esp_system.h>

// Módulos do Sistema de Automação
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
#include "pressostato.h"

// -------------------------------------------------------------------------
// 1. Constantes e Configurações do Watchdog e Tarefas
// -------------------------------------------------------------------------

// Tempo de expiração do Watchdog por Hardware (10 segundos)
#define WDT_TIMEOUT_SECONDS 10

// Servidor Web Assíncrono na porta padrão HTTP 80
AsyncWebServer server(80);

// -------------------------------------------------------------------------
// 2. Variáveis de Temporização Não-Bloqueante (Escalonamento Cooperativo)
// -------------------------------------------------------------------------

static unsigned long lastOverloadCheck = 0;
const unsigned long INTERVALO_OVERLOAD_MS = 100; // Varredura térmica rápida (100 ms)

static unsigned long lastPressostatoCheck = 0;
const unsigned long INTERVALO_PRESSAO_MS = 200; // Leitura do pressostato (200 ms)

static unsigned long lastCompressorCheck = 0;
const unsigned long INTERVALO_STATUS_MS = 1000; // Feedback físico dos contatores (1 s)

static unsigned long lastSyncMotoresCheck = 0;
const unsigned long INTERVALO_SYNCMOTOR_MS = 5000; // Validação de travas de horário (5 s)

static unsigned long lastTimerUpdate = 0;
const unsigned long INTERVALO_TIMERS_MS = 60000; // Atualização dos cronômetros (1 min)

static unsigned long lastUpdateNTP = 0;
const unsigned long INTERVALO_NTP_MS = 300000; // Sincronização periódica NTP (5 min)

static unsigned long lastTelemetryLog = 0;
const unsigned long INTERVALO_TELEMETRIA_MS = 30000; // Log de saúde do hardware (30 s)

// -------------------------------------------------------------------------
// 3. Métricas de Performance do Loop e Diagnóstico
// -------------------------------------------------------------------------

static unsigned long loopCounter = 0;
static unsigned long lastLoopFreqMeasure = 0;
static float loopFrequencyHz = 0.0f;
static uint32_t minRecordedFreeHeap = 0xFFFFFFFF;

// -------------------------------------------------------------------------
// 4. Protótipos de Funções Internas
// -------------------------------------------------------------------------

void setupServer();
void configureRoutes();
void redirectToAccessDenied(AsyncWebServerRequest *request);
static void inicializarWatchdog();
static void imprimirDiagnosticoBoot();
static const char *obterDescricaoReset(esp_reset_reason_t reason);
static void registrarMetricasLoop();

// -------------------------------------------------------------------------
// 5. Configuração e Inicialização do Watchdog de Hardware
// -------------------------------------------------------------------------

/**
 * Inicializa o Task Watchdog Timer (TWDT) do ESP32.
 * Se o firmware travar em algum laço infinito por mais de 10 segundos,
 * o hardware reinicializa a CPU automaticamente em modo seguro.
 */
static void inicializarWatchdog()
{
    Serial.println("[WATCHDOG] Configurando cão de guarda por hardware (TWDT)...");
    esp_task_wdt_init(WDT_TIMEOUT_SECONDS, true); // true = dispara reinicialização
    esp_task_wdt_add(NULL);                       // Adiciona a tarefa atual (loopTask)
    Serial.printf("[WATCHDOG] Watchdog ativo com timeout de %d segundos.\n", WDT_TIMEOUT_SECONDS);
}

// -------------------------------------------------------------------------
// 6. Diagnóstico de Inicialização e Saúde da CPU
// -------------------------------------------------------------------------

/**
 * Traduz o código de reset do hardware para diagnóstico em console.
 */
static const char *obterDescricaoReset(esp_reset_reason_t reason)
{
    switch (reason)
    {
    case ESP_RST_POWERON:
        return "Energização Inicial (Power-on)";
    case ESP_RST_EXT:
        return "Reset Externo via Pino EN/RST";
    case ESP_RST_SW:
        return "Reinicialização via Software (esp_restart)";
    case ESP_RST_PANIC:
        return "Falha Crítica de Software (Kernel Panic / Exception)";
    case ESP_RST_INT_WDT:
        return "Watchdog de Interrupção Disparado";
    case ESP_RST_TASK_WDT:
        return "Task Watchdog Disparado (Loop Congelado)";
    case ESP_RST_WDT:
        return "Watchdog Geral de Hardware Disparado";
    case ESP_RST_DEEPSLEEP:
        return "Retorno do Modo Deep Sleep";
    case ESP_RST_BROWNOUT:
        return "Queda de Tensão Elétrica na Alimentação (Brownout)";
    case ESP_RST_SDIO:
        return "Reset disparado via SDIO";
    default:
        return "Causa de Reset Não Identificada";
    }
}

/**
 * Imprime telemetria completa dos componentes do microcontrolador na inicialização.
 */
static void imprimirDiagnosticoBoot()
{
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);

    esp_reset_reason_t reason = esp_reset_reason();

    Serial.println("==================================================");
    Serial.println("   ENERGIZAR - SISTEMA DE AUTOMAÇÃO INDUSTRIAL    ");
    Serial.println("==================================================");
    Serial.printf("[BOOT] Motivo do Reset: %s\n", obterDescricaoReset(reason));
    Serial.printf("[BOOT] Modelo do Chip: ESP32 Rev %d | Núcleos: %d\n",
                  chip_info.revision, chip_info.cores);
    Serial.printf("[BOOT] Clock da CPU: %d MHz\n", getCpuFrequencyMhz());
    Serial.printf("[BOOT] Memória Flash: %d MB (Velocidade: %d MHz)\n",
                  ESP.getFlashChipSize() / (1024 * 1024),
                  ESP.getFlashChipSpeed() / 1000000);
    Serial.printf("[BOOT] Heap Inicial Livre: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("[BOOT] Menor Heap Já Registrado: %d bytes\n", ESP.getMinFreeHeap());
    Serial.printf("[BOOT] Endereço MAC do Rádio: %s\n", WiFi.macAddress().c_str());
    Serial.println("==================================================");
}

// -------------------------------------------------------------------------
// 7. Setup Geral do Sistema (Executado uma única vez)
// -------------------------------------------------------------------------

void setup()
{
    // 1. Inicializa comunicação serial industrial de depuração
    Serial.begin(115200);
    delay(300); // Aguarda estabilização das fontes de alimentação

    // 2. Emite o relatório de diagnóstico do hardware
    imprimirDiagnosticoBoot();

    // 3. Inicializa o cão de guarda de segurança
    inicializarWatchdog();

    // 4. Inicializa o sistema de arquivos SPIFFS
    Serial.println("[SETUP] Inicializando sistema de arquivos SPIFFS...");
    if (!SPIFFS.begin(true))
    {
        Serial.println("[SETUP] ERRO CRÍTICO: Falha ao montar o SPIFFS. Tentando formatar...");
    }
    else
    {
        Serial.printf("[SETUP] SPIFFS Montado com Sucesso. (Usado: %d / Total: %d bytes)\n",
                      SPIFFS.usedBytes(), SPIFFS.totalBytes());
    }

    // 5. Configura o subsistema de rede sem fio (Modo AP + Conexão com Roteadores)
    Serial.println("[SETUP] Inicializando subsistema Wi-Fi assíncrono...");
    setupAP();
    loadSavedWiFiNetworks();

    // 6. Configura a sincronização de tempo nativa SNTP do ESP-IDF
    Serial.println("[SETUP] Configurando cliente de relógio NTP nativo...");
    setupTimeClient();

    // 7. Configura módulos de hardware, entradas de intertravamento e proteções térmicas
    Serial.println("[SETUP] Configurando entradas de manutenção e sobrecarga...");
    setupManutencao();
    setupSobrecarga();
    setupPressostato();

    // 8. Inicializa controle dos motores (Saídas de pulso de relé e leituras de status)
    Serial.println("[SETUP] Configurando saídas de acionamento dos compressores...");
    setupLigaDesliga(server);

    // 9. Configura rotas HTTP e inicializa o servidor web assíncrono
    Serial.println("[SETUP] Configurando endpoints e servidor web...");
    setupServer();

    // 10. Executa a primeira validação de consistência dos sensores
    verificarPressostato();
    atualizarEstadoMotores();

    // Registra a menor marca de memória inicial
    minRecordedFreeHeap = ESP.getFreeHeap();

    Serial.println("==================================================");
    Serial.println("[SETUP] INICIALIZAÇÃO INDUSTRIAL CONCLUÍDA COM ÊXITO.");
    Serial.println("[SETUP] O sistema agora opera em regime de tarefas não-bloqueantes.");
    Serial.println("==================================================");
}

// -------------------------------------------------------------------------
// 8. Loop Principal (Execução Cooperativa Contínua Não-Bloqueante)
// -------------------------------------------------------------------------

void loop()
{
    // Alimenta o Watchdog de hardware para sinalizar que o processador está vivo
    esp_task_wdt_reset();

    unsigned long currentMillis = millis();

    // ---------------------------------------------------------------------
    // TAREFA 1: PRIORIDADE MÁXIMA - Pulso dos Contatores (Sub-milissegundo)
    // ---------------------------------------------------------------------
    // Desliga os pinos de pulso de 500 ms assim que o tempo expira, sem atrasos
    processarPulsosMotores();

    // ---------------------------------------------------------------------
    // TAREFA 2: Gerenciamento Contínuo da Conexão Wi-Fi em Segundo Plano
    // ---------------------------------------------------------------------
    // Trata timeouts e reconexão automática sem prender o processador
    processarConexaoWiFi();

    // ---------------------------------------------------------------------
    // TAREFA 3: Monitoramento Contínuo da Chave Física de Manutenção
    // ---------------------------------------------------------------------
    // Lê o pino GPIO 4 e processa o debounce e religamento sequencial seguro
    atualizarEstadoManutencao();

    // ---------------------------------------------------------------------
    // TAREFA 4: Varredura Rápida dos Relés Térmicos de Sobrecarga (100 ms)
    // ---------------------------------------------------------------------
    if (currentMillis - lastOverloadCheck >= INTERVALO_OVERLOAD_MS)
    {
        monitorarSobrecarga();
        lastOverloadCheck = currentMillis;
    }

    // ---------------------------------------------------------------------
    // TAREFA 5: Monitoramento do Pressostato de Segurança (200 ms)
    // ---------------------------------------------------------------------
    if (currentMillis - lastPressostatoCheck >= INTERVALO_PRESSAO_MS)
    {
        verificarPressostato();
        lastPressostatoCheck = currentMillis;
    }

    // ---------------------------------------------------------------------
    // TAREFA 6: Feedback Físico dos Contatores dos Compressores (1000 ms)
    // ---------------------------------------------------------------------
    if (currentMillis - lastCompressorCheck >= INTERVALO_STATUS_MS)
    {
        monitorarStatusCompressores();
        lastCompressorCheck = currentMillis;
    }

    // ---------------------------------------------------------------------
    // TAREFA 7: Verificação de Horários Operacionais e Travas Globais (5000 ms)
    // ---------------------------------------------------------------------
    if (currentMillis - lastSyncMotoresCheck >= INTERVALO_SYNCMOTOR_MS)
    {
        atualizarEstadoMotores();
        lastSyncMotoresCheck = currentMillis;
    }

    // ---------------------------------------------------------------------
    // TAREFA 8: Atualização dos Temporizadores de Descanso dos Motores (60 s)
    // ---------------------------------------------------------------------
    if (currentMillis - lastTimerUpdate >= INTERVALO_TIMERS_MS)
    {
        updateTimers();
        lastTimerUpdate = currentMillis;
    }

    // ---------------------------------------------------------------------
    // TAREFA 9: Sincronização do Relógio com Servidores NTP (5 minutos)
    // ---------------------------------------------------------------------
    if (currentMillis - lastUpdateNTP >= INTERVALO_NTP_MS)
    {
        updateTime();
        lastUpdateNTP = currentMillis;
    }

    // ---------------------------------------------------------------------
    // TAREFA 10: Telemetria de Saúde do Sistema e Frequência do Loop (30 s)
    // ---------------------------------------------------------------------
    if (currentMillis - lastTelemetryLog >= INTERVALO_TELEMETRIA_MS)
    {
        registrarMetricasLoop();
        lastTelemetryLog = currentMillis;
    }

    // Incrementa o contador de iterações para cálculo de ciclos por segundo
    loopCounter++;
}

// -------------------------------------------------------------------------
// 9. Medição de Desempenho e Saúde de Memória
// -------------------------------------------------------------------------

/**
 * Calcula a taxa de iterações do loop por segundo (Hz) e audita o consumo de RAM.
 */
static void registrarMetricasLoop()
{
    unsigned long agora = millis();
    unsigned long delta = agora - lastLoopFreqMeasure;

    if (delta > 0)
    {
        loopFrequencyHz = ((float)loopCounter * 1000.0f) / (float)delta;
    }

    loopCounter = 0;
    lastLoopFreqMeasure = agora;

    uint32_t currentFreeHeap = ESP.getFreeHeap();
    if (currentFreeHeap < minRecordedFreeHeap)
    {
        minRecordedFreeHeap = currentFreeHeap;
    }

    Serial.println("--------------------------------------------------");
    Serial.printf("[TELEMETRIA] Frequência de Execução do Loop: %.2f Hz\n", loopFrequencyHz);
    Serial.printf("[TELEMETRIA] Memória Heap Atual: %u bytes | Mínima Histórica: %u bytes\n",
                  currentFreeHeap, minRecordedFreeHeap);
    Serial.printf("[TELEMETRIA] Uptime do Sistema: %s\n", getUptimeFormatted().c_str());
    Serial.printf("[TELEMETRIA] Hora do Sistema: %s (NTP Sincronizado: %s)\n",
                  getFormattedTimeBR().c_str(), isTimeSynchronized() ? "SIM" : "NÃO");
    Serial.printf("[TELEMETRIA] Estado Wi-Fi: %s | Modo AP: %s\n",
                  isWiFiConnected() ? "CONECTADO" : "DESCONECTADO",
                  isAPMode ? "SIM" : "NÃO");
    Serial.printf("[TELEMETRIA] Motores Ativos: M1=%s, M2=%s, M3=%s\n",
                  motoresLigados[0] ? "ON" : "OFF",
                  motoresLigados[1] ? "ON" : "OFF",
                  motoresLigados[2] ? "ON" : "OFF");
    Serial.println("--------------------------------------------------");
}

// -------------------------------------------------------------------------
// 10. Configuração do Servidor Web e Montagem das Telas
// -------------------------------------------------------------------------

void setupServer()
{
    Serial.println("[HTTP] Inicializando montagem dos módulos de página web...");

    // Telas Operacionais e Informativas
    setupIndexPage(server);
    setupCreditosPage(server);
    setupDashboardPage(server);
    setupUmidadePage(server);
    setupPressaoPage(server);
    setupWiFiGerenciadorPage(server);

    // Telas de Erro e Exceção HTTP
    setupAcessoInvalidoPage(server);
    setupNotFoundPage(server);
    setupUsuarioJaLogadoPage(server);
    setupCredenciaisInvalidasPage(server);

    // Registra APIs REST, endpoints JSON e controles administrativos
    configureRoutes();

    // Inicializa o serviço HTTP
    server.begin();
    Serial.println("[HTTP] Servidor Web ativo e aguardando requisições na porta 80.");
}

// -------------------------------------------------------------------------
// 11. Rotas de API REST, Autenticação e Telemetria
// -------------------------------------------------------------------------

void configureRoutes()
{
    // ---------------------------------------------------------------------
    // ROTA API 1: Processamento de Login (POST)
    // ---------------------------------------------------------------------
    server.on("/login", HTTP_POST, [](AsyncWebServerRequest *request)
              { handleLogin(request); });

    // ---------------------------------------------------------------------
    // ROTA API 2: Encerramento de Sessão (Logout)
    // ---------------------------------------------------------------------
    server.on("/logout", HTTP_GET, [](AsyncWebServerRequest *request)
              { handleLogout(request); });

    // ---------------------------------------------------------------------
    // ROTA API 3: Verificação de Sessão Ativa
    // ---------------------------------------------------------------------
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

    // ---------------------------------------------------------------------
    // ROTA API 4: Status dos Motores (Direto da RAM - Zero Desgaste da Flash)
    // ---------------------------------------------------------------------
    server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        String statusJSON = "{";
        for (int i = 0; i < NUM_MOTORES; i++)
        {
            statusJSON += "\"motor" + String(i + 1) + "\": \"" + (motoresLigados[i] ? "ON" : "OFF") + "\"";
            if (i < NUM_MOTORES - 1)
            {
                statusJSON += ", ";
            }
        }
        statusJSON += "}";

        request->send(200, "application/json", statusJSON); });

    // ---------------------------------------------------------------------
    // ROTA API 5: Telemetria Completa de Diagnóstico do Sistema (JSON)
    // ---------------------------------------------------------------------
    server.on("/api/telemetria", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        String json = "{";
        json += "\"heap_livre\":" + String(ESP.getFreeHeap()) + ",";
        json += "\"heap_minimo\":" + String(minRecordedFreeHeap) + ",";
        json += "\"loop_frequencia_hz\":" + String(loopFrequencyHz, 1) + ",";
        json += "\"uptime_segundos\":" + String(getUptimeSeconds()) + ",";
        json += "\"uptime\":\"" + getUptimeFormatted() + "\",";
        json += "\"hora_atual\":\"" + getFormattedTimeBR() + "\",";
        json += "\"ntp_sincronizado\":" + String(isTimeSynchronized() ? "true" : "false") + ",";
        json += "\"wifi_conectado\":" + String(isWiFiConnected() ? "true" : "false") + ",";
        json += "\"modo_ap\":" + String(isAPMode ? "true" : "false") + ",";
        json += "\"manutencao\":" + String(sistemaEmManutencao ? "true" : "false") + ",";
        json += "\"pressostato\":" + String(pressostatoAtivo ? "true" : "false") + ",";
        json += "\"motores\":[";
        for (int i = 0; i < NUM_MOTORES; i++)
        {
            json += "{\"id\":" + String(i + 1) + ",";
            json += "\"ligado\":" + String(motoresLigados[i] ? "true" : "false") + ",";
            json += "\"sobrecarga\":" + String(sobrecargaDetectada[i] ? "true" : "false") + ",";
            json += "\"timer_ativo\":" + String(isMotorTimerActive(i) ? "true" : "false") + "}";
            if (i < NUM_MOTORES - 1) json += ",";
        }
        json += "]}";

        request->send(200, "application/json", json); });

    // ---------------------------------------------------------------------
    // ROTA API 6: Parada Geral de Emergência (Corte Instantâneo dos Contatores)
    // ---------------------------------------------------------------------
    server.on("/api/emergencia", HTTP_POST, [](AsyncWebServerRequest *request)
              {
        if (!isAuthenticated(request))
        {
            request->send(401, "application/json", "{\"erro\":\"nao_autorizado\"}");
            return;
        }

        Serial.println("==================================================");
        Serial.println("[EMERGÊNCIA] COMANDO DE CORTE GERAL RECEBIDO VIA API!");
        Serial.println("==================================================");

        desligarTodosMotores();

        request->send(200, "application/json", "{\"status\":\"todos_motores_desligados\"}"); });

    // ---------------------------------------------------------------------
    // ROTA API 7: Reinicialização Segura do Sistema (Reboot Remoto)
    // ---------------------------------------------------------------------
    server.on("/api/reiniciar", HTTP_POST, [](AsyncWebServerRequest *request)
              {
        if (!isAuthenticated(request))
        {
            request->send(401, "application/json", "{\"erro\":\"nao_autorizado\"}");
            return;
        }

        Serial.println("[SISTEMA] Comando de reinicialização remota recebido. Desarmando contatores...");
        desligarTodosMotores();

        request->send(200, "application/json", "{\"status\":\"reiniciando_em_2_segundos\"}");

        // Agenda o reset para 2 segundos permitindo que a resposta HTTP chegue ao cliente
        xTaskCreate([](void *p) {
            vTaskDelay(pdMS_TO_TICKS(2000));
            esp_restart();
        }, "reboot_task", 2048, NULL, 1, NULL); });
}

// -------------------------------------------------------------------------
// 12. Funções de Redirecionamento de Acesso
// -------------------------------------------------------------------------

/**
 * Redireciona requisições não autenticadas para a página de aviso de segurança.
 */
void redirectToAccessDenied(AsyncWebServerRequest *request)
{
    request->redirect("/acesso-invalido");
}