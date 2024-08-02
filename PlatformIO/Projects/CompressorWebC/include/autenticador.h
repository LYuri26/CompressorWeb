#ifndef AUTENTICADOR_H
#define AUTENTICADOR_H

#include <WebServer.h>

extern WebServer server;

extern bool userLoggedIn;
extern String loggedInUser;

void handleLogin();
void handleLogout();
void sendAlreadyLoggedInPage();
void sendInvalidCredentialsPage();
void sendLogoutErrorPage();

#endif
