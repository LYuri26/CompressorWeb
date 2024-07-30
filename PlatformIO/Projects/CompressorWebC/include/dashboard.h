#ifndef DASHBOARD_H  // Se o identificador DASHBOARD_H não está definido, o compilador inclui o código a seguir
#define DASHBOARD_H  // Define o identificador DASHBOARD_H para evitar inclusão múltipla deste bloco de código

#include <WebServer.h>  // Inclui a biblioteca WebServer que fornece funcionalidades para criar e gerenciar um servidor web no ESP32

// Declaração da função setupDashboardPage
// Esta função é responsável por configurar a página do dashboard no servidor web
// Recebe uma referência ao objeto WebServer como parâmetro, que é usado para definir rotas e handlers
void setupDashboardPage(WebServer& server);

#endif  // Marca o fim da diretiva de pré-processador #ifndef/#define
