#ifndef PRESSAO_H
#define PRESSAO_H

// -------------------------------------------------------------------------
// Inclusão de Bibliotecas
// -------------------------------------------------------------------------

#include <ESPAsyncWebServer.h> // Biblioteca para criar e gerenciar um servidor web assíncrono

// -------------------------------------------------------------------------
// Declaração de Funções
// -------------------------------------------------------------------------

/**
 * Configura a página de nível de óleo no servidor web.
 * 
 * Esta função define a rota e o conteúdo da página de nível de óleo, 
 * que exibirá informações relacionadas ao nível de óleo em um formato 
 * acessível via um servidor web assíncrono.
 * 
 * @param server Referência para o objeto do servidor web assíncrono que 
 *               gerenciará a configuração da página.
 */
void setupPressaoPage(AsyncWebServer& server);

#endif // PRESSAO_H
