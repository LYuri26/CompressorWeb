/**
 * ============================================================================
 * PROJETO: EnergizAR - Sistema de Automação e Monitoramento ESP32
 * ARQUIVO: src/sobrecarga.cpp
 * DESCRIÇÃO: Monitoramento das entradas dos relés térmicos, filtragem de
 * sinais, qualificação de inicialização e desligamento preventivo.
 * ============================================================================
 */

#include "sobrecarga.h"
#include "ligadesliga.h"

// -------------------------------------------------------------------------
// 1. Mapeamento dos pinos
// -------------------------------------------------------------------------

// Mantém a ligação física existente:
// Motor 1 - Compressor: GPIO 15
// Motor 2 - Ventilador: GPIO 25
// Motor 3 - Secador:    GPIO 18

const int pinosSobrecarga[NUM_MOTORES] = {15, 25, 18};

// -------------------------------------------------------------------------
// 2. Estados e temporizações
// -------------------------------------------------------------------------

bool sobrecargaDetectada[NUM_MOTORES] = {false, false, false};

static bool ultimoEstadoSobrecarga[NUM_MOTORES] = {
    false, false, false};

static unsigned long ultimoTempoSobrecarga[NUM_MOTORES] = {
    0, 0, 0};

// Indica se a entrada já apresentou LOW estável após a inicialização.
// Uma entrada HIGH no boot não será classificada automaticamente
// como sobrecarga, mas também não será considerada validada.
static bool entradaSobrecargaArmada[NUM_MOTORES] = {
    false, false, false};

static bool avisoEntradaInvalidaEmitido[NUM_MOTORES] = {
    false, false, false};

static unsigned long tempoInicioNivelBaixo[NUM_MOTORES] = {
    0, 0, 0};

unsigned long tempoInicioSobrecarga[NUM_MOTORES] = {
    0, 0, 0};

const unsigned long DEBOUNCE_DELAY = 200;
const unsigned long CARENCIA_BOOT_MS = 3000;
const unsigned long TEMPO_VALIDACAO_ENTRADA_MS = 1000;

// -------------------------------------------------------------------------
// 3. Inicialização
// -------------------------------------------------------------------------

void setupSobrecarga()
{
    Serial.println(
        "[SOBRECARGA] Configurando entradas de proteção térmica...");

    unsigned long agora = millis();

    for (int i = 0; i < NUM_MOTORES; i++)
    {
        pinMode(pinosSobrecarga[i], INPUT_PULLDOWN);

        sobrecargaDetectada[i] = false;
        ultimoEstadoSobrecarga[i] = false;

        ultimoTempoSobrecarga[i] = agora;
        tempoInicioSobrecarga[i] = 0;

        entradaSobrecargaArmada[i] = false;
        avisoEntradaInvalidaEmitido[i] = false;
        tempoInicioNivelBaixo[i] = 0;

        Serial.printf(
            "[SOBRECARGA] Motor %d configurado no GPIO %d.\n",
            i + 1,
            pinosSobrecarga[i]);
    }

    Serial.println(
        "[SOBRECARGA] Entradas aguardando validacao inicial.");
}

// -------------------------------------------------------------------------
// 4. Desativação dos motores
// -------------------------------------------------------------------------

void desativarMotor(int motorIndex)
{
    if (motorIndex < 0 || motorIndex >= NUM_MOTORES)
    {
        return;
    }

    digitalWrite(pinosMotores[motorIndex], LOW);

    if (motoresLigados[motorIndex])
    {
        motoresLigados[motorIndex] = false;

        saveMotorState(
            arquivosMotores[motorIndex],
            false);

        Serial.printf(
            "[SEGURANCA] Motor %d desligado.\n",
            motorIndex + 1);
    }
}

void desativarMotores()
{
    Serial.println(
        "[SEGURANCA] Desligando todos os motores.");

    for (int i = 0; i < NUM_MOTORES; i++)
    {
        desativarMotor(i);
    }
}

void desativarMotoresSobrecarga()
{
    for (int i = 0; i < NUM_MOTORES; i++)
    {
        if (sobrecargaDetectada[i])
        {
            desativarMotor(i);
        }
    }
}

