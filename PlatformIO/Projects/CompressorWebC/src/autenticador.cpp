#include <WebServer.h>
#include "autenticador.h"

// Instância do servidor
extern WebServer server;

// Variáveis globais
bool userLoggedIn = false;
String loggedInUser = "";

void handleLogin()
{
    String username = server.arg("username");
    String password = server.arg("password");

    if (username == "admin" && password == "admin123")
    {
        if (!userLoggedIn)
        {
            userLoggedIn = true;
            loggedInUser = username;
            server.sendHeader("Location", "/dashboard");
            server.send(302, "text/plain", "");
        }
        else
        {
            server.sendHeader("Location", "/usuario-ja-logado");
            server.send(302, "text/plain", "");
        }
    }
    else
    {
        server.sendHeader("Location", "/credenciais-invalidas");
        server.send(302, "text/plain", "");
    }
}

void handleLogout()
{
    if (userLoggedIn)
    {
        userLoggedIn = false;
        loggedInUser = "";
    }
    server.sendHeader("Location", "/");
    server.send(302, "text/plain", "");
}
