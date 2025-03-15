#ifndef MANUTENCAO_H
#define MANUTENCAO_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include "ligadesliga.h"

extern bool sistemaEmManutencao;

void setupManutencao();
void atualizarEstadoManutencao();

#endif