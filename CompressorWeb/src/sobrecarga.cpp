#include "sobrecarga.h"
#include "ligadesliga.h"

// Pinos de entrada para monitorar a sobrecarga
const int pinosSobrecarga[] = {15, 25, 18};
bool sobrecargaDetectada[] = {false, false, false}; // Estado de sobrecarga para cada motor
bool ultimoEstadoSobrecarga[] = {false, false, false};
unsigned long ultimoTempoSobrecarga[3] = {0, 0, 0};
const unsigned long DEBOUNCE_DELAY = 100; // 100ms de debounce

void setupSobrecarga()
{
    // Inicializa os pinos de entrada para sobrecarga com pull-down
    for (int i = 0; i < 3; i++)
    {
        pinMode(pinosSobrecarga[i], INPUT_PULLDOWN);
    }

    // Inicializa os pinos de controle dos motores e status
    for (int i = 0; i < 3; i++)
    {
        pinMode(pinosMotores[i], OUTPUT);
        pinMode(pinosStatus[i], OUTPUT);
        digitalWrite(pinosMotores[i], LOW); // Garante que os motores comecem desligados
        digitalWrite(pinosStatus[i], LOW);  // Garante que os status comecem desligados
    }

    Serial.println("Sistema de sobrecarga inicializado.");
}

void monitorarSobrecarga()
{
    for (int i = 0; i < 3; i++)
    {
        bool sobrecargaAtual = digitalRead(pinosSobrecarga[i]) == HIGH;

        // Verifica se houve mudança no estado de sobrecarga
        if (sobrecargaAtual != ultimoEstadoSobrecarga[i])
        {
            if (millis() - ultimoTempoSobrecarga[i] > DEBOUNCE_DELAY)
            {
                ultimoEstadoSobrecarga[i] = sobrecargaAtual;
                sobrecargaDetectada[i] = sobrecargaAtual;
                ultimoTempoSobrecarga[i] = millis();

                if (sobrecargaAtual)
                {
                    digitalWrite(pinosMotores[i], LOW);
                    digitalWrite(pinosStatus[i], LOW);
                    Serial.println("Sobrecarga detectada no pino " + String(pinosSobrecarga[i]) + ". Motor " + String(i + 1) + " desativado e pinos bloqueados.");
                }
                else
                {
                    digitalWrite(pinosMotores[i], LOW);
                    digitalWrite(pinosStatus[i], LOW);
                    Serial.println("Sobrecarga resolvida no pino " + String(pinosSobrecarga[i]) + ". Motor " + String(i + 1) + " restaurado.");
                }
            }
        }
    }
}

void desativarMotores()
{
    for (int i = 0; i < 3; i++)
    {
        digitalWrite(pinosMotores[i], LOW);
        digitalWrite(pinosStatus[i], LOW);
    }
    Serial.println("Todos os motores desativados e pinos bloqueados devido à sobrecarga.");
}

void restaurarEstadoMotores()
{
    for (int i = 0; i < 3; i++)
    {
        // Restaura o estado dos motores (desligados por padrão)
        digitalWrite(pinosMotores[i], LOW);
        digitalWrite(pinosStatus[i], LOW);
        Serial.println("Estado do motor " + String(i + 1) + " restaurado: Desligado");
    }
}