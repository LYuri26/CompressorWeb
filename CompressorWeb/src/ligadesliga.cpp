/**
 * ============================================================================
 * PROJETO: EnergizAR - Sistema de Automação e Monitoramento ESP32
 * ARQUIVO: src/ligadesliga.cpp
 * DESCRIÇÃO: Controlador central de potência e acionamento dos compressores.
 *            Implementa geração de pulsos não-bloqueantes de 500 ms, controle
 *            de temporizadores de segurança, leitura de retorno com debounce,
 *            horímetro acumulador em memória e proteção contra desgaste da Flash.
 * ============================================================================
 */

#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>

#include "ligadesliga.h"
#include "autenticador.h"
#include "tempo.h"
#include "manutencao.h"
#include "pressostato.h"
#include "sobrecarga.h"

// -------------------------------------------------------------------------
// 1. Definições de Hardware e Pinos Globais (Ligação Externa)
// -------------------------------------------------------------------------

/**
 * Pinos de saída digital configurados para acionar as bobinas dos contatores:
 * Motor 0: Compressor Principal (GPIO 13)
 * Motor 1: Motor Ventilador     (GPIO 27)
 * Motor 2: Motor Secador        (GPIO 33)
 */
int pinosMotores[NUM_MOTORES] = {13, 27, 33};

/**
 * Pinos de entrada digital para ler o retorno auxiliar (NA) dos contatores:
 * Status 0: Retorno Compressor  (GPIO 14)
 * Status 1: Retorno Ventilador  (GPIO 26)
 * Status 2: Retorno Secador     (GPIO 32)
 */
int pinosStatus[NUM_MOTORES] = {14, 26, 32};

const int NUM_MOTORS = NUM_MOTORES;
const int *motorPins = pinosMotores;

// -------------------------------------------------------------------------
// 2. Variáveis de Estado Operacional em Memória RAM
// -------------------------------------------------------------------------

bool motoresLigados[NUM_MOTORES] = {false, false, false};
bool timersAtivos[NUM_MOTORES] = {false, false, false};
bool motoresEstadoAnterior[NUM_MOTORES] = {false, false, false};
unsigned long motorTimers[NUM_MOTORES] = {0, 0, 0};

// Arquivos de persistência com ligação externa explícita
extern const String arquivosMotores[NUM_MOTORES] = {"/motor1.txt", "/motor2.txt", "/motor3.txt"};
extern const String arquivosStatus[NUM_MOTORES] = {"/status1.txt", "/status2.txt", "/status3.txt"};
extern const String arquivosTimers[NUM_MOTORES] = {"/timer1.txt", "/timer2.txt", "/timer3.txt"};

// Constantes de intervalos temporais de segurança
const unsigned long LIGAR_TIMEOUT = 60;     // 60 segundos de intervalo após ligar
const unsigned long DESLIGAR_TIMEOUT = 10;  // 10 segundos de intervalo após desligar
const unsigned long DURACAO_PULSO_MS = 500; // Duração do pulso de relé em ms

// -------------------------------------------------------------------------
// 3. Controle Temporal de Pulsos Não-Bloqueantes
// -------------------------------------------------------------------------

static bool pulsoAtivo[NUM_MOTORES] = {false, false, false};
static unsigned long pulsoInicio[NUM_MOTORES] = {0, 0, 0};

// Filtro digital de leitura física (Debounce) nos pinos de status
static bool ultimoStatusLido[NUM_MOTORES] = {false, false, false};
static unsigned long tempoFiltroStatus[NUM_MOTORES] = {0, 0, 0};
const unsigned long DEBOUNCE_STATUS_MS = 80; // 80 ms para filtrar arco elétrico

// Cache de memória para evitar escritas destrutivas repetidas na Flash do ESP32
static int cacheEstadoMotor[NUM_MOTORES] = {-1, -1, -1};
static int cacheEstadoStatus[NUM_MOTORES] = {-1, -1, -1};
static unsigned long cacheTimers[NUM_MOTORES] = {0, 0, 0};

// Telemetria industrial: Horímetro e ciclos de acionamento
static unsigned long tempoInicioOperacao[NUM_MOTORES] = {0, 0, 0};
static unsigned long totalSegundosOperacao[NUM_MOTORES] = {0, 0, 0};
static unsigned long ciclosAcionamento[NUM_MOTORES] = {0, 0, 0};

