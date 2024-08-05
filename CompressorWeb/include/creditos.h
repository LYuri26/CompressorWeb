#ifndef CREDITOS_H
#define CREDITOS_H

// -------------------------------------------------------------------------
// Inclusão de Bibliotecas
// -------------------------------------------------------------------------

#include <ESPAsyncWebServer.h>  // Inclui a biblioteca para o servidor web assíncrono

// -------------------------------------------------------------------------
// Declaração de Funções
// -------------------------------------------------------------------------

/**
 * Configura a página de créditos no servidor web.
 * 
 * Esta função define as rotas e manipuladores (handlers) necessários para a página de créditos,
 * que é uma página web acessada pelo servidor. A página de créditos pode conter informações sobre
 * os criadores do projeto, agradecimentos e outras informações relacionadas.
 * 
 * @param server Referência para o objeto AsyncWebServer utilizado para adicionar e configurar rotas e handlers.
 */
void setupCreditosPage(AsyncWebServer& server);

#endif // CREDITOS_H
