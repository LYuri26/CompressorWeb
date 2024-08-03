#ifndef CREDITOS_H  // Verifica se a macro CREDITOS_H não está definida
#define CREDITOS_H  // Define a macro CREDITOS_H para evitar inclusão múltipla deste cabeçalho

#include <WebServer.h>  // Inclui a biblioteca WebServer, que fornece funcionalidades para criar e gerenciar um servidor web no ESP32

// Declaração da função setupCreditosPage
// A função setupCreditosPage é responsável por configurar a página de créditos no servidor web
// Recebe uma referência ao objeto WebServer como parâmetro, que será utilizado para definir rotas e manipuladores (handlers)
void setupCreditosPage(WebServer& server);

#endif // CREDITOS_H  // Finaliza a diretiva de pré-processador #ifndef/#define
