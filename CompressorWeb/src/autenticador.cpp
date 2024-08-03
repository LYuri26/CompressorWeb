#include "autenticador.h"
#include <ESPAsyncWebServer.h>

extern AsyncWebServer server;

bool userLoggedIn = false;
String loggedInUser = "";
String cookieHeader = "";

void handleLogin(AsyncWebServerRequest *request) {
    String username;
    String password;

    if (request->hasParam("username")) {
        username = request->getParam("username")->value();
    } else {
        Serial.println("Parâmetro 'username' não encontrado.");
    }

    if (request->hasParam("password")) {
        password = request->getParam("password")->value();
    } else {
        Serial.println("Parâmetro 'password' não encontrado.");
    }

    Serial.println("Tentando login com usuário: " + username + " e senha: " + password);

    if (username == "admin" && password == "admin123") {
        if (!userLoggedIn) {
            userLoggedIn = true;
            loggedInUser = username;
            Serial.println("Login bem-sucedido para o usuário: " + username);
            request->redirect("/dashboard");
        } else {
            Serial.println("Tentativa de login para o usuário já logado: " + username);
            request->redirect("/usuario-ja-logado");
        }
    } else {
        Serial.println("Falha de login para o usuário: " + username);
        request->redirect("/?login_failed=true");
    }
}

void handleLogout(AsyncWebServerRequest *request) {
    if (userLoggedIn) {
        Serial.println("Logout do usuário: " + loggedInUser);
        userLoggedIn = false;
        loggedInUser = "";
        AsyncResponseStream *response = request->beginResponseStream("text/plain");
        response->addHeader("Set-Cookie", "session_id=; expires=Thu, 01 Jan 1970 00:00:00 GMT; Path=/; HttpOnly");
        response->addHeader("Location", "/");
        response->setCode(302);
        request->send(response);
    } else {
        Serial.println("Tentativa de logout quando nenhum usuário está logado.");
        request->redirect("/");
    }
}
