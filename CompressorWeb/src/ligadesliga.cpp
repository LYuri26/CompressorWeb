#include <FS.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include "autenticador.h"
#include "ligadesliga.h"
#include "tempo.h"
#include "manutencao.h"

// Pinos de controle de motores e pinos adicionais para status
int pinosMotores[] = {13, 27, 33}; // Para gerar pulsos rápidos
int pinosStatus[] = {14, 26, 32};  // Para indicar estado do compressor

const long intervalo = 1000; // Intervalo para gravação de estado
const String arquivosMotores[] = {"/motor1.txt", "/motor2.txt", "/motor3.txt"};
const String arquivosStatus[] = {"/status1.txt", "/status2.txt", "/status3.txt"};
const String arquivosTimers[] = {"/timer1.txt", "/timer2.txt", "/timer3.txt"};

bool motoresLigados[] = {false, false, false};
bool timersAtivos[] = {false, false, false};
bool motoresEstadoAnterior[3] = {false, false, false}; // Defina a variável aqui
unsigned long previousMillis[] = {0, 0, 0};
unsigned long lastToggleTime[] = {0, 0, 0};
const int NUM_MOTORS = sizeof(pinosMotores) / sizeof(pinosMotores[0]); // Número de motores
const int *motorPins = pinosMotores;                                   // Aponta para o array de pinos dos motores
unsigned long motorTimers[] = {0, 0, 0};                               // Armazena o tempo de espera para cada motor
const unsigned long LIGAR_TIMEOUT = 60;                                // 60 segundos (1 minuto)
const unsigned long DESLIGAR_TIMEOUT = 10;                             // 10 segundos

// -------------------------------------------------------------------------
// Função para extrair as horas de uma string de horário
// -------------------------------------------------------------------------
int getHoursFromTime(const String &time)
{
    return time.substring(11, 13).toInt(); // Extrai e retorna as horas da string de horário
}

// -------------------------------------------------------------------------
// Função para extrair os minutos de uma string de horário
// -------------------------------------------------------------------------
int getMinutesFromTime(const String &time)
{
    return time.substring(14, 16).toInt(); // Extrai e retorna os minutos da string de horário
}

// -------------------------------------------------------------------------
// Função para verificar se o horário atual é após o horário de fechamento
// -------------------------------------------------------------------------
bool isAfterClosingTime()
{
    updateTime();                                      // Atualiza o horário atual
    int hour = getHoursFromTime(getTimeClient());      // Obtém a hora atual
    int minute = getMinutesFromTime(getTimeClient());  // Obtém os minutos atuais
    return (hour > 22) || (hour == 22 && minute >= 0); // Retorna verdadeiro se for após 22:00
}

// -------------------------------------------------------------------------
// Função para verificar se o horário atual é antes do horário de abertura
// -------------------------------------------------------------------------
bool isBeforeOpeningTime()
{
    updateTime();                                     // Atualiza o horário atual
    int hour = getHoursFromTime(getTimeClient());     // Obtém a hora atual
    int minute = getMinutesFromTime(getTimeClient()); // Obtém os minutos atuais
    return (hour < 8) || (hour == 8 && minute < 0);   // Retorna verdadeiro se for antes de 08:00
}

// -------------------------------------------------------------------------
// Função para desligar todos os motores e bloqueá-los
// -------------------------------------------------------------------------
void desligarTodosMotores()
{
    for (int i = 0; i < 3; i++)
    {
        digitalWrite(pinosMotores[i], HIGH);
        delay(500); // Mantém o pulso por 500ms
        digitalWrite(pinosMotores[i], LOW);
        motoresLigados[i] = false;
        saveMotorState(arquivosMotores[i], false);
    }
}

// -------------------------------------------------------------------------
// Função para verificar se o sistema deve estar bloqueado (fora do horário de funcionamento ou em manutenção)
// -------------------------------------------------------------------------
bool sistemaDeveEstarBloqueado()
{
    return sistemaEmManutencao || isAfterClosingTime() || isBeforeOpeningTime();
}

