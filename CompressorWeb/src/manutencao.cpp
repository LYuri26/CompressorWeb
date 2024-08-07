#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include "manutencao.h"
#include "ligadesliga.h" // Inclua o arquivo onde compressorLigado é declarado
#include "dashboard.h"

// Configuração do pino para o botão de manutenção
const int pinoManutencao = 4;

// Variável para verificar se o sistema está em manutenção
bool sistemaEmManutencao = false;

void setupManutencao()
{
    pinMode(pinoManutencao, INPUT_PULLUP); // Configura o pino do botão com pull-up interno
}

void atualizarEstadoManutencao()
{
    static unsigned long lastMaintenanceCheck = 0;
    unsigned long currentMillis = millis();

    // Verifica o estado do botão de manutenção a cada 500 ms para evitar leituras excessivas
    if (currentMillis - lastMaintenanceCheck >= 500)
    {
        lastMaintenanceCheck = currentMillis;

        // Verifica o estado do pino de manutenção
        if (digitalRead(pinoManutencao) == LOW) // Botão pressionado
        {
            if (!sistemaEmManutencao)
            {
                sistemaEmManutencao = true;
                compressorLigado = false; // Desativa o compressor
                digitalWrite(pinoLigaDesliga, LOW); // Garante que o compressor está desligado
                saveCompressorState(compressorLigado); // Salva o estado do compressor
                Serial.println("Sistema em manutenção. Compressor desligado.");
            }
        }
        else
        {
            if (sistemaEmManutencao)
            {
                sistemaEmManutencao = false;
                Serial.println("Estado de manutenção: Inativo");
            }
        }
    }
}