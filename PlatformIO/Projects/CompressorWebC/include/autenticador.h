#ifndef AUTENTICADOR_H
#define AUTENTICADOR_H

#include <WebServer.h>

// Declaração das funções
void handleLogin();
void handleLogout();

// Variáveis globais
extern bool userLoggedIn;
extern String loggedInUser;
extern WebServer server;

#endif // AUTENTICACAO_H
