#ifndef CREDITOS_H  // Verifica se a macro CREDITOS_H não está definida
#define CREDITOS_H  // Define a macro CREDITOS_H para evitar inclusão múltipla deste cabeçalho

#include <ESPAsyncWebServer.h>  // Inclui a biblioteca para o servidor web assíncrono

// Declaração da função setupCreditosPage
// A função setupCreditosPage é responsável por configurar a página de créditos no servidor web
// Recebe uma referência ao objeto AsyncWebServer como parâmetro, que será utilizado para definir rotas e manipuladores (handlers)
void setupCreditosPage(AsyncWebServer& server);

#endif // CREDITOS_H  // Finaliza a diretiva de pré-processador #ifndef/#define
