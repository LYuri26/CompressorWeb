#ifndef DASHBOARD_H  // Verifica se a macro DASHBOARD_H não está definida
#define DASHBOARD_H  // Define a macro DASHBOARD_H para evitar múltiplas inclusões deste arquivo de cabeçalho

#include <WebServer.h>  // Inclui a biblioteca WebServer que fornece funcionalidades para criar e gerenciar um servidor web no ESP32

// Declaração da função setupDashboardPage
// A função setupDashboardPage é responsável por configurar a página do dashboard no servidor web
// Recebe uma referência para um objeto WebServer como parâmetro, que será utilizado para definir rotas e manipuladores (handlers)
void setupDashboardPage(WebServer& server);

// Declaração da função handleToggleAction
// A função handleToggleAction é responsável por configurar as ações de ligar/desligar do compressor no servidor web
// Recebe uma referência para um objeto WebServer como parâmetro, que será utilizado para definir rotas e manipuladores (handlers)
void handleToggleAction(WebServer& server);

#endif  // Finaliza a diretiva de pré-processador #ifndef/#define