static bool toggleRouteConfigurada = false;

// -------------------------------------------------------------------------
// 4. Funções de Manipulação Não-Bloqueante de Hardware (Pulsos)
// -------------------------------------------------------------------------

/**
 * Dispara um pulso em nível HIGH no pino correspondente sem travar o processador.
 * O desligamento (LOW) ocorre automaticamente através de processarPulsosMotores().
 */
void dispararPulsoMotor(int motorIdx)
{
    if (motorIdx < 0 || motorIdx >= NUM_MOTORES)
    {
        return;
    }

    digitalWrite(pinosMotores[motorIdx], HIGH);
    pulsoAtivo[motorIdx] = true;
    pulsoInicio[motorIdx] = millis();

    Serial.printf("[HARDWARE_PULSO] Motor %d (GPIO %d) -> HIGH iniciado.\n",
                  motorIdx + 1, pinosMotores[motorIdx]);
}

/**
 * Deve ser chamada a cada ciclo no loop() principal para verificar e desligar
 * os pinos que completaram a janela de 500 ms de pulso sem bloquear a CPU.
 */
void processarPulsosMotores()
{
    unsigned long agora = millis();

    for (int i = 0; i < NUM_MOTORES; i++)
    {
        if (pulsoAtivo[i])
        {
            if (agora - pulsoInicio[i] >= DURACAO_PULSO_MS)
            {
                digitalWrite(pinosMotores[i], LOW);
                pulsoAtivo[i] = false;

                Serial.printf("[HARDWARE_PULSO] Motor %d (GPIO %d) -> LOW finalizado (500ms).\n",
                              i + 1, pinosMotores[i]);
            }
        }
    }
}

// -------------------------------------------------------------------------
// 5. Gestão do Sistema de Arquivos (SPIFFS com Cache de Proteção de Flash)
// -------------------------------------------------------------------------

static void initSPIFFS()
{
    if (!SPIFFS.begin(true))
    {
        Serial.println("[SPIFFS] ERRO CRÍTICO: Falha ao inicializar o SPIFFS.");
    }
    else
    {
        Serial.println("[SPIFFS] Sistema de arquivos SPIFFS montado com sucesso.");
    }
}

bool readMotorState(const String &arquivoEstado)
{
    if (!SPIFFS.exists(arquivoEstado))
    {
        return false;
    }

    File file = SPIFFS.open(arquivoEstado, "r");
    if (!file)
    {
        return false;
    }

    String state = file.readStringUntil('\n');
    file.close();
    state.trim();

    return (state == "1" || state.equalsIgnoreCase("true") || state.equalsIgnoreCase("ON"));
}

void saveMotorState(const String &arquivoEstado, bool state)
{
    int idx = -1;
    for (int i = 0; i < NUM_MOTORES; i++)
    {
        if (arquivosMotores[i] == arquivoEstado)
        {
            idx = i;
            break;
        }
    }

    // Economiza ciclo de gravação se o cache já possuir este estado
    if (idx != -1 && cacheEstadoMotor[idx] == (state ? 1 : 0))
    {
        return;
    }

    File file = SPIFFS.open(arquivoEstado, "w");
    if (!file)
    {
        Serial.println("[SPIFFS] Erro ao abrir arquivo para escrita: " + arquivoEstado);
        return;
    }

    file.println(state ? "1" : "0");
    file.close();

    if (idx != -1)
    {
        cacheEstadoMotor[idx] = state ? 1 : 0;
    }

    Serial.printf("[SPIFFS] Estado persistido em %s: %s\n",
                  arquivoEstado.c_str(), state ? "Ligado (1)" : "Desligado (0)");
}

void saveStatusState(int pin, bool state)
{
    String arquivoEstado = "";
    int idx = -1;

    if (pin == pinosStatus[0])
    {
        arquivoEstado = arquivosStatus[0];
        idx = 0;
    }
    else if (pin == pinosStatus[1])
    {
        arquivoEstado = arquivosStatus[1];
        idx = 1;
    }
    else if (pin == pinosStatus[2])
    {
        arquivoEstado = arquivosStatus[2];
        idx = 2;
    }

    if (arquivoEstado == "" || idx == -1)
    {
        return;
    }

    if (cacheEstadoStatus[idx] == (state ? 1 : 0))
    {
        return; // Estado inalterado, preserva a vida útil da memória Flash
    }

    File file = SPIFFS.open(arquivoEstado, "w");
    if (!file)
    {
        return;
    }

    file.println(state ? "ON" : "OFF");
    file.close();

    cacheEstadoStatus[idx] = (state ? 1 : 0);
}

