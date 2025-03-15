#include "sobrecarga.h"
#include "ligadesliga.h"

// Pinos de entrada para monitorar a sobrecarga
const int pinosSobrecarga[] = {16, 17, 18};
bool sobrecargaDetectada[] = {false, false, false}; // Estado de sobrecarga para cada motor
unsigned long tempoInicioSobrecarga[] = {0, 0, 0};  // Armazena o tempo de início da sobrecarga
const unsigned long tempoMinimoSobrecarga = 2000;   // Tempo mínimo de sobrecarga para desativar os motores (2 segundos)

void setupSobrecarga()
{
    // Inicializa os pinos de entrada para sobrecarga
    for (int i = 0; i < 3; i++)
    {
        pinMode(pinosSobrecarga[i], INPUT);
    }

    // Inicializa o SPIFFS
    if (!SPIFFS.begin(true))
    {
        Serial.println("Erro ao iniciar SPIFFS.");
    }
    else
    {
        Serial.println("SPIFFS inicializado.");
    }

    // Restaura o estado dos motores
    restaurarEstadoMotores();
}

void desativarMotor(int motorIndex)
{
    // Salva o estado atual do motor antes de desativá-lo
    motoresEstadoAnterior[motorIndex] = readMotorState(arquivosMotores[motorIndex]);

    digitalWrite(pinosMotores[motorIndex], LOW);        // Desativa o pino do motor
    saveMotorState(arquivosMotores[motorIndex], false); // Atualiza o arquivo de estado do motor
    saveMotorState(arquivosStatus[motorIndex], false);  // Atualiza o arquivo de status do motor para 0 (desligado)
    sobrecargaDetectada[motorIndex] = true;             // Atualiza o estado de sobrecarga

    Serial.println("Motor " + String(motorIndex + 1) + " desativado devido à sobrecarga.");
}

void monitorarSobrecarga()
{
    static bool ultimoEstadoSobrecarga[] = {false, false, false}; // Armazena o último estado de sobrecarga

    for (int i = 0; i < 3; i++)
    {
        bool sobrecargaAtual = digitalRead(pinosSobrecarga[i]) == HIGH;

        // Verifica se houve mudança no estado de sobrecarga
        if (sobrecargaAtual != ultimoEstadoSobrecarga[i])
        {
            ultimoEstadoSobrecarga[i] = sobrecargaAtual; // Atualiza o último estado

            if (sobrecargaAtual)
            {
                // Inicia o timer de sobrecarga
                tempoInicioSobrecarga[i] = millis();
                Serial.println("Sobrecarga detectada no pino " + String(pinosSobrecarga[i]) + ". Iniciando contagem...");
            }
            else
            {
                // Se a sobrecarga foi resolvida antes do tempo mínimo, ignora
                tempoInicioSobrecarga[i] = 0;
                Serial.println("Sobrecarga resolvida no pino " + String(pinosSobrecarga[i]) + " antes do tempo mínimo.");

                // Restaura o estado do motor ao estado anterior
                if (motoresEstadoAnterior[i])
                {
                    digitalWrite(pinosMotores[i], HIGH);
                    delay(500); // Mantém o pulso por 500ms
                    digitalWrite(pinosMotores[i], LOW);
                    saveMotorState(arquivosMotores[i], true);
                    saveMotorState(arquivosStatus[i], true);
                    Serial.println("Motor " + String(i + 1) + " restaurado para o estado anterior: Ligado.");
                }
                else
                {
                    Serial.println("Motor " + String(i + 1) + " restaurado para o estado anterior: Desligado.");
                }
            }
        }

        // Verifica se a sobrecarga persiste por mais de 2 segundos
        if (sobrecargaAtual && tempoInicioSobrecarga[i] != 0 && (millis() - tempoInicioSobrecarga[i] >= tempoMinimoSobrecarga))
        {
            // Verifica se o pino de status está ativo (HIGH) e o arquivo de status é 1
            if (digitalRead(pinosStatus[i]) == HIGH && readMotorState(arquivosStatus[i]) == 1)
            {
                // Desativa apenas o motor correspondente ao pino de sobrecarga
                desativarMotor(i);
                tempoInicioSobrecarga[i] = 0; // Reseta o timer

                Serial.println("Sobrecarga persistente no pino " + String(pinosSobrecarga[i]) + ". Motor " + String(i + 1) + " desativado.");
            }
        }
    }
}

void desativarMotores()
{
    for (int i = 0; i < 3; i++)
    {
        digitalWrite(pinosMotores[i], LOW);        // Desativa o pino do motor
        saveMotorState(arquivosMotores[i], false); // Atualiza o arquivo de estado do motor
        saveMotorState(arquivosStatus[i], false);  // Atualiza o arquivo de status do motor para 0 (desligado)
    }
    Serial.println("Todos os motores desativados devido à sobrecarga.");
}

void restaurarEstadoMotores()
{
    for (int i = 0; i < 3; i++)
    {
        bool estadoSalvo = readMotorState(arquivosMotores[i]);
        digitalWrite(pinosMotores[i], estadoSalvo ? HIGH : LOW);
        Serial.println("Estado do motor " + String(i + 1) + " restaurado: " + (estadoSalvo ? "Ligado" : "Desligado"));
    }
}