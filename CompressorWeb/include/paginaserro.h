#ifndef PAGINASERRO_H
#define PAGINASERRO_H

#include <ESPAsyncWebServer.h>  // Inclui a biblioteca para o servidor web assíncrono

void setupAcessoInvalidoPage(AsyncWebServer& server);
void setupNotFoundPage(AsyncWebServer& server);
void setupUsuarioJaLogadoPage(AsyncWebServer& server);
void setupCredenciaisInvalidasPage(AsyncWebServer& server);

#endif // PAGINASERRO_H
