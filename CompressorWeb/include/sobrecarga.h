#ifndef SOBRECARGA_H
#define SOBRECARGA_H

#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>

// Pinos de entrada para monitorar a sobrecarga
extern const int pinosSobrecarga[];

// Arquivos para salvar o estado dos motores
extern const String arquivosEstados[];
extern bool sobrecargaDetectada[];

// Funções
void setupSobrecarga();
void monitorarSobrecarga();
void desativarMotores();
void restaurarEstadoMotores();
bool readMotorState(const String &arquivoEstado);
void saveMotorState(const String &arquivoEstado, bool state);

#endif // SOBRECARGA_H