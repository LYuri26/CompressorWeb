#ifndef LIGADESLIGA_H
#define LIGADESLIGA_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

// Declaração de Variáveis Globais
extern int pinosMotores[];           // Pinos de controle dos motores
extern int pinosStatus[];            // Pinos de status dos compressores
extern bool motoresLigados[];        // Estado dos motores
extern bool sistemaEmManutencao;     // Estado de manutenção
extern bool timersAtivos[];          // Timers dos motores
extern bool motoresEstadoAnterior[]; // Estado dos motores antes da manutenção ou sobrecarga
extern bool isAfterClosingTime();
extern bool isBeforeOpeningTime();
extern const String arquivosTimers[];

// Arquivos para salvar o estado dos motores
extern const String arquivosMotores[];
extern const String arquivosStatus[];

// Funções
void setupLigaDesliga(AsyncWebServer &server);
void saveMotorState(const String &arquivoEstado, bool state);
void updateMotorStatus();
void monitorarStatusCompressores();
void saveStatusState(int pin, bool state);
void desligarTodosMotores();
bool sistemaDeveEstarBloqueado();
void atualizarEstadoMotores();
void desativarSistemaForaDoHorario();
bool checkMotorStatusChange(); // Declaração da função
bool isMotorTimerActive(int motorIdx);
void updateTimers();

#endif // LIGADESLIGA_H