#ifndef DASHBOARD_H  // Verifica se a macro DASHBOARD_H não está definida
#define DASHBOARD_H  // Define a macro DASHBOARD_H para evitar múltiplas inclusões deste arquivo de cabeçalho

#include <ESPAsyncWebServer.h>  // Inclui a biblioteca para o servidor web assíncrono

// Declaração da função setupDashboardPage
// A função setupDashboardPage é responsável por configurar a página do dashboard no servidor web
// Recebe uma referência para um objeto AsyncWebServer como parâmetro, que será utilizado para definir rotas e manipuladores (handlers)
void setupDashboardPage(AsyncWebServer& server);

// Declaração da função handleToggleAction
// A função handleToggleAction é responsável por configurar as ações de ligar/desligar do compressor no servidor web
// Recebe uma referência para um objeto AsyncWebServer como parâmetro, que será utilizado para definir rotas e manipuladores (handlers)
void handleToggleAction(AsyncWebServer& server);

#endif  // Finaliza a diretiva de pré-processador #ifndef/#define
