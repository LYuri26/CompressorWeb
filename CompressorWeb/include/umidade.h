#ifndef UMIDADE_H
#define UMIDADE_H

// -------------------------------------------------------------------------
// Inclusão de Bibliotecas
// -------------------------------------------------------------------------

#include <ESPAsyncWebServer.h> // Inclui a biblioteca para o servidor web assíncrono

// -------------------------------------------------------------------------
// Declaração de Funções
// -------------------------------------------------------------------------

/**
 * Configura a página de umidade no servidor web.
 * 
 * @param server Instância do servidor web assíncrono para adicionar rotas e manipular requisições.
 * 
 * @note Esta função define a configuração da página da umidade que será servida pelo servidor web.
 * Ela deve ser chamada durante a configuração do servidor para garantir que a página de umidade esteja disponível.
 */
void setupUmidadePage(AsyncWebServer& server);

#endif // UMIDADE_H
