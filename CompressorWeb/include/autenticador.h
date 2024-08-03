#ifndef AUTENTICADOR_H
#define AUTENTICADOR_H

#include <ESPAsyncWebServer.h>

// Declaração das variáveis globais
extern String sessionId;
extern bool userLoggedIn;
extern String loggedInUser;

// Declaração das funções
void handleLogin(AsyncWebServerRequest *request);
void handleLogout(AsyncWebServerRequest *request);
bool isAuthenticated(AsyncWebServerRequest *request);
void notAuthenticated(AsyncWebServerRequest *request);

#endif // AUTENTICADOR_H
