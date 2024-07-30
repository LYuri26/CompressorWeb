#ifndef CREDITOS_H  // Verifica se o identificador CREDITOS_H não está definido
#define CREDITOS_H  // Define o identificador CREDITOS_H para evitar inclusão múltipla deste bloco de código

#include <WebServer.h>  // Inclui a biblioteca WebServer, que fornece funcionalidades para criar e gerenciar um servidor web no ESP32

// Declara a função setupCreditsPage
// Esta função é responsável por configurar a página de créditos no servidor web
// Recebe uma referência ao objeto WebServer como parâmetro, que é usado para definir rotas e handlers
void setupCreditosPage(WebServer& server);

#endif // CREDITOS_H  // Marca o fim da diretiva de pré-processador #ifndef/#define