unsigned long readTimerState(const String &arquivoTimer)
{
    if (!SPIFFS.exists(arquivoTimer))
    {
        return 0;
    }

    File file = SPIFFS.open(arquivoTimer, "r");
    if (!file)
    {
        return 0;
    }

    String remainingTime = file.readStringUntil('\n');
    file.close();
    remainingTime.trim();

    return (unsigned long)remainingTime.toInt();
}

void saveTimerState(const String &arquivoTimer, unsigned long remainingTime)
{
    int idx = -1;
    for (int i = 0; i < NUM_MOTORES; i++)
    {
        if (arquivosTimers[i] == arquivoTimer)
        {
            idx = i;
            break;
        }
    }

    // Se ambos são 0, não reescreve na Flash repetidamente
    if (idx != -1 && cacheTimers[idx] == 0 && remainingTime == 0)
    {
        return;
    }

    File file = SPIFFS.open(arquivoTimer, "w");
    if (!file)
    {
        return;
    }

    file.println(remainingTime);
    file.close();

    if (idx != -1)
    {
        cacheTimers[idx] = remainingTime;
    }
}

// -------------------------------------------------------------------------
// 6. Funções de Horário e Regras Operacionais de Fábrica
// -------------------------------------------------------------------------

static int getHoursFromTime(const String &time)
{
    if (time.length() >= 13 && time.indexOf('T') != -1)
    {
        int tIndex = time.indexOf('T');
        return time.substring(tIndex + 1, tIndex + 3).toInt();
    }
    else if (time.length() >= 5 && time.indexOf(':') != -1)
    {
        return time.substring(0, time.indexOf(':')).toInt();
    }
    return -1;
}

static int getMinutesFromTime(const String &time)
{
    if (time.length() >= 16 && time.indexOf('T') != -1)
    {
        int tIndex = time.indexOf('T');
        return time.substring(tIndex + 4, tIndex + 6).toInt();
    }
    else if (time.length() >= 5 && time.indexOf(':') != -1)
    {
        int firstColon = time.indexOf(':');
        return time.substring(firstColon + 1, firstColon + 3).toInt();
    }
    return -1;
}

bool isAfterClosingTime()
{
    String curTime = getTimeClient();
    int hour = getHoursFromTime(curTime);
    int minute = getMinutesFromTime(curTime);

    if (hour == -1)
    {
        return false;
    }
    return (hour > 22) || (hour == 22 && minute >= 0);
}

bool isBeforeOpeningTime()
{
    String curTime = getTimeClient();
    int hour = getHoursFromTime(curTime);

    if (hour == -1)
    {
        return false;
    }
    return (hour < 8);
}

bool sistemaDeveEstarBloqueado()
{
    return sistemaEmManutencao || isAfterClosingTime() || isBeforeOpeningTime();
}

void desligarTodosMotores()
{
    for (int i = 0; i < NUM_MOTORES; i++)
    {
        dispararPulsoMotor(i);
        motoresLigados[i] = false;
        saveMotorState(arquivosMotores[i], false);
    }
    Serial.println("[SISTEMA_MOTORES] Todos os motores foram desligados com sucesso.");
}

void desativarSistemaForaDoHorario()
{
    if (isAfterClosingTime() || isBeforeOpeningTime())
    {
        desligarTodosMotores();
        Serial.println("[HORÁRIO] Sistema desativado: Fora do horário permitido (08:00 às 22:00).");
    }
}

void atualizarEstadoMotores()
{
    if (sistemaDeveEstarBloqueado())
    {
        desligarTodosMotores();
    }
}

// -------------------------------------------------------------------------
// 7. Gerenciamento dos Temporizadores de Descanso dos Motores
// -------------------------------------------------------------------------

bool isMotorTimerActive(int motorIdx)
{
    if (motorIdx < 0 || motorIdx >= NUM_MOTORES)
    {
        return false;
    }

    if (millis() < motorTimers[motorIdx])
    {
        return true;
    }

    if (motorTimers[motorIdx] != 0 && millis() >= motorTimers[motorIdx])
    {
        motorTimers[motorIdx] = 0;
        timersAtivos[motorIdx] = false;
        saveTimerState(arquivosTimers[motorIdx], 0);
    }

    return false;
}

