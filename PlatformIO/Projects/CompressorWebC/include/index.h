#ifndef INDEX_H  // Se não definido INDEX_H
#define INDEX_H  // Define INDEX_H

#include <ESP8266WebServer.h>  // Inclui a biblioteca ESP8266WebServer para manipulação de servidor web

// Declara a função setupIndexPage, que configura a página inicial (index)
// A função recebe uma referência a um objeto ESP8266WebServer como parâmetro
void setupIndexPage(ESP8266WebServer& server);

#endif  // Finaliza a diretiva de pré-processador #ifndef/#define
