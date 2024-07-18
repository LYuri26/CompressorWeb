#ifndef DASHBOARD_H  // Se não definido DASHBOARD_H
#define DASHBOARD_H  // Define DASHBOARD_H

#include <ESP8266WebServer.h>  // Inclui a biblioteca ESP8266WebServer para manipulação de servidor web

// Declara a função setupDashboardPage, que configura a página do dashboard
// A função recebe uma referência a um objeto ESP8266WebServer como parâmetro
void setupDashboardPage(ESP8266WebServer& server);

#endif  // Finaliza a diretiva de pré-processador #ifndef/#define
