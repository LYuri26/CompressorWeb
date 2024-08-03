#ifndef LIGADESLIGA_H
#define LIGADESLIGA_H

#include <ESPAsyncWebServer.h>  // Inclui a biblioteca para o servidor web assíncrono

// Declaração da função setupLigaDesliga
// Esta função configura as rotas e handlers do servidor web relacionados à funcionalidade de ligar e desligar
void setupLigaDesliga(AsyncWebServer& server);  // A função recebe uma referência para um objeto AsyncWebServer e não retorna nenhum valor

// Declaração da função updateCompressorStatus
// Esta função é responsável por atualizar o status do compressor, possivelmente com base em alguma lógica ou condição
void updateCompressorStatus();  // A função não recebe parâmetros e não retorna nenhum valor

#endif // LIGADESLIGA_H
