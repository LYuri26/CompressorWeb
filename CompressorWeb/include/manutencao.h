#ifndef MANUTENCAO_H
#define MANUTENCAO_H

#include <ESPAsyncWebServer.h>


// Função para configurar o botão de manutenção
void setupManutencao();

// Função para atualizar o estado do botão de manutenção
void atualizarEstadoManutencao();

// Variável para verificar se o sistema está em manutenção
extern bool sistemaEmManutencao;

#endif // MANUTENCAO_H
