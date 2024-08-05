#ifndef INDEX_H
#define INDEX_H

// -------------------------------------------------------------------------
// Inclusão de Bibliotecas
// -------------------------------------------------------------------------

#include <ESPAsyncWebServer.h>  // Biblioteca para criar e gerenciar um servidor web assíncrono

// -------------------------------------------------------------------------
// Declaração de Funções
// -------------------------------------------------------------------------

/**
 * Configura a página inicial do servidor web.
 * 
 * Esta função define a configuração para a página inicial acessada pelo servidor web.
 * Ela define as rotas HTTP e os manipuladores necessários para servir o conteúdo da
 * página inicial do servidor.
 * 
 * @param server Referência para o objeto AsyncWebServer que é usado para configurar rotas e handlers.
 */
void setupIndexPage(AsyncWebServer& server);

#endif // INDEX_H