// -------------------------------------------------------------------------
// Função para desativar o sistema fora do horário permitido
// -------------------------------------------------------------------------
void desativarSistemaForaDoHorario()
{
    if (isAfterClosingTime() || isBeforeOpeningTime())
    {
        desligarTodosMotores();
        Serial.println("Sistema desativado fora do horário permitido (08:00 às 22:00).");
    }
}

// -------------------------------------------------------------------------
// Função para atualizar o estado dos motores com base no horário e estado de manutenção
// -------------------------------------------------------------------------
void atualizarEstadoMotores()
{
    if (sistemaDeveEstarBloqueado())
    {
        desligarTodosMotores();
    }
}

unsigned long readTimerState(const String &arquivoTimer)
{
    File file = SPIFFS.open(arquivoTimer, "r");
    if (!file)
    {
        Serial.println("Arquivo de timer não encontrado: " + arquivoTimer + ". Assumindo tempo restante 0.");
        return 0;
    }

    String remainingTime = file.readStringUntil('\n');
    file.close();
    return remainingTime.toInt();
}

void saveTimerState(const String &arquivoTimer, unsigned long remainingTime)
{
    File file = SPIFFS.open(arquivoTimer, "w");
    if (!file)
    {
        Serial.println("Erro ao abrir o arquivo para escrita: " + arquivoTimer);
        return;
    }

    file.println(remainingTime);
    file.close();
    Serial.println("Tempo restante salvo no arquivo " + arquivoTimer + ": " + String(remainingTime));
}

// -------------------------------------------------------------------------
// Função para configurar a página de login e o controle dos motores
// -------------------------------------------------------------------------
void handleToggleAction(AsyncWebServer &server)
{
    server.on("/toggle", HTTP_ANY, [](AsyncWebServerRequest *request)
              {
        if (!isAuthenticated(request)) {
            redirectToAccessDenied(request);
            return;
        }

        if (sistemaDeveEstarBloqueado()) {
            request->send(200, "text/plain", "Sistema bloqueado. Operação não permitida.");
            return;
        }

        int motorIdx = request->getParam("motor")->value().toInt() - 1;
        String action = request->getParam("action")->value();

        if (motorIdx < 0 || motorIdx > 2) {
            request->send(400, "text/plain", "Motor inválido!");
            return;
        }

        // Verifica se o cronômetro está ativo
        if (isMotorTimerActive(motorIdx)) {
            request->send(200, "text/plain", "Aguarde o término do cronômetro para realizar esta ação.");
            return;
        }

        // Exibe no console qual botão foi pressionado e qual ação foi solicitada
        if (motorIdx == 0) {
            Serial.println("BOTÃO MOTOR COMPRESSOR MANDOU COMANDO PARA " + String(action == "ligar" ? "LIGAR" : "DESLIGAR"));
        } else if (motorIdx == 1) {
            Serial.println("BOTÃO MOTOR VENTILADOR MANDOU COMANDO PARA " + String(action == "ligar" ? "LIGAR" : "DESLIGAR"));
        } else if (motorIdx == 2) {
            Serial.println("BOTÃO MOTOR SECADOR MANDOU COMANDO PARA " + String(action == "ligar" ? "LIGAR" : "DESLIGAR"));
        }

        if (action == "ligar") {
            if (isAfterClosingTime() || isBeforeOpeningTime()) {
                request->send(200, "text/plain", "Não é permitido ligar o motor fora do horário de funcionamento.");
                return;
            }

            // Envia um pulso rápido
            digitalWrite(pinosMotores[motorIdx], HIGH);
            delay(500); // Mantém o pulso por 500ms
            digitalWrite(pinosMotores[motorIdx], LOW);

            // Verifica o status do motor após o pulso
            if (digitalRead(pinosStatus[motorIdx]) == HIGH) {
                motoresLigados[motorIdx] = true;
                motorTimers[motorIdx] = millis() + (LIGAR_TIMEOUT * 1000); // Inicia o cronômetro de 60 segundos
                saveTimerState(arquivosTimers[motorIdx], LIGAR_TIMEOUT * 1000); // Salva o tempo restante
                request->send(200, "text/plain", "Motor " + String(motorIdx + 1) + " ligado!");
            } else {
                request->send(400, "text/plain", "Falha ao ligar o motor.");
            }
        } else if (action == "desligar") {
            // Envia um pulso rápido
            digitalWrite(pinosMotores[motorIdx], HIGH);
            delay(500); // Mantém o pulso por 500ms
            digitalWrite(pinosMotores[motorIdx], LOW);

            motoresLigados[motorIdx] = false;
            motorTimers[motorIdx] = millis() + (DESLIGAR_TIMEOUT * 1000); // Inicia o cronômetro de 10 segundos
            saveTimerState(arquivosTimers[motorIdx], DESLIGAR_TIMEOUT * 1000); // Salva o tempo restante
            request->send(200, "text/plain", "Motor " + String(motorIdx + 1) + " desligado!");
        } else {
            request->send(400, "text/plain", "Ação inválida!");
        }

        // Atualizar estado imediatamente após a ação
        saveMotorState(arquivosMotores[motorIdx], motoresLigados[motorIdx]); });
}

