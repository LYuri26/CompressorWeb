#ifndef INDEX_H  // Se não definido INDEX_H
#define INDEX_H  // Define INDEX_H

#include <WebServer.h>  // Inclui a biblioteca WebServer para manipulação de servidor web no ESP32

// Declara a função setupIndexPage, que configura a página inicial (index)
// A função recebe uma referência a um objeto WebServer como parâmetro
void setupIndexPage(WebServer& server);

#endif  // Finaliza a diretiva de pré-processador #ifndef/#define
