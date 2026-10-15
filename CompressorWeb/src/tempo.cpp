/**
 * ============================================================================
 * PROJETO: EnergizAR - Sistema de Automação e Monitoramento ESP32
 * ARQUIVO: src/tempo.cpp
 * DESCRIÇÃO: Subsistema de tempo real (RTC) e sincronização SNTP industrial.
 *            Utiliza as rotinas de baixo nível do ESP-IDF (esp_sntp.h) para
 *            manter o relógio de alta precisão em segundo plano, sem travamentos
 *            de loop e sem alocação dinâmica de buffers UDP desnecessários.
 * ============================================================================
 */

#include "tempo.h"
#include <WiFi.h>
#include <esp_sntp.h>

// -------------------------------------------------------------------------
// 1. Configurações de Fuso Horário e Servidores NTP de Referência
// -------------------------------------------------------------------------

// Servidores primários do Observatório Nacional / Registro.br (Baixíssima latência no Brasil)
static const char *NTP_SERVER_BR_A = "a.st1.ntp.br";
static const char *NTP_SERVER_BR_B = "b.st1.ntp.br";
// Servidor de contingência internacional
static const char *NTP_SERVER_POOL = "pool.ntp.org";

// Fuso horário padrão: América/São Paulo (UTC-3 = -3 * 3600 = -10800 segundos)
static long gmtOffsetSeconds = -10800;
static int daylightOffsetSeconds = 0;

// Limite temporal mínimo considerado válido (1º de janeiro de 2024 = 1704067200)
// Evita falsos positivos antes da primeira sincronização real do NTP
static const time_t TIMESTAMP_MINIMO_VALIDO = 1704067200;

// -------------------------------------------------------------------------
// 2. Variáveis de Estado Interno e Cache de Temporização
// -------------------------------------------------------------------------

// Armazena a string de compatibilidade para ligadesliga.cpp
static String currentTime = "2026-01-01T00:00";

// Flags e estatísticas de sincronização
static bool timeSyncAchieved = false;
static unsigned long lastSyncTimestamp = 0;
static unsigned long syncSuccessCount = 0;
static unsigned long syncFailCount = 0;
static unsigned long bootMillis = 0;

// Controle de log periódico de diagnóstico
static unsigned long lastRoutineLog = 0;
static const unsigned long LOG_ROUTINE_INTERVAL_MS = 60000; // 1 minuto

// -------------------------------------------------------------------------
// 3. Callback de Notificação do SNTP Nativo do ESP-IDF
// -------------------------------------------------------------------------

/**
 * Função executada automaticamente pelo núcleo do ESP32 assim que um pacote
 * NTP é recebido e o RTC interno é disciplinado com sucesso.
 */
static void timeSyncNotificationCallback(struct timeval *tv)
{
    timeSyncAchieved = true;
    lastSyncTimestamp = millis();
    syncSuccessCount++;

    time_t now = tv->tv_sec;
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);

    char dataHoraLog[32];
    snprintf(dataHoraLog, sizeof(dataHoraLog), "%02d/%02d/%04d %02d:%02d:%02d",
             timeinfo.tm_mday,
             timeinfo.tm_mon + 1,
             timeinfo.tm_year + 1900,
             timeinfo.tm_hour,
             timeinfo.tm_min,
             timeinfo.tm_sec);

    Serial.println("==================================================");
    Serial.println("[NTP_CALLBACK] Sincronização de relógio efetuada com êxito!");
    Serial.printf("[NTP_CALLBACK] Horário disciplinado: %s (UTC-3)\n", dataHoraLog);
    Serial.printf("[NTP_CALLBACK] Total de sincronizações bem-sucedidas: %lu\n", syncSuccessCount);
    Serial.println("==================================================");

    // Atualiza imediatamente a string padrão de exportação
    updateTime();
}

// -------------------------------------------------------------------------
// 4. Inicialização do Cliente de Tempo
// -------------------------------------------------------------------------

/**
 * Configura o cliente SNTP em segundo plano, define os servidores primários
 * e secundários e registra a função de notificação de sincronismo.
 */