// -------------------------------------------------------------------------
// Função para monitorar o status dos compressores
// -------------------------------------------------------------------------
void monitorarStatusCompressores()
{
    static bool ultimoEstadoMotores[3] = {false, false, false}; // Armazena o último estado dos motores

    for (int i = 0; i < 3; i++)
    {
        bool estadoAtual = digitalRead(pinosStatus[i]) == HIGH; // Lê o status atual do compressor

        // Verifica se o estado mudou desde a última verificação
        if (estadoAtual != ultimoEstadoMotores[i])
        {
            ultimoEstadoMotores[i] = estadoAtual; // Atualiza o último estado
            motoresLigados[i] = estadoAtual;      // Atualiza o estado do motor

            // Salva o estado no arquivo correspondente
            saveStatusState(pinosStatus[i], estadoAtual);

            // Exibe a atualização no console
            Serial.println("Compressor " + String(i + 1) + " " + (estadoAtual ? "ligado" : "desligado"));
        }
    }
}

bool isMotorTimerActive(int motorIdx)
{
    unsigned long remainingTime = readTimerState(arquivosTimers[motorIdx]);
    if (remainingTime > 0)
    {
        motorTimers[motorIdx] = millis() + remainingTime; // Restaura o tempo restante
        return true;
    }
    return false;
}

void updateTimers()
{
    for (int i = 0; i < NUM_MOTORS; i++)
    {
        if (motorTimers[i] > millis())
        {
            unsigned long remainingTime = motorTimers[i] - millis();
            saveTimerState(arquivosTimers[i], remainingTime); // Atualiza o tempo restante no SPIFFS
        }
        else
        {
            saveTimerState(arquivosTimers[i], 0); // Zera o tempo restante no SPIFFS
        }
    }
}

// -------------------------------------------------------------------------
// Função para inicializar o SPIFFS
// -------------------------------------------------------------------------
void initSPIFFS()
{
    if (!SPIFFS.begin(true))
    {
        Serial.println("Erro ao iniciar SPIFFS.");
    }
    else
    {
        Serial.println("SPIFFS inicializado.");
    }
}

// -------------------------------------------------------------------------
// Função para ler o estado do motor a partir de um arquivo SPIFFS
// -------------------------------------------------------------------------
bool readMotorState(const String &arquivoEstado)
{
    File file = SPIFFS.open(arquivoEstado, "r");
    if (!file)
    {
        Serial.println("Arquivo de estado não encontrado: " + arquivoEstado + ". Assumindo estado desligado.");
        return false;
    }

    String state = file.readStringUntil('\n');
    file.close();
    Serial.println("Estado lido do arquivo " + arquivoEstado + ": " + state);
    return state.toInt() == 1;
}

// -------------------------------------------------------------------------
// Função para salvar o estado do motor em um arquivo SPIFFS
// -------------------------------------------------------------------------
void saveMotorState(const String &arquivoEstado, bool state)
{
    File file = SPIFFS.open(arquivoEstado, "w");
    if (!file)
    {
        Serial.println("Erro ao abrir o arquivo para escrita: " + arquivoEstado);
        return;
    }

    file.println(state ? "1" : "0");
    file.close();
    Serial.println("Estado salvo no arquivo " + arquivoEstado + ": " + (state ? "Ligado" : "Desligado"));
}

