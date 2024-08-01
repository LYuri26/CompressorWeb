#ifndef LIGADESLIGA_H  // Verifica se a macro LIGADESLIGA_H não está definida
#define LIGADESLIGA_H  // Define a macro LIGADESLIGA_H para evitar a inclusão múltipla deste cabeçalho

#include <WebServer.h>  // Inclui a biblioteca WebServer, que fornece as funcionalidades para criar e gerenciar um servidor web

// Declaração da função setupLigaDesliga
// Esta função configura as rotas e handlers do servidor web relacionados à funcionalidade de ligar e desligar
void setupLigaDesliga(WebServer& server);  // A função recebe uma referência para um objeto WebServer e não retorna nenhum valor

// Declaração da função updateCompressorStatus
// Esta função é responsável por atualizar o status do compressor, possivelmente com base em alguma lógica ou condição
void updateCompressorStatus();  // A função não recebe parâmetros e não retorna nenhum valor

#endif // LIGADESLIGA_H  // Finaliza a proteção contra inclusão múltipla do cabeçalho
