/**
 * ============================================================================
 * PROJETO: EnergizAR - Sistema de Automação e Monitoramento ESP32
 * ARQUIVO: src/pressostato.cpp
 * DESCRIÇÃO: Módulo de monitoramento e intertravamento do pressostato de
 *            segurança da linha de gás/óleo do compressor (Motor 1).
 *            Implementa filtro de ruído (debounce), desligamento assíncrono
 *            e prevenção contra escritas repetitivas na memória Flash.
 * ============================================================================
 */

#include "pressostato.h"
#include "ligadesliga.h"

// -------------------------------------------------------------------------
// 1. Definições de Hardware e Pinos
// -------------------------------------------------------------------------

/**
 * Pino digital de entrada conectado ao contato do pressostato mecânico.
 * Pino: GPIO 17 (G17)
 */
const int pinoPressostato = 17;

// Flag global indicando se o alarme do pressostato está ativo
bool pressostatoAtivo = false;

// -------------------------------------------------------------------------
// 2. Variáveis de Temporização e Filtragem Digital (Debounce)
// -------------------------------------------------------------------------

// Variáveis de controle de debounce contra repiques da chave eletromecânica
static bool ultimoEstadoLeitura = false;
static unsigned long tempoFiltroDebounce = 0;
const unsigned long DEBOUNCE_PRESSOSTATO_MS = 200; // Janela estável de 200 ms

// Controle para registro de logs periódicos sem poluir o monitor serial
static unsigned long ultimoLogDiagnostico = 0;
const unsigned long INTERVALO_LOG_MS = 15000; // Log de rotina a cada 15 segundos

// -------------------------------------------------------------------------
// 3. Inicialização do Módulo de Pressão
// -------------------------------------------------------------------------

/**
 * Configura o pino GPIO do pressostato em modo INPUT_PULLDOWN.
 * Garante nível lógico LOW quando os contatos da chave estiverem abertos.
 */
void setupPressostato()
{
    Serial.println("[PRESSOSTATO] Inicializando monitoramento do sensor de pressão...");

    pinMode(pinoPressostato, INPUT_PULLDOWN);

    // Faz uma leitura inicial para determinar o estado de partida
    bool leituraInicial = (digitalRead(pinoPressostato) == HIGH);
    pressostatoAtivo = leituraInicial;
    ultimoEstadoLeitura = leituraInicial;
    tempoFiltroDebounce = millis();

    if (pressostatoAtivo)
    {
        Serial.println("[PRESSOSTATO] ALERTA: Pressostato já se encontra acionado na inicialização!");
    }
    else
    {
        Serial.println("[PRESSOSTATO] Pressão em faixa operacional segura (NORMAL).");
    }

    Serial.println("[PRESSOSTATO] Módulo configurado com sucesso no pino GPIO 17.");
}

// -------------------------------------------------------------------------
// 4. Registro de Eventos e Diagnóstico
// -------------------------------------------------------------------------

/**
 * Registra o diagnóstico de acionamento do pressostato com detalhes operacionais.
 */
static void registrarEventoPressostato(bool ativo, bool compressorEstavaLigado)
{
    Serial.println("==================================================");
    if (ativo)
    {
        Serial.println("[PRESSOSTATO] ALERTA CRÍTICO: Pressostato de segurança acionado!");
        Serial.printf("[PRESSOSTATO] Pino GPIO %d em nível HIGH.\n", pinoPressostato);
        if (compressorEstavaLigado)
        {
            Serial.println("[PRESSOSTATO] AÇÃO: Compressor estava em operação e foi desarmado.");
        }
        else
        {
            Serial.println("[PRESSOSTATO] AÇÃO: Compressor já estava desligado. Partida bloqueada.");
        }
    }
    else
    {
        Serial.println("[PRESSOSTATO] INFORMAÇÃO: Alarme do pressostato normalizado (LOW).");
        Serial.println("[PRESSOSTATO] A linha de pressão retornou aos parâmetros de trabalho.");
    }
    Serial.println("==================================================");
}

// -------------------------------------------------------------------------
// 5. Varredura e Intertravamento do Compressor
// -------------------------------------------------------------------------

/**
 * Realiza a leitura com filtro digital e intertravamento de proteção.
 * Se o pressostato acusar anomalia de pressão, o Compressor (Motor 1)
 * é imediatamente desligado de forma não-bloqueante.
 */
void verificarPressostato()
{
    unsigned long agora = millis();

    // 1. Leitura do estado elétrico atual da porta
    bool leituraAtual = (digitalRead(pinoPressostato) == HIGH);

    // 2. Filtro de Debounce para eliminar transientes mecânicos
    if (leituraAtual != ultimoEstadoLeitura)
    {
        tempoFiltroDebounce = agora;
        ultimoEstadoLeitura = leituraAtual;
    }

    // 3. Validação após sinal permanecer estável pelo tempo estipulado
    if ((agora - tempoFiltroDebounce) > DEBOUNCE_PRESSOSTATO_MS)
    {
        // Verifica se houve mudança real confirmada no estado do pressostato
        if (leituraAtual != pressostatoAtivo)
        {
            pressostatoAtivo = leituraAtual;
            bool compressorLigado = motoresLigados[0] || (digitalRead(pinosStatus[0]) == HIGH);

            registrarEventoPressostato(pressostatoAtivo, compressorLigado);

            // Ação de proteção imediata caso o pressostato tenha entrado em alarme
            if (pressostatoAtivo)
            {
                // Se o compressor estiver acionado, comuta para desligado via pulso
                if (motoresLigados[0])
                {
                    dispararPulsoMotor(0); // Pulso não-bloqueante de 500 ms
                    motoresLigados[0] = false;
                }

                // Atualiza o estado salvo na Flash apenas na transição de estado
                saveMotorState(arquivosMotores[0], false);
                saveStatusState(pinosStatus[0], false);
            }
        }
    }

    // 4. Trava contínua de segurança (Interlock ativo)
    // Se o pressostato estiver em falha, garante que o compressor não opere
    if (pressostatoAtivo && motoresLigados[0])
    {
        dispararPulsoMotor(0);
        motoresLigados[0] = false;
        saveMotorState(arquivosMotores[0], false);
        Serial.println("[PRESSOSTATO] Intertravamento acionado: Tentativa de ligar com pressostato ativo rejeitada.");
    }

    // 5. Log de diagnóstico periódico de rotina
    if (agora - ultimoLogDiagnostico >= INTERVALO_LOG_MS)
    {
        ultimoLogDiagnostico = agora;
        bool statusCompressor = (digitalRead(pinosStatus[0]) == HIGH);

        // Se houver incoerência entre leitura do pressostato e status do compressor
        if (pressostatoAtivo && statusCompressor)
        {
            Serial.println("[PRESSOSTATO] Incoerência detectada: Pressostato ativo mas retorno do compressor está HIGH.");
            dispararPulsoMotor(0);
            motoresLigados[0] = false;
            saveMotorState(arquivosMotores[0], false);
            saveStatusState(pinosStatus[0], false);
        }
    }
}