#ifndef SOBRECARGA_H
#define SOBRECARGA_H

#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>
#include "ligadesliga.h"

// Pinos de entrada para monitorar a sobrecarga
extern const int pinosSobrecarga[];

// Estado de sobrecarga para cada motor
extern bool sobrecargaDetectada[];

// Tempo de início da sobrecarga e tempo mínimo para desativar os motores
extern unsigned long tempoInicioSobrecarga[];
extern const unsigned long tempoMinimoSobrecarga;

// Arquivos para salvar o estado dos motores
extern const String arquivosMotores[];

// Funções
void setupSobrecarga();
void desativarMotor(int motorIndex);
void monitorarSobrecarga();
void desativarMotores();
void restaurarEstadoMotores();
bool readMotorState(const String &arquivoEstado);
void saveMotorState(const String &arquivoEstado, bool state);

#endif // SOBRECARGA_H