unsigned long getMotorRemainingTime(int motorIdx)
{
    if (!isMotorTimerActive(motorIdx))
    {
        return 0;
    }
    return (motorTimers[motorIdx] - millis()) / 1000;
}

void updateTimers()
{
    unsigned long agora = millis();
    for (int i = 0; i < NUM_MOTORES; i++)
    {
        if (motorTimers[i] > agora)
        {
            unsigned long tempoRestante = motorTimers[i] - agora;
            saveTimerState(arquivosTimers[i], tempoRestante);
            timersAtivos[i] = true;
        }
        else
        {
            if (timersAtivos[i])
            {
                saveTimerState(arquivosTimers[i], 0);
                timersAtivos[i] = false;
                motorTimers[i] = 0;
            }
        }
    }
}

// -------------------------------------------------------------------------
// 8. Monitoramento do Feedback Real dos Compressores (Entradas com Debounce)
// -------------------------------------------------------------------------

void monitorarStatusCompressores()
{
    unsigned long agora = millis();

    for (int i = 0; i < NUM_MOTORES; i++)
    {
        bool leituraRaw = (digitalRead(pinosStatus[i]) == HIGH);

        // Filtro de Debounce contra centelhamento elétrico dos contatores
        if (leituraRaw != ultimoStatusLido[i])
        {
            tempoFiltroStatus[i] = agora;
            ultimoStatusLido[i] = leituraRaw;
        }

        if ((agora - tempoFiltroStatus[i]) > DEBOUNCE_STATUS_MS)
        {
            if (leituraRaw != motoresLigados[i])
            {
                motoresLigados[i] = leituraRaw;

                // Gestão do horímetro industrial
                if (motoresLigados[i])
                {
                    tempoInicioOperacao[i] = agora;
                    ciclosAcionamento[i]++;
                    Serial.printf("[STATUS] Contator do Motor %d FECHOU (LIGADO). Ciclos: %lu\n",
                                  i + 1, ciclosAcionamento[i]);
                }
                else
                {
                    if (tempoInicioOperacao[i] > 0)
                    {
                        totalSegundosOperacao[i] += (agora - tempoInicioOperacao[i]) / 1000;
                        tempoInicioOperacao[i] = 0;
                    }
                    Serial.printf("[STATUS] Contator do Motor %d ABRIU (DESLIGADO). Total: %lu seg.\n",
                                  i + 1, totalSegundosOperacao[i]);
                }

                saveStatusState(pinosStatus[i], motoresLigados[i]);
                saveMotorState(arquivosMotores[i], motoresLigados[i]);
            }
        }
    }
}

bool checkMotorStatusChange()
{
    static bool lastMotorState[NUM_MOTORES] = {false, false, false};
    bool currentMotorState[NUM_MOTORES];
    bool mudou = false;

    for (int i = 0; i < NUM_MOTORES; i++)
    {
        currentMotorState[i] = (digitalRead(motorPins[i]) == HIGH);
        if (currentMotorState[i] != lastMotorState[i])
        {
            mudou = true;
        }
    }

    if (mudou)
    {
        memcpy(lastMotorState, currentMotorState, sizeof(currentMotorState));
        return true;
    }

    return false;
}

void updateMotorStatus()
{
    for (int i = 0; i < NUM_MOTORES; i++)
    {
        bool feedbackFisico = (digitalRead(pinosStatus[i]) == HIGH);
        bool estadoNoArquivo = readMotorState(arquivosMotores[i]);

        if (estadoNoArquivo && !feedbackFisico)
        {
            dispararPulsoMotor(i);
            motoresLigados[i] = true;
        }
        else if (!estadoNoArquivo && feedbackFisico)
        {
            dispararPulsoMotor(i);
            motoresLigados[i] = false;
        }
        else
        {
            motoresLigados[i] = feedbackFisico;
        }

        saveMotorState(arquivosMotores[i], motoresLigados[i]);
        saveStatusState(pinosStatus[i], feedbackFisico);
    }
}

// -------------------------------------------------------------------------
// 9. Rota Assíncrona de Toggle (/toggle)
// -------------------------------------------------------------------------

