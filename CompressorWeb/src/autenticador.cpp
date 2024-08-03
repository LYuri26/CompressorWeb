#include "autenticador.h"

// Variáveis globais
String sessionId = ""; // ID de sessão atual
bool userLoggedIn = false;
String loggedInUser = "";

String generateSessionId() {
    // Simples geração de ID de sessão baseado em millis(). 
    String newSessionId = String(millis(), HEX);
    Serial.println("Gerando novo sessionId: " + newSessionId);
    return newSessionId;
}

bool isAuthenticated(AsyncWebServerRequest *request) {
    if (request->hasHeader("Cookie")) {
        String cookie = request->header("Cookie");
        int sessionIndex = cookie.indexOf("session_id=");
        if (sessionIndex != -1) {
            String sessionValue = cookie.substring(sessionIndex + 11);
            Serial.println("Cookie recebido: " + cookie);
            Serial.println("ID de sessão do cookie: " + sessionValue);
            if (sessionValue.equals(sessionId)) {
                Serial.println("Sessão autenticada com sucesso.");
                return true;
            } else {
                Serial.println("ID de sessão do cookie não corresponde ao ID atual.");
            }
        } else {
            Serial.println("Cabeçalho de cookie não contém 'session_id'.");
        }
    } else {
        Serial.println("Cabeçalho de cookie não encontrado.");
    }
    return false;
}

void handleLogin(AsyncWebServerRequest *request) {
    String username;
    String password;

    if (request->hasParam("username", true)) {
        username = request->getParam("username", true)->value();
    } else {
        Serial.println("Parâmetro 'username' não encontrado.");
        request->redirect("/?login_failed=true");
        return;
    }

    if (request->hasParam("password", true)) {
        password = request->getParam("password", true)->value();
    } else {
        Serial.println("Parâmetro 'password' não encontrado.");
        request->redirect("/?login_failed=true");
        return;
    }

    Serial.println("Tentando login com usuário: " + username + " e senha: " + password);

    if (username == "admin" && password == "admin123") {
        if (!userLoggedIn) {
            userLoggedIn = true;
            loggedInUser = username;
            sessionId = generateSessionId();
            Serial.println("Login bem-sucedido para o usuário: " + username);
            Serial.println("ID de sessão definido: " + sessionId);
            AsyncWebServerResponse *response = request->beginResponse(302, "text/plain", "");
            response->addHeader("Set-Cookie", "session_id=" + sessionId + "; Path=/; HttpOnly");
            response->addHeader("Location", "/dashboard");
            request->send(response);
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
        sessionId = "";
        Serial.println("ID de sessão removido.");
        AsyncWebServerResponse *response = request->beginResponse(302, "text/plain", "");
        response->addHeader("Set-Cookie", "session_id=; expires=Thu, 01 Jan 1970 00:00:00 GMT; Path=/; HttpOnly");
        response->addHeader("Location", "/");
        request->send(response);
    } else {
        Serial.println("Tentativa de logout quando nenhum usuário está logado.");
        request->redirect("/");
    }
}

void notAuthenticated(AsyncWebServerRequest *request) {
    Serial.println("Redirecionando para a página inicial, usuário não autenticado.");
    request->redirect("/");
}

void handleDashboard(AsyncWebServerRequest *request) {
    if (isAuthenticated(request)) {
        Serial.println("Acesso ao dashboard concedido.");
        request->send(200, "text/plain", "Bem-vindo ao Dashboard!");
    } else {
        Serial.println("Acesso ao dashboard negado.");
        notAuthenticated(request);
    }
}
