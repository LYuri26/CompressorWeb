#include <Arduino.h>            // Inclui a biblioteca principal do Arduino para funcionalidades básicas
#include <ESPAsyncWebServer.h>  // Inclui a biblioteca para criar um servidor web assíncrono
#include "manutencao.h"         // Inclui o cabeçalho que pode definir funções e variáveis relacionadas à manutenção
#include "ligadesliga.h"        // Inclui o cabeçalho onde a variável compressorLigado é declarada e funções relacionadas ao compressor
#include "dashboard.h"         // Inclui o cabeçalho que pode definir funções e variáveis relacionadas ao dashboard

// -------------------------------------------------------------------------
// Configuração do pino para o botão de manutenção
// -------------------------------------------------------------------------

const int pinoManutencao = 4; // Define o pino 4 como o pino para o botão de manutenção

// -------------------------------------------------------------------------
// Variável para verificar se o sistema está em manutenção
// -------------------------------------------------------------------------

bool sistemaEmManutencao = false; // Inicializa a variável que indica se o sistema está em manutenção como falso

// -------------------------------------------------------------------------
// Função para configurar o botão de manutenção
// -------------------------------------------------------------------------

void setupManutencao()
{
    pinMode(pinoManutencao, INPUT_PULLUP); // Configura o pino do botão de manutenção como entrada com resistência pull-up interna
}

// -------------------------------------------------------------------------
// Função para atualizar o estado do sistema de manutenção
// -------------------------------------------------------------------------

void atualizarEstadoManutencao()
{
    static unsigned long lastMaintenanceCheck = 0; // Variável estática para armazenar o último momento de verificação
    unsigned long currentMillis = millis(); // Obtém o tempo atual em milissegundos desde o início do programa

    // Verifica o estado do botão de manutenção a cada 500 ms para evitar leituras excessivas
    if (currentMillis - lastMaintenanceCheck >= 500) // Se passaram pelo menos 500 ms desde a última verificação
    {
        lastMaintenanceCheck = currentMillis; // Atualiza o tempo da última verificação

        // Verifica o estado do pino de manutenção
        if (digitalRead(pinoManutencao) == LOW) // Se o botão estiver pressionado (nível baixo)
        {
            if (!sistemaEmManutencao) // Se o sistema não está em manutenção
            {
                sistemaEmManutencao = true; // Define o sistema como em manutenção
                compressorLigado = false; // Desativa o compressor
                digitalWrite(pinoLigaDesliga, LOW); // Garante que o pino de controle do compressor está em nível baixo (desligado)
                saveCompressorState(compressorLigado); // Salva o estado atual do compressor
                Serial.println("Sistema em manutenção. Compressor desligado."); // Imprime uma mensagem indicando que o sistema está em manutenção
            }
        }
        else // Se o botão não estiver pressionado
        {
            if (sistemaEmManutencao) // Se o sistema está em manutenção
            {
                sistemaEmManutencao = false; // Define o sistema como não em manutenção
                Serial.println("Estado de manutenção: Inativo"); // Imprime uma mensagem indicando que o estado de manutenção foi desativado
            }
        }
    }
}
