#ifndef LIGADESLIGA_H
#define LIGADESLIGA_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

#define NUM_MOTORES 3

// -------------------------------------------------------------------------
// Declaração de Variáveis e Pinos Globais (Memória RAM)
// -------------------------------------------------------------------------
extern int pinosMotores[NUM_MOTORES];           // Pinos de controle (pulsos)
extern int pinosStatus[NUM_MOTORES];            // Pinos de feedback dos compressores
extern bool motoresLigados[NUM_MOTORES];        // Estado operacional em RAM
extern bool timersAtivos[NUM_MOTORES];          // Indicador de timer de segurança ativo
extern bool motoresEstadoAnterior[NUM_MOTORES]; // Estado antes de manutenção/falhas
extern unsigned long motorTimers[NUM_MOTORES];  // Timestamp de liberação

// Arquivos de persistência no SPIFFS
extern const String arquivosMotores[NUM_MOTORES];
extern const String arquivosStatus[NUM_MOTORES];
extern const String arquivosTimers[NUM_MOTORES];

// -------------------------------------------------------------------------
// Funções de Inicialização e Hardware Não-Bloqueante
// -------------------------------------------------------------------------
void setupLigaDesliga(AsyncWebServer &server);
void processarPulsosMotores();         // Desliga o pulso de 500ms sem delay()
void dispararPulsoMotor(int motorIdx); // Inicia o pulso assíncrono no pino
void monitorarStatusCompressores();    // Faz a leitura física dos compressores
void updateMotorStatus();              // Sincroniza hardware com arquivos

// -------------------------------------------------------------------------
// Funções de Controle e Temporização
// -------------------------------------------------------------------------
void updateTimers();                   // Atualiza cronômetros no disco/RAM
bool isMotorTimerActive(int motorIdx); // Verifica se motor está em descanso
unsigned long getMotorRemainingTime(int motorIdx);
void handleToggleAction(AsyncWebServer &server);

// -------------------------------------------------------------------------
// Funções de Travas e Regras de Segurança
// -------------------------------------------------------------------------
void desligarTodosMotores();
void atualizarEstadoMotores();
void desativarSistemaForaDoHorario();
bool sistemaDeveEstarBloqueado();
bool checkMotorStatusChange();

// -------------------------------------------------------------------------
// Verificação de Horário Operacional
// -------------------------------------------------------------------------
bool isAfterClosingTime();
bool isBeforeOpeningTime();

// -------------------------------------------------------------------------
// Persistência com Cache Seguro (Sem desgaste da Flash)
// -------------------------------------------------------------------------
void saveMotorState(const String &arquivoEstado, bool state);
bool readMotorState(const String &arquivoEstado);
void saveStatusState(int pin, bool state);
unsigned long readTimerState(const String &arquivoTimer);
void saveTimerState(const String &arquivoTimer, unsigned long remainingTime);

#endif // LIGADESLIGA_H