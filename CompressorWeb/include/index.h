#ifndef INDEX_H  // Verifica se a macro INDEX_H não está definida
#define INDEX_H  // Define a macro INDEX_H para evitar múltiplas inclusões deste arquivo de cabeçalho

#include <ESPAsyncWebServer.h>  // Inclui a biblioteca para o servidor web assíncrono

// Declaração da função setupIndexPage
// A função setupIndexPage é responsável por configurar a página inicial do servidor web
// Recebe uma referência para um objeto AsyncWebServer como parâmetro, que será utilizado para definir rotas e manipuladores (handlers)
void setupIndexPage(AsyncWebServer& server);

#endif  // Finaliza a diretiva de pré-processador #ifndef/#define
