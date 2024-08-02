#ifndef PAGINASERRO_H
#define PAGINASERRO_H

#include <WebServer.h>  // ou <WebServer.h> para ESP32

// Função para enviar a página de "Usuário Já Logado"
void sendAlreadyLoggedInPage(WebServer &server);

// Função para enviar a página de "Credenciais Inválidas"
void sendInvalidCredentialsPage(WebServer &server);

// Função para enviar a página de "Erro de Logout"
void sendLogoutErrorPage(WebServer &server);

// Função para configurar as páginas de erro
void setupErrorPages(WebServer &server);

void setupErrorPages(WebServer &server);

#endif // PAGINAS_ERRO_H
