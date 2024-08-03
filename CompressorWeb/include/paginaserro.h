#ifndef PAGINASERRO_H
#define PAGINASERRO_H

#include <WebServer.h>

void setupAcessoInvalidoPage(WebServer& server);
void setupNotFoundPage(WebServer& server);
void setupUsuarioJaLogadoPage(WebServer& server);
void setupCredenciaisInvalidasPage(WebServer& server);

#endif // PAGINASERRO_H