void setupTimeClient()
{
    Serial.println("[TEMPO] Inicializando subsistema de sincronização temporal...");

    bootMillis = millis();

    // 1. Desliga instâncias anteriores para reinicialização limpa
    if (sntp_enabled())
    {
        sntp_stop();
    }

    // 2. Configura modo de operação: Sincronização suave e contínua
    sntp_setoperatingmode(SNTP_OPMODE_POLL);

    // 3. Registra o callback de evento do ESP-IDF
    sntp_set_time_sync_notification_cb(timeSyncNotificationCallback);

    // 4. Inicia a configuração com múltiplos servidores para redundância máxima
    configTime(gmtOffsetSeconds, daylightOffsetSeconds,
               NTP_SERVER_BR_A, NTP_SERVER_BR_B, NTP_SERVER_POOL);

    Serial.printf("[TEMPO] Servidor NTP 1: %s\n", NTP_SERVER_BR_A);
    Serial.printf("[TEMPO] Servidor NTP 2: %s\n", NTP_SERVER_BR_B);
    Serial.printf("[TEMPO] Servidor NTP 3: %s\n", NTP_SERVER_POOL);
    Serial.printf("[TEMPO] Deslocamento GMT: %ld segundos (%ld horas)\n",
                  gmtOffsetSeconds, gmtOffsetSeconds / 3600);

    // Executa a primeira atualização lógica das variáveis
    updateTime();

    Serial.println("[TEMPO] Subsistema configurado. Operando em background de forma não-bloqueante.");
}

// -------------------------------------------------------------------------
// 5. Funções de Atualização e Leitura de Horário
// -------------------------------------------------------------------------

/**
 * Lê o relógio interno (POSIX gettimeofday/localtime_r) com timeout ultracurto (20 ms).
 * Nunca bloqueia a CPU nem os controladores dos compressores.
 */
void updateTime()
{
    time_t agora;
    time(&agora);

    // Valida se o tempo atual do sistema é plausível
    if (agora >= TIMESTAMP_MINIMO_VALIDO)
    {
        timeSyncAchieved = true;
    }

    struct tm timeinfo;
    // localtime_r é reentrante e seguro para sistemas multitarefa FreeRTOS
    if (localtime_r(&agora, &timeinfo) != nullptr)
    {
        char buffer[25];
        // Formato ISO estrito exigido pelo parser de ligadesliga.cpp: "YYYY-MM-DDTHH:MM"
        snprintf(buffer, sizeof(buffer), "%04d-%02d-%02dT%02d:%02d",
                 timeinfo.tm_year + 1900,
                 timeinfo.tm_mon + 1,
                 timeinfo.tm_mday,
                 timeinfo.tm_hour,
                 timeinfo.tm_min);

        currentTime = String(buffer);
    }
}

/**
 * Retorna a hora atual como string no formato padrão "YYYY-MM-DDTHH:MM".
 * Garante que o relógio interno seja lido a cada requisição.
 */
String getTimeClient()
{
    updateTime();
    return currentTime;
}

/**
 * Força uma nova solicitação imediata aos servidores NTP em caso de reconexão Wi-Fi.
 */
void setTimeFromNTP()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("[TEMPO] Solicitando ressincronização forçada com os servidores NTP...");
        configTime(gmtOffsetSeconds, daylightOffsetSeconds,
                   NTP_SERVER_BR_A, NTP_SERVER_BR_B, NTP_SERVER_POOL);
    }
    else
    {
        Serial.println("[TEMPO] Não é possível sincronizar NTP: Conexão Wi-Fi inativa.");
        syncFailCount++;
    }
}

/**
 * Imprime a data e hora interna no console serial para fins de depuração.
 */
void printInternalTime()
{
    time_t agora;
    time(&agora);
    struct tm timeinfo;

    if (localtime_r(&agora, &timeinfo) != nullptr)
    {
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%02d/%02d/%04d %02d:%02d:%02d",
                 timeinfo.tm_mday,
                 timeinfo.tm_mon + 1,
                 timeinfo.tm_year + 1900,
                 timeinfo.tm_hour,
                 timeinfo.tm_min,
                 timeinfo.tm_sec);

        Serial.printf("[RELÓGIO_ESP32] %s (Sincronizado: %s)\n",
                      buffer, timeSyncAchieved ? "SIM" : "NÃO");
    }
    else
    {
        Serial.println("[RELÓGIO_ESP32] Erro na conversão de localtime_r.");
    }
}

