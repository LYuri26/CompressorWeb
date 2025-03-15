#include "sobrecarga.h"
#include "ligadesliga.h"

// Pinos de entrada para monitorar a sobrecarga
const int pinosSobrecarga[] = {16, 17, 18};
bool sobrecargaDetectada[] = {false, false, false}; // Estado de sobrecarga para cada motor

void setupSobrecarga()
{
    // Inicializa os pinos de entrada para sobrecarga
    for (int i = 0; i < 3; i++)
    {
        pinMode(pinosSobrecarga[i], INPUT);
    }

    // Inicializa os pinos de controle dos motores e status
    for (int i = 0; i < 3; i++)
    {
        pinMode(pinosMotores[i], OUTPUT);
        pinMode(pinosStatus[i], OUTPUT);
        digitalWrite(pinosMotores[i], LOW); // Garante que os motores comecem desligados
        digitalWrite(pinosStatus[i], LOW);  // Garante que os status comecem desligados
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
            sobrecargaDetectada[i] = sobrecargaAtual;    // Atualiza o estado de sobrecarga

            if (sobrecargaAtual)
            {
                // Se houver sobrecarga, desativa os pinos de controle e status
                digitalWrite(pinosMotores[i], LOW);
                digitalWrite(pinosStatus[i], LOW);
                Serial.println("Sobrecarga detectada no pino " + String(pinosSobrecarga[i]) + ". Motor " + String(i + 1) + " desativado e pinos bloqueados.");
            }
            else
            {
                // Se não houver sobrecarga, restaura o estado salvo
                bool estadoSalvo = readMotorState(arquivosMotores[i]);
                digitalWrite(pinosMotores[i], estadoSalvo ? HIGH : LOW);
                digitalWrite(pinosStatus[i], estadoSalvo ? HIGH : LOW);
                Serial.println("Sobrecarga resolvida no pino " + String(pinosSobrecarga[i]) + ". Motor " + String(i + 1) + " restaurado.");
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
        saveMotorState(arquivosMotores[i], false);
    }
    Serial.println("Todos os motores desativados e pinos bloqueados devido à sobrecarga.");
}

void restaurarEstadoMotores()
{
    for (int i = 0; i < 3; i++)
    {
        bool estadoSalvo = readMotorState(arquivosMotores[i]);
        digitalWrite(pinosMotores[i], estadoSalvo ? HIGH : LOW);
        digitalWrite(pinosStatus[i], estadoSalvo ? HIGH : LOW);
        Serial.println("Estado do motor " + String(i + 1) + " restaurado: " + (estadoSalvo ? "Ligado" : "Desligado"));
    }
}