void restaurarEstadoMotores()
{
    Serial.println(
        "[SOBRECARGA] Preparando motores para rearme seguro.");

    for (int i = 0; i < NUM_MOTORES; i++)
    {
        digitalWrite(pinosMotores[i], LOW);
        motoresLigados[i] = false;

        sobrecargaDetectada[i] = false;

        saveMotorState(
            arquivosMotores[i],
            false);

        Serial.printf(
            "[SOBRECARGA] Motor %d permanece desligado.\n",
            i + 1);
    }
}

// -------------------------------------------------------------------------
// 5. Monitoramento das entradas
// -------------------------------------------------------------------------

void monitorarSobrecarga()
{
    unsigned long agora = millis();

    // Aguarda a estabilização inicial do ESP32.
    if (agora < CARENCIA_BOOT_MS)
    {
        return;
    }

    for (int i = 0; i < NUM_MOTORES; i++)
    {
        bool leituraFisica =
            (digitalRead(pinosSobrecarga[i]) == HIGH);

        // -------------------------------------------------------------
        // ETAPA A: validação inicial da entrada
        // -------------------------------------------------------------

        if (!entradaSobrecargaArmada[i])
        {
            // A entrada precisa permanecer LOW por um período contínuo
            // antes de ser habilitada para detectar novas transições.
            if (!leituraFisica)
            {
                if (tempoInicioNivelBaixo[i] == 0)
                {
                    tempoInicioNivelBaixo[i] = agora;
                }

                if (
                    agora - tempoInicioNivelBaixo[i] >= TEMPO_VALIDACAO_ENTRADA_MS)
                {
                    entradaSobrecargaArmada[i] = true;

                    ultimoEstadoSobrecarga[i] = false;
                    ultimoTempoSobrecarga[i] = agora;

                    avisoEntradaInvalidaEmitido[i] = false;

                    Serial.printf(
                        "[SOBRECARGA] Entrada do motor %d validada "
                        "em LOW. Monitoramento habilitado.\n",
                        i + 1);
                }
            }
            else
            {
                tempoInicioNivelBaixo[i] = 0;

                if (!avisoEntradaInvalidaEmitido[i])
                {
                    Serial.printf(
                        "[AVISO] Entrada do motor %d no GPIO %d "
                        "permanece HIGH durante a validacao. "
                        "Estado eletrico ainda nao validado.\n",
                        i + 1,
                        pinosSobrecarga[i]);

                    avisoEntradaInvalidaEmitido[i] = true;
                }

                // Estado desconhecido: não permitir acionamento.
                if (motoresLigados[i])
                {
                    desativarMotor(i);
                }
            }

            // Não classifica a leitura inicial como uma nova
            // sobrecarga antes de validar a entrada.
            continue;
        }

        // -------------------------------------------------------------
        // ETAPA B: filtragem temporal após a validação
        // -------------------------------------------------------------

        if (leituraFisica != ultimoEstadoSobrecarga[i])
        {
            ultimoEstadoSobrecarga[i] = leituraFisica;
            ultimoTempoSobrecarga[i] = agora;
        }

        if (
            agora - ultimoTempoSobrecarga[i] >= DEBOUNCE_DELAY)
        {
            if (
                leituraFisica != sobrecargaDetectada[i])
            {
                sobrecargaDetectada[i] = leituraFisica;

                if (sobrecargaDetectada[i])
                {
                    tempoInicioSobrecarga[i] = agora;

                    Serial.println(
                        "==================================================");

                    Serial.printf(
                        "[ALERTA CRITICO] SOBRECARGA CONFIRMADA "
                        "NO MOTOR %d!\n",
                        i + 1);

                    Serial.printf(
                        "[ALERTA CRITICO] GPIO %d em HIGH estavel.\n",
                        pinosSobrecarga[i]);

                    Serial.println(
                        "==================================================");

                    desativarMotor(i);
                }
                else
                {
                    tempoInicioSobrecarga[i] = 0;

                    Serial.printf(
                        "[SOBRECARGA] Sinal do motor %d retornou "
                        "para LOW. Falha eletrica liberada.\n",
                        i + 1);
                }
            }
        }

        // Intertravamento contínuo de segurança.
        if (
            sobrecargaDetectada[i] &&
            motoresLigados[i])
        {
            desativarMotor(i);
        }
    }
}
