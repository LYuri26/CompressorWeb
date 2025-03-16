#include <Arduino.h>
#include "manutencao.h"
#include "ligadesliga.h"

const int pinoManutencao = 4;     // Definição da variável global
bool sistemaEmManutencao = false; // Definição da variável global

void setupManutencao()
{
    pinMode(pinoManutencao, INPUT_PULLUP); // Configura o pino do botão de manutenção
}

bool atualizarEstadoManutencao()
{
    static unsigned long lastMaintenanceCheck = 0;
    static bool lastMaintenanceState = false; // Último estado de manutenção conhecido
    unsigned long currentMillis = millis();

    // Verifica o estado do botão de manutenção a cada 500 ms
    if (currentMillis - lastMaintenanceCheck >= 500)
    {
        lastMaintenanceCheck = currentMillis;

        // Verifica o estado do pino de manutenção
        bool currentMaintenanceState = (digitalRead(pinoManutencao) == LOW);

        // Verifica se houve mudança no estado de manutenção
        if (currentMaintenanceState != lastMaintenanceState)
        {
            lastMaintenanceState = currentMaintenanceState;

            // Executa ações com base no novo estado de manutenção
            if (currentMaintenanceState)
            {
                // Entrou em modo de manutenção
                sistemaEmManutencao = true;

                // Salva o estado atual dos motores antes de desativá-los
                for (int i = 0; i < 3; i++)
                {
                    motoresEstadoAnterior[i] = motoresLigados[i];
                }

                desligarTodosMotores(); // Desliga todos os motores
                Serial.println("Sistema em manutenção. Motores desativados.");
            }
            else
            {
                // Saiu do modo de manutenção
                sistemaEmManutencao = false;

                // Restaura o estado dos motores ao estado anterior
                for (int i = 0; i < 3; i++)
                {
                    if (motoresEstadoAnterior[i])
                    {
                        digitalWrite(pinosMotores[i], HIGH);
                        delay(500); // Mantém o pulso por 500ms
                        digitalWrite(pinosMotores[i], LOW);
                        motoresLigados[i] = true;
                        saveMotorState(arquivosMotores[i], true);
                    }
                }

                Serial.println("Estado de manutenção: Inativo. Motores restaurados.");
            }

            return true; // Houve mudança
        }
    }

    return false; // Nenhuma mudança
}