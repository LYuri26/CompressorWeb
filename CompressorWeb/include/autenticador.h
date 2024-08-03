#ifndef AUTENTICADOR_H
#define AUTENTICADOR_H

#include <ESPAsyncWebServer.h>

// Declarando as funções com o parâmetro AsyncWebServerRequest*
void handleLogin(AsyncWebServerRequest *request);
void handleLogout(AsyncWebServerRequest *request);
extern bool userLoggedIn;
extern String loggedInUser;

#endif
