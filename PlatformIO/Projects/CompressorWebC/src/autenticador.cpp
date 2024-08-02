#include "autenticador.h"

bool userLoggedIn = false;
String loggedInUser = "";

// Função para tratar o login do usuário
void handleLogin() {
    String username = server.arg("username");
    String password = server.arg("password");

    if (username == "admin" && password == "admin123") {
        if (!userLoggedIn) {
            userLoggedIn = true;
            loggedInUser = username;
            server.sendHeader("Location", "/dashboard");
            server.send(302, "text/plain", "");
        } else {
            sendAlreadyLoggedInPage();
        }
    } else {
        sendInvalidCredentialsPage();
    }
}