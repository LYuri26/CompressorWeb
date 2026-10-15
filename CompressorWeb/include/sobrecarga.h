#ifndef SOBRECARGA_H
#define SOBRECARGA_H

#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>
#include "ligadesliga.h"

// -------------------------------------------------------------------------
// Declaração de Variáveis Globais de Sobrecarga
// -------------------------------------------------------------------------

// Pinos de entrada digital para monitorar contatores/relés térmicos
extern const int pinosSobrecarga[NUM_MOTORES];

// Flag de status: true se houver desarme térmico/sobrecarga no motor
extern bool sobrecargaDetectada[NUM_MOTORES];

// Temporizadores e constantes para validação de sobrecarga
extern unsigned long tempoInicioSobrecarga[NUM_MOTORES];
extern const unsigned long tempoMinimoSobrecarga;

// -------------------------------------------------------------------------
// Declaração de Funções de Proteção Térmica
// -------------------------------------------------------------------------

void setupSobrecarga();
void monitorarSobrecarga();
void desativarMotor(int motorIndex);
void desativarMotores();
void desativarMotoresSobrecarga();
void restaurarEstadoMotores();

#endif // SOBRECARGA_H