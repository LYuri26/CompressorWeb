#ifndef MANUTENCAO_H
#define MANUTENCAO_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include "ligadesliga.h"

extern const int pinoManutencao; // Declaração da variável global
extern bool sistemaEmManutencao; // Declaração da variável global

void setupManutencao();           // Declaração da função
bool atualizarEstadoManutencao(); // Declaração da função

#endif