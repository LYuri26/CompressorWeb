#ifndef PRESSOSTATO_H
#define PRESSOSTATO_H

#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>
#include "ligadesliga.h"

// -------------------------------------------------------------------------
// Declaração de Variáveis e Funções do Pressostato
// -------------------------------------------------------------------------

// Pino digital do pressostato e flag de estado do alarme
extern const int pinoPressostato;
extern bool pressostatoAtivo;

// Funções de inicialização e monitoramento contínuo
void setupPressostato();
void verificarPressostato();

#endif // PRESSOSTATO_H