#ifndef LIGADESLIGA_H
#define LIGADESLIGA_H

#include <ESPAsyncWebServer.h>

// Declaração de Variáveis Globais
extern int pinosMotores[];       // Pinos de controle dos motores
extern int pinosStatus[];        // Pinos de status dos compressores
extern bool motoresLigados[];    // Estado dos motores
extern bool sistemaEmManutencao; // Estado de manutenção
extern bool timersAtivos[];      // Timers dos motores

// Arquivos para salvar o estado dos motores
extern const String arquivosEstados[];

// Funções
void setupLigaDesliga(AsyncWebServer &server);
void saveMotorState(const String &arquivoEstado, bool state);
void updateMotorStatus();
void monitorarStatusCompressores();
void saveStatusState(int pin, bool state);
void desligarTodosMotores();
bool sistemaDeveEstarBloqueado();
void atualizarEstadoMotores();

#endif // LIGADESLIGA_H