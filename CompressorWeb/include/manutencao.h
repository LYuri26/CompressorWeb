#ifndef MANUTENCAO_H
#define MANUTENCAO_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include "ligadesliga.h"

// -------------------------------------------------------------------------
// Declaração de Variáveis Globais de Manutenção
// -------------------------------------------------------------------------

/**
 * Pino digital de entrada da chave seletora ou botão tipo cogumelo com trava.
 * Nível lógico LOW indica sistema em manutenção (chave comutada para GND).
 */
extern const int pinoManutencao;

/**
 * Flag indicando se o sistema se encontra atualmente em modo de manutenção.
 */
extern bool sistemaEmManutencao;

// -------------------------------------------------------------------------
// Declaração de Funções de Controle de Manutenção
// -------------------------------------------------------------------------

void setupManutencao();
bool atualizarEstadoManutencao();
bool isSistemaEmManutencao();
void forcarSaidaManutencao();

#endif // MANUTENCAO_H