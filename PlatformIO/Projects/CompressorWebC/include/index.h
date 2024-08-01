#ifndef INDEX_H  // Se INDEX_H não está definido, o compilador inclui o código a seguir
#define INDEX_H  // Define INDEX_H para garantir que este bloco de código não seja incluído novamente

#include <WebServer.h>  // Inclui a biblioteca WebServer que permite criar e gerenciar um servidor web no ESP32

// Declara a função setupIndexPage
// Esta função é responsável por configurar a página inicial do servidor web
// Recebe uma referência ao objeto WebServer como parâmetro, que é usado para definir rotas e handlers
void setupIndexPage(WebServer& server);

#endif  // Finaliza a diretiva de pré-processador #ifndef/#define
