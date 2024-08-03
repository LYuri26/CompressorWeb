#include "autenticador.h"
#include <ESPAsyncWebServer.h>

extern AsyncWebServer server;

bool userLoggedIn = false;
String loggedInUser = "";
String cookieHeader = ""; // Para armazenar o cookie

void handleLogin(AsyncWebServerRequest *request)
{
    String username;
    String password;

    if (request->hasParam("username")) {
        username = request->getParam("username")->value();
    }

    if (request->hasParam("password")) {
        password = request->getParam("password")->value();
    }

    // Você pode armazenar o cookie aqui, por exemplo
    if (request->hasHeader("Cookie")) {
        cookieHeader = request->header("Cookie").c_str(); // Correção aqui
    } else {
        cookieHeader = "";
    }

    if (username == "admin" && password == "admin123")
    {
        if (!userLoggedIn)
        {
            userLoggedIn = true;
            loggedInUser = username;
            request->redirect("/dashboard");
            Serial.println("Login successful");
            Serial.printf("Username: %s\n", username.c_str());
            Serial.printf("Password: %s\n", password.c_str());
            Serial.printf("Cookie received: %s\n", cookieHeader.c_str());
            Serial.println("Cookie set: session_id=123456");
        }
        else
        {
            request->redirect("/usuario-ja-logado");
            Serial.println("User already logged in");
        }
    }
    else
    {
        request->redirect("/credenciais-invalidas");
        Serial.println("Invalid credentials");
        Serial.printf("Username: %s\n", username.c_str());
        Serial.printf("Password: %s\n", password.c_str());
    }
}

void handleLogout(AsyncWebServerRequest *request)
{
    if (userLoggedIn)
    {
        userLoggedIn = false;
        loggedInUser = "";
        // Para remover o cookie, não há suporte direto para definir cookies, 
        // você deve adicionar o cabeçalho manualmente na resposta
        AsyncResponseStream *response = request->beginResponseStream("text/plain");
        response->addHeader("Set-Cookie", "session_id=; expires=Thu, 01 Jan 1970 00:00:00 GMT; HttpOnly");
        response->addHeader("Location", "/");
        request->send(response);
    }
    else
    {
        request->redirect("/");
    }
    Serial.println("User logged out");
}