// -------------------------------------------------------------------------
// 6. Consultas de Estado e Métricas de Sincronismo
// -------------------------------------------------------------------------

/**
 * Retorna true se o relógio já foi disciplinado com sucesso pela internet.
 */
bool isTimeSynchronized()
{
    time_t agora;
    time(&agora);
    return (agora >= TIMESTAMP_MINIMO_VALIDO && timeSyncAchieved);
}

/**
 * Retorna o timestamp UNIX bruto (segundos decorridos desde 01/01/1970).
 */
unsigned long getEpochTime()
{
    time_t agora;
    time(&agora);
    return (unsigned long)agora;
}

/**
 * Retorna a hora formatada no padrão brasileiro: "DD/MM/YYYY HH:MM:SS".
 */
String getFormattedTimeBR()
{
    time_t agora;
    time(&agora);
    struct tm timeinfo;

    if (localtime_r(&agora, &timeinfo) != nullptr)
    {
        char buffer[25];
        snprintf(buffer, sizeof(buffer), "%02d/%02d/%04d %02d:%02d:%02d",
                 timeinfo.tm_mday,
                 timeinfo.tm_mon + 1,
                 timeinfo.tm_year + 1900,
                 timeinfo.tm_hour,
                 timeinfo.tm_min,
                 timeinfo.tm_sec);
        return String(buffer);
    }
    return String("00/00/0000 00:00:00");
}

/**
 * Retorna a data e hora no padrão ISO 8601 completo com fuso horário:
 * "YYYY-MM-DDTHH:MM:SS-03:00".
 */
String getISODateTime()
{
    time_t agora;
    time(&agora);
    struct tm timeinfo;

    if (localtime_r(&agora, &timeinfo) != nullptr)
    {
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%04d-%02d-%02dT%02d:%02d:%02d-03:00",
                 timeinfo.tm_year + 1900,
                 timeinfo.tm_mon + 1,
                 timeinfo.tm_mday,
                 timeinfo.tm_hour,
                 timeinfo.tm_min,
                 timeinfo.tm_sec);
        return String(buffer);
    }
    return String("2026-01-01T00:00:00-03:00");
}

// -------------------------------------------------------------------------
// 7. Funções Utilitárias de Extração de Partes do Tempo
// -------------------------------------------------------------------------

int getHoraAtual()
{
    time_t agora;
    time(&agora);
    struct tm timeinfo;
    if (localtime_r(&agora, &timeinfo) != nullptr)
    {
        return timeinfo.tm_hour;
    }
    return -1;
}

int getMinutoAtual()
{
    time_t agora;
    time(&agora);
    struct tm timeinfo;
    if (localtime_r(&agora, &timeinfo) != nullptr)
    {
        return timeinfo.tm_min;
    }
    return -1;
}

int getSegundoAtual()
{
    time_t agora;
    time(&agora);
    struct tm timeinfo;
    if (localtime_r(&agora, &timeinfo) != nullptr)
    {
        return timeinfo.tm_sec;
    }
    return -1;
}

/**
 * Retorna o dia da semana: 0 = Domingo, 1 = Segunda-feira, ..., 6 = Sábado.
 */
int getDiaSemana()
{
    time_t agora;
    time(&agora);
    struct tm timeinfo;
    if (localtime_r(&agora, &timeinfo) != nullptr)
    {
        return timeinfo.tm_wday;
    }
    return -1;
}

// -------------------------------------------------------------------------
// 8. Cálculo de Uptime (Tempo de Atividade Contínua)
// -------------------------------------------------------------------------

/**
 * Retorna os segundos totais decorridos desde a última inicialização do ESP32.
 * Trata o transbordamento (rollover) do contador de 32 bits de millis() a cada 49 dias.
 */
unsigned long getUptimeSeconds()
{
    static unsigned long lastMillis = 0;
    static unsigned long overflowCount = 0;
    unsigned long currentMillis = millis();

    if (currentMillis < lastMillis)
    {
        overflowCount++;
    }
    lastMillis = currentMillis;

    // Cálculo exato de 64 bits para evitar distorção de tempo
    uint64_t totalMillis = ((uint64_t)overflowCount << 32) + currentMillis;
    return (unsigned long)(totalMillis / 1000);
}

/**
 * Retorna o tempo de atividade formatado: "X dias, HH:MM:SS".
 */
