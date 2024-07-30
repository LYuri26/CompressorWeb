#ifndef LIGADESLIGA_H  // Verifica se o identificador LIGADESLIGA_H não está definido
#define LIGADESLIGA_H  // Define o identificador LIGADESLIGA_H para evitar inclusões múltiplas do cabeçalho

#include <WebServer.h>  // Inclui a biblioteca WebServer, que fornece as funcionalidades para criar um servidor web

// Declaração da função setupLigaDesliga
// Esta função configura a funcionalidade de ligar/desligar, associando rotas e handlers ao servidor web
void setupLigaDesliga(WebServer& server);

// Declaração da função updateCompressorStatus
// Esta função atualiza o status do compressor com base em condições definidas
void updateCompressorStatus();

#endif // LIGADESLIGA_H  // Marca o fim da inclusão condicional do cabeçalho
