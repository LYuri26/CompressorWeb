#ifndef MANUTENCAO_H          // Verifica se o cabeçalho MANUTENCAO_H não foi definido anteriormente
#define MANUTENCAO_H          // Define o cabeçalho MANUTENCAO_H para evitar inclusões múltiplas

#include <ESPAsyncWebServer.h>  // Inclui a biblioteca para criar um servidor web assíncrono

// -------------------------------------------------------------------------
// Declarações das funções e variáveis relacionadas à manutenção
// -------------------------------------------------------------------------

// Função para configurar o botão de manutenção
void setupManutencao();        // Declara a função que configura o botão de manutenção

// Função para atualizar o estado do botão de manutenção
void atualizarEstadoManutencao(); // Declara a função que atualiza o estado do botão de manutenção

// Variável para verificar se o sistema está em manutenção
extern bool sistemaEmManutencao; // Declara a variável global 'sistemaEmManutencao' como extern, indicando que é definida em outro lugar

#endif // MANUTENCAO_H          // Marca o fim da inclusão condicional do cabeçalho MANUTENCAO_H
