#ifndef LIGADESLIGA_H
#define LIGADESLIGA_H

#include <ESPAsyncWebServer.h>

// Declaração de Variáveis Globais
extern int pinosMotores[];       // Declaração da variável global para armazenar o estado dos motores
extern bool motoresLigados[];    // Declaração da variável global para armazenar o estado dos motores
extern bool sistemaEmManutencao; // Declaração da variável global para o estado de manutenção
extern bool timersAtivos[];      // Declaração da variável global para os timers dos motores

// Declaração das variáveis e funções usadas em outros arquivos
extern const String arquivosEstados[];
extern void saveMotorState(const String &arquivoEstado, bool state);
void updateMotorStatus();
// Declaração de Funções
void setupLigaDesliga(AsyncWebServer &server);

#endif // LIGADESLIGA_H