String getUptimeFormatted()
{
    unsigned long totalSec = getUptimeSeconds();
    unsigned long dias = totalSec / 86400;
    unsigned int restoSec = totalSec % 86400;
    unsigned int horas = restoSec / 3600;
    unsigned int minutos = (restoSec % 3600) / 60;
    unsigned int segundos = restoSec % 60;

    char buffer[32];
    if (dias > 0)
    {
        snprintf(buffer, sizeof(buffer), "%lud %02uh %02um %02us", dias, horas, minutos, segundos);
    }
    else
    {
        snprintf(buffer, sizeof(buffer), "%02uh %02um %02us", horas, minutos, segundos);
    }
    return String(buffer);
}

// -------------------------------------------------------------------------
// 9. Ajuste Manual de Emergência do RTC (Operação Sem Internet)
// -------------------------------------------------------------------------

/**
 * Permite ajustar o RTC manualmente caso o compressor opere em local sem Wi-Fi.
 */
bool setManualDateTime(int ano, int mes, int dia, int hora, int minuto, int segundo)
{
    // Validação dos parâmetros cronológicos
    if (ano < 2024 || ano > 2099 || mes < 1 || mes > 12 || dia < 1 || dia > 31 ||
        hora < 0 || hora > 23 || minuto < 0 || minuto > 59 || segundo < 0 || segundo > 59)
    {
        Serial.println("[TEMPO] Parâmetros de ajuste manual de relógio fora do intervalo aceitável.");
        return false;
    }

    struct tm timeinfo;
    memset(&timeinfo, 0, sizeof(struct tm));

    timeinfo.tm_year = ano - 1900;
    timeinfo.tm_mon = mes - 1;
    timeinfo.tm_mday = dia;
    timeinfo.tm_hour = hora;
    timeinfo.tm_min = minuto;
    timeinfo.tm_sec = segundo;
    timeinfo.tm_isdst = 0;

    time_t epochManual = mktime(&timeinfo);
    if (epochManual == (time_t)-1)
    {
        Serial.println("[TEMPO] Falha na conversão mktime durante ajuste manual.");
        return false;
    }

    struct timeval tv;
    tv.tv_sec = epochManual;
    tv.tv_usec = 0;

    // Aplica o tempo ao relógio interno do sistema
    settimeofday(&tv, nullptr);
    timeSyncAchieved = true;
    updateTime();

    Serial.println("==================================================");
    Serial.println("[TEMPO] Relógio do ESP32 ajustado manualmente com êxito!");
    Serial.printf("[TEMPO] Novo horário aplicado: %02d/%02d/%04d %02d:%02d:%02d\n",
                  dia, mes, ano, hora, minuto, segundo);
    Serial.println("==================================================");

    return true;
}

// -------------------------------------------------------------------------
// 10. Relatório de Estado Temporal em JSON para o Dashboard
// -------------------------------------------------------------------------

/**
 * Gera um objeto JSON completo com todas as variáveis de data, hora e diagnóstico.
 */
String getDateTimeJSON()
{
    time_t agora;
    time(&agora);
    struct tm t;
    bool ok = (localtime_r(&agora, &t) != nullptr);

    String json = "{";
    json += "\"sincronizado\":" + String(isTimeSynchronized() ? "true" : "false") + ",";
    json += "\"epoch\":" + String((unsigned long)agora) + ",";
    json += "\"iso\":\"" + getISODateTime() + "\",";
    json += "\"formatado_br\":\"" + getFormattedTimeBR() + "\",";
    json += "\"padrao_sistema\":\"" + getTimeClient() + "\",";
    json += "\"uptime_segundos\":" + String(getUptimeSeconds()) + ",";
    json += "\"uptime\":\"" + getUptimeFormatted() + "\",";
    json += "\"hora\":" + String(ok ? t.tm_hour : 0) + ",";
    json += "\"minuto\":" + String(ok ? t.tm_min : 0) + ",";
    json += "\"segundo\":" + String(ok ? t.tm_sec : 0) + ",";
    json += "\"dia_semana\":" + String(ok ? t.tm_wday : 0) + ",";
    json += "\"sucessos_ntp\":" + String(syncSuccessCount) + ",";
    json += "\"falhas_ntp\":" + String(syncFailCount) + "";
    json += "}";

    return json;
}