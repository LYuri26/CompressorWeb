#ifndef PAGINASERRO_H
#define PAGINASERRO_H

// -------------------------------------------------------------------------
// Inclusão da Biblioteca para Servidor Web Assíncrono
// -------------------------------------------------------------------------

#include <ESPAsyncWebServer.h> // Biblioteca necessária para criar e gerenciar um servidor web assíncrono

// -------------------------------------------------------------------------
// Declaração de Funções
// -------------------------------------------------------------------------

/**
 * Configura a página de acesso inválido no servidor web.
 * 
 * Esta função define a rota e o conteúdo para uma página de acesso inválido,
 * que será exibida quando um usuário tentar acessar uma área restrita sem
 * as permissões necessárias.
 * 
 * @param server Referência para o objeto do servidor web assíncrono que gerenciará a página.
 */
void setupAcessoInvalidoPage(AsyncWebServer& server);

/**
 * Configura a página de erro 404 (não encontrado) no servidor web.
 * 
 * Esta função define a rota e o conteúdo para uma página de erro 404, que será
 * exibida quando o usuário tentar acessar uma URL que não existe no servidor.
 * 
 * @param server Referência para o objeto do servidor web assíncrono que gerenciará a página.
 */
void setupNotFoundPage(AsyncWebServer& server);

/**
 * Configura a página para usuários já logados no servidor web.
 * 
 * Esta função define a rota e o conteúdo para uma página que informa ao usuário
 * que ele já está logado e não precisa fazer login novamente.
 * 
 * @param server Referência para o objeto do servidor web assíncrono que gerenciará a página.
 */
void setupUsuarioJaLogadoPage(AsyncWebServer& server);

/**
 * Configura a página de credenciais inválidas no servidor web.
 * 
 * Esta função define a rota e o conteúdo para uma página que informa ao usuário
 * que as credenciais fornecidas durante o login são inválidas.
 * 
 * @param server Referência para o objeto do servidor web assíncrono que gerenciará a página.
 */
void setupCredenciaisInvalidasPage(AsyncWebServer& server);

#endif // PAGINASERRO_H
