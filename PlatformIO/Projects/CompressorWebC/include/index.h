#ifndef INDEX_H  // Verifica se a macro INDEX_H não está definida
#define INDEX_H  // Define a macro INDEX_H para evitar múltiplas inclusões deste arquivo de cabeçalho

#include <WebServer.h>  // Inclui a biblioteca WebServer que fornece funcionalidades para criar e gerenciar um servidor web no ESP32

// Declaração da função setupIndexPage
// A função setupIndexPage é responsável por configurar a página inicial do servidor web
// Recebe uma referência para um objeto WebServer como parâmetro, que será utilizado para definir rotas e manipuladores (handlers)
void setupIndexPage(WebServer& server);

#endif  // Finaliza a diretiva de pré-processador #ifndef/#define