void handleToggleAction(AsyncWebServer &server)
{
    if (toggleRouteConfigurada)
    {
        return;
    }
    toggleRouteConfigurada = true;

    server.on("/toggle", HTTP_ANY, [](AsyncWebServerRequest *request)
              {
        if (!isAuthenticated(request))
        {
            redirectToAccessDenied(request);
            return;
        }

        if (sistemaDeveEstarBloqueado())
        {
            request->send(200, "text/plain", "Sistema bloqueado: Manutenção ou fora do horário permitido.");
            return;
        }

        if (!request->hasParam("motor") || !request->hasParam("action"))
        {
            request->send(400, "text/plain", "Parâmetros incorretos ou ausentes.");
            return;
        }

        int motorIdx = request->getParam("motor")->value().toInt() - 1;
        String action = request->getParam("action")->value();
        action.toLowerCase();

        if (motorIdx < 0 || motorIdx >= NUM_MOTORES)
        {
            request->send(400, "text/plain", "Identificador de motor inválido!");
            return;
        }

        if (sobrecargaDetectada[motorIdx])
        {
            request->send(200, "text/plain", "Comando rejeitado: Motor em sobrecarga térmica.");
            return;
        }

        if (motorIdx == 0 && pressostatoAtivo)
        {
            request->send(200, "text/plain", "Comando rejeitado: Pressostato em condição de alarme.");
            return;
        }

        if (isMotorTimerActive(motorIdx))
        {
            request->send(200, "text/plain", "Aguarde o temporizador de segurança deste motor finalizar.");
            return;
        }

        if (action == "ligar")
        {
            if (isAfterClosingTime() || isBeforeOpeningTime())
            {
                request->send(200, "text/plain", "Não é permitido ligar o motor fora do horário de funcionamento.");
                return;
            }

            dispararPulsoMotor(motorIdx);
            motoresLigados[motorIdx] = true;

            motorTimers[motorIdx] = millis() + (LIGAR_TIMEOUT * 1000);
            timersAtivos[motorIdx] = true;
            saveTimerState(arquivosTimers[motorIdx], LIGAR_TIMEOUT * 1000);
            saveMotorState(arquivosMotores[motorIdx], true);

            Serial.printf("[WEB] Motor %d: Comando LIGAR aceito com sucesso.\n", motorIdx + 1);
            request->send(200, "text/plain", "Motor " + String(motorIdx + 1) + " ligado com sucesso!");
        }
        else if (action == "desligar")
        {
            dispararPulsoMotor(motorIdx);
            motoresLigados[motorIdx] = false;

            motorTimers[motorIdx] = millis() + (DESLIGAR_TIMEOUT * 1000);
            timersAtivos[motorIdx] = true;
            saveTimerState(arquivosTimers[motorIdx], DESLIGAR_TIMEOUT * 1000);
            saveMotorState(arquivosMotores[motorIdx], false);

            Serial.printf("[WEB] Motor %d: Comando DESLIGAR aceito com sucesso.\n", motorIdx + 1);
            request->send(200, "text/plain", "Motor " + String(motorIdx + 1) + " desligado com sucesso!");
        }
        else
        {
            request->send(400, "text/plain", "Ação solicitada inválida!");
        } });
}

// -------------------------------------------------------------------------
// 10. Inicialização Geral do Módulo
// -------------------------------------------------------------------------

void setupLigaDesliga(AsyncWebServer &server)
{
    Serial.println("[SETUP] Configurando sistema de acionamento Liga/Desliga...");

    initSPIFFS();

    for (int i = 0; i < NUM_MOTORES; i++)
    {
        pinMode(pinosMotores[i], OUTPUT);
        digitalWrite(pinosMotores[i], LOW);
        pinMode(pinosStatus[i], INPUT_PULLDOWN);
    }

    for (int i = 0; i < NUM_MOTORES; i++)
    {
        unsigned long tempoRestante = readTimerState(arquivosTimers[i]);
        if (tempoRestante > 0)
        {
            motorTimers[i] = millis() + tempoRestante;
            timersAtivos[i] = true;
            Serial.printf("[TIMER] Motor %d restaurou temporizador de %lu ms restantes.\n",
                          i + 1, tempoRestante);
        }
    }

    handleToggleAction(server);
    setupTimeClient();
    atualizarEstadoMotores();

    Serial.println("[SETUP] Módulo Liga/Desliga inicializado com sucesso.");
}