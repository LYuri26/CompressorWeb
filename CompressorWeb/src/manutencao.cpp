#include <Arduino.h>           // Inclui a biblioteca principal do Arduino
#include <ESPAsyncWebServer.h> // Inclui a biblioteca para criar um servidor web assíncrono no ESP32
#include "manutencao.h"        // Inclui o cabeçalho que define funções e variáveis relacionadas à manutenção
#include "ligadesliga.h"       // Inclui o cabeçalho atualizado que define funções para ligar e desligar componentes
#include "dashboard.h"         // Inclui o cabeçalho para funções relacionadas ao painel de controle

// -------------------------------------------------------------------------
// Configuração do pino para o botão de manutenção
// -------------------------------------------------------------------------
const int pinoManutencao = 25; // Define o pino 25 como o pino para o botão de manutenção

// -------------------------------------------------------------------------
// Variável para verificar se o sistema está em manutenção
// -------------------------------------------------------------------------
bool sistemaEmManutencao = false; // Inicializa a variável que indica se o sistema está em manutenção como falso

// -------------------------------------------------------------------------
// Função para configurar o botão de manutenção
// -------------------------------------------------------------------------
void setupManutencao()
{
    // Configura o pino do botão de manutenção como entrada com resistência pull-up interna
    pinMode(pinoManutencao, INPUT_PULLUP); // INPUT_PULLUP ativa a resistência pull-up interna
}

// -------------------------------------------------------------------------
// Função para atualizar o estado do sistema de manutenção
// -------------------------------------------------------------------------
void atualizarEstadoManutencao()
{
    static unsigned long lastMaintenanceCheck = 0;
    unsigned long currentMillis = millis();

    // Verifica o estado do botão de manutenção a cada 500 ms
    if (currentMillis - lastMaintenanceCheck >= 500)
    {
        lastMaintenanceCheck = currentMillis;

        // Verifica o estado do pino de manutenção
        if (digitalRead(pinoManutencao) == LOW)
        {
            if (!sistemaEmManutencao)
            {
                sistemaEmManutencao = true;
                desligarTodosMotores(); // Desliga todos os motores
                Serial.println("Sistema em manutenção. Motores desativados.");
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