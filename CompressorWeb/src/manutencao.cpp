#include <Arduino.h>           // Inclui a biblioteca principal do Arduino, que fornece funções e tipos básicos para programação em C++ para microcontroladores
#include <ESPAsyncWebServer.h> // Inclui a biblioteca para criar um servidor web assíncrono no ESP32
#include "manutencao.h"        // Inclui o cabeçalho que define funções e variáveis relacionadas à manutenção
#include "ligadesliga.h"       // Inclui o cabeçalho atualizado que pode definir funções para ligar e desligar componentes
#include "dashboard.h"         // Inclui o cabeçalho para funções relacionadas ao painel de controle

// -------------------------------------------------------------------------
// Configuração do pino para o botão de manutenção
// -------------------------------------------------------------------------
const int pinoManutencao = 25; // Define o pino 25 como o pino para o botão de manutenção

// -------------------------------------------------------------------------
// Função para desativar todos os motores
// -------------------------------------------------------------------------
void desativarMotores()
{
    // Itera sobre todos os motores (supondo que há 3 motores)
    for (int i = 0; i < 3; i++)
    {
        motoresLigados[i] = false;                             // Define o estado do motor como desligado
        digitalWrite(pinosMotores[i], LOW);                    // Define o pino do motor como LOW, desativando-o (LOW geralmente significa desligado em circuitos digitais)
        saveMotorState(arquivosEstados[i], motoresLigados[i]); // Salva o estado do motor em um arquivo para persistência (presumivelmente em um sistema de arquivos)
    }
}

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
    pinMode(pinoManutencao, INPUT_PULLUP); // INPUT_PULLUP ativa a resistência pull-up interna, garantindo que o pino seja lido como HIGH quando o botão não está pressionado
}

// -------------------------------------------------------------------------
// Função para atualizar o estado do sistema de manutenção
// -------------------------------------------------------------------------
void atualizarEstadoManutencao()
{
    static unsigned long lastMaintenanceCheck = 0; // Variável estática para armazenar o último momento de verificação da manutenção (preserva o valor entre chamadas)
    unsigned long currentMillis = millis();        // Obtém o tempo atual em milissegundos desde que o programa começou (função millis() do Arduino)

    // Verifica o estado do botão de manutenção a cada 500 ms para evitar leituras excessivas
    if (currentMillis - lastMaintenanceCheck >= 500) // Se passaram pelo menos 500 milissegundos desde a última verificação
    {
        lastMaintenanceCheck = currentMillis; // Atualiza o tempo da última verificação para o tempo atual

        // Verifica o estado do pino de manutenção
        if (digitalRead(pinoManutencao) == LOW) // Se o botão estiver pressionado (nível baixo)
        {
            if (!sistemaEmManutencao) // Se o sistema não está em manutenção
            {
                sistemaEmManutencao = true;                                    // Define o sistema como em manutenção
                desativarMotores();                                            // Chama a função para desativar todos os motores
                Serial.println("Sistema em manutenção. Motores desativados."); // Imprime uma mensagem para o console serial indicando que o sistema está em manutenção
            }
        }
        else // Se o botão não estiver pressionado
        {
            if (sistemaEmManutencao) // Se o sistema está em manutenção
            {
                sistemaEmManutencao = false;                     // Define o sistema como não em manutenção
                Serial.println("Estado de manutenção: Inativo"); // Imprime uma mensagem para o console serial indicando que o estado de manutenção foi desativado
            }
        }
    }
}
