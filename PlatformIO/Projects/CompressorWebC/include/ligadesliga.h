#ifndef LIGADESLIGA_H  // Se não definido LIGADESLIGA_H
#define LIGADESLIGA_H  // Define LIGADESLIGA_H

#include <ESP8266WebServer.h>  // Inclui a biblioteca ESP8266WebServer para manipulação de servidor web

// Declara a função setupLigaDesliga, que configura a funcionalidade de ligar/desligar
// A função recebe uma referência a um objeto ESP8266WebServer como parâmetro
void setupLigaDesliga(ESP8266WebServer& server);

#endif // LIGADESLIGA_H  // Finaliza a diretiva de pré-processador #ifndef/#define
