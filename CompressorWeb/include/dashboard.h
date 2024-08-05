#ifndef DASHBOARD_H
#define DASHBOARD_H

// -------------------------------------------------------------------------
// Inclusão de Bibliotecas
// -------------------------------------------------------------------------

#include <ESPAsyncWebServer.h>  // Inclui a biblioteca para criar e gerenciar um servidor web assíncrono

// -------------------------------------------------------------------------
// Declaração de Funções
// -------------------------------------------------------------------------

/**
 * Configura a página do dashboard no servidor web.
 * 
 * Esta função define a configuração para a página do dashboard acessada pelo servidor web.
 * Ela adiciona rotas HTTP e manipuladores (handlers) necessários para servir o conteúdo da
 * página do dashboard. A página do dashboard pode incluir informações de status e controles.
 * 
 * @param server Referência para o objeto AsyncWebServer usado para adicionar e configurar rotas e handlers.
 */
void setupDashboardPage(AsyncWebServer& server);

/**
 * Configura as ações de ligar/desligar do compressor no servidor web.
 * 
 * Esta função adiciona rotas e manipuladores (handlers) específicos para controlar o estado do compressor
 * (ligar ou desligar). A função deve tratar as requisições do usuário para alterar o estado do compressor.
 * 
 * @param server Referência para o objeto AsyncWebServer usado para definir rotas e handlers relacionados ao controle do compressor.
 */
void handleToggleAction(AsyncWebServer& server);

#endif // DASHBOARD_H