// -------------------------------------------------------------------------
// Função para salvar o estado dos pinos de status
// -------------------------------------------------------------------------
void saveStatusState(int pin, bool state)
{
    String arquivoEstado;
    if (pin == 32)
        arquivoEstado = arquivosStatus[0];
    else if (pin == 33)
        arquivoEstado = arquivosStatus[1];
    else if (pin == 35)
        arquivoEstado = arquivosStatus[2];

    saveMotorState(arquivoEstado, state);
}

// -------------------------------------------------------------------------
// Função para configurar o sistema de ligar/desligar motores
// -------------------------------------------------------------------------
void setupLigaDesliga(AsyncWebServer &server)
{
    initSPIFFS();

    // Configura os pinos de controle dos motores
    for (int i = 0; i < 3; i++)
    {
        pinMode(pinosMotores[i], OUTPUT);
        digitalWrite(pinosMotores[i], LOW); // Garante que os motores comecem desligados
    }

    // Configura os pinos de status para os compressores
    for (int i = 0; i < 3; i++)
    {
        pinMode(pinosStatus[i], INPUT); // Esses pinos irão ler o status do compressor
    }

    handleToggleAction(server);
    setupTimeClient();

    // Atualiza o estado dos motores no início
    atualizarEstadoMotores();
}

bool checkMotorStatusChange()
{
    static bool lastMotorState[NUM_MOTORS]; // Armazena o último estado conhecido dos motores
    bool currentMotorState[NUM_MOTORS];

    // Lê o estado atual dos motores
    for (int i = 0; i < NUM_MOTORS; i++)
    {
        currentMotorState[i] = digitalRead(motorPins[i]);
    }

    // Compara com o último estado conhecido
    for (int i = 0; i < NUM_MOTORS; i++)
    {
        if (currentMotorState[i] != lastMotorState[i])
        {
            // Atualiza o último estado conhecido
            memcpy(lastMotorState, currentMotorState, sizeof(currentMotorState));
            return true; // Houve mudança
        }
    }

    return false; // Nenhuma mudança
}

// -------------------------------------------------------------------------
// Função para atualizar o estado dos motores
// -------------------------------------------------------------------------
void updateMotorStatus()
{
    Serial.println("Atualizando status dos motores...");

    for (int i = 0; i < 3; i++)
    {
        bool motorLigado = digitalRead(pinosStatus[i]);               // Lê o status real do motor
        bool estadoMotorArquivo = readMotorState(arquivosMotores[i]); // Lê o estado do motor no arquivo

        // Lógica para ligar/desligar o motor
        if ((estadoMotorArquivo == 1 && motorLigado == 1) || (estadoMotorArquivo == 0 && motorLigado == 1))
        {
            // Motor deve ser ligado
            digitalWrite(pinosMotores[i], HIGH);
            delay(500); // Mantém o pulso por 500ms
            digitalWrite(pinosMotores[i], LOW);
            motoresLigados[i] = true;

            // Se motor(NUMERO).txt == 0 e status(NUMERO).txt == 1, atualiza o arquivo para 1
            if (estadoMotorArquivo == 0 && motorLigado == 1)
            {
                saveMotorState(arquivosMotores[i], true); // Atualiza o arquivo para 1
            }
        }
        else if ((estadoMotorArquivo == 1 && motorLigado == 0) || (estadoMotorArquivo == 0 && motorLigado == 0))
        {
            // Motor deve ser desligado
            digitalWrite(pinosMotores[i], HIGH);
            delay(500); // Mantém o pulso por 500ms
            digitalWrite(pinosMotores[i], LOW);
            motoresLigados[i] = false;
        }

        // Salva o estado no arquivo correspondente
        saveMotorState(arquivosMotores[i], motoresLigados[i]);
        saveStatusState(pinosStatus[i], motorLigado);

        Serial.println("Status do Motor " + String(i + 1) + " atualizado: " + (motoresLigados[i] ? "ON" : "OFF"));
    }
}