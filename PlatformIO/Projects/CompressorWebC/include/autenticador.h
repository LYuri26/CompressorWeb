// autenticador.h
#ifndef AUTENTICADOR_H
#define AUTENTICADOR_H

#include <WebServer.h>

// Declarações das funções
void handleLogin();
void handleLogout();

extern bool userLoggedIn; // Declara a variável como externa
extern String loggedInUser;

extern WebServer server;

#endif // AUTENTICADOR_H
