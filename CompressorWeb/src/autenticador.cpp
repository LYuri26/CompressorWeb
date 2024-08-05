#include "autenticador.h"

// -------------------------------------------------------------------------
// Variáveis Globais
// -------------------------------------------------------------------------

String sessionId = "";           // ID da sessão atual do usuário
bool userLoggedIn = false;       // Status de login do usuário
String loggedInUser = "";        // Nome do usuário que está logado

// -------------------------------------------------------------------------
// Funções de Autenticação
// -------------------------------------------------------------------------

/**
 * Gera um novo ID de sessão baseado no valor atual de millis().
 *
 * @return String contendo o novo ID de sessão.
 */
String generateSessionId() {
    // Cria um ID de sessão usando o valor atual de millis() convertido para hexadecimal
    String newSessionId = String(millis(), HEX);
    Serial.println("Gerando novo sessionId: " + newSessionId);
    return newSessionId;
}

/**
 * Verifica se o usuário está autenticado com base no cookie de sessão.
 *
 * @param request Ponteiro para o pedido HTTP.
 * @return true se o usuário estiver autenticado, false caso contrário.
 */
bool isAuthenticated(AsyncWebServerRequest *request) {
    if (request->hasHeader("Cookie")) {
        // Obtém o valor do cookie de sessão
        String cookie = request->header("Cookie");
        int sessionIndex = cookie.indexOf("session_id=");
        if (sessionIndex != -1) {
            // Extrai o valor do ID de sessão do cookie
            String sessionValue = cookie.substring(sessionIndex + 11);
            Serial.println("Cookie recebido: " + cookie);
            Serial.println("ID de sessão do cookie: " + sessionValue);
            // Verifica se o ID de sessão do cookie corresponde ao ID atual
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

/**
 * Processa a solicitação de login do usuário.
 *
 * @param request Ponteiro para o pedido HTTP.
 */
void handleLogin(AsyncWebServerRequest *request) {
    String username;
    String password;

    // Obtém o parâmetro de nome de usuário
    if (request->hasParam("username", true)) {
        username = request->getParam("username", true)->value();
    } else {
        Serial.println("Parâmetro 'username' não encontrado.");
        request->redirect("/?login_failed=true");
        return;
    }

    // Obtém o parâmetro de senha
    if (request->hasParam("password", true)) {
        password = request->getParam("password", true)->value();
    } else {
        Serial.println("Parâmetro 'password' não encontrado.");
        request->redirect("/?login_failed=true");
        return;
    }

    Serial.println("Tentando login com usuário: " + username + " e senha: " + password);

    // Verifica credenciais
    if (username == "admin" && password == "admin123") {
        if (!userLoggedIn) {
            // Processa o login bem-sucedido
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

/**
 * Processa a solicitação de logout do usuário.
 *
 * @param request Ponteiro para o pedido HTTP.
 */
void handleLogout(AsyncWebServerRequest *request) {
    if (userLoggedIn) {
        // Processa o logout
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

// -------------------------------------------------------------------------
// Funções de Redirecionamento e Página de Dashboard
// -------------------------------------------------------------------------

/**
 * Redireciona o usuário para a página inicial se não estiver autenticado.
 *
 * @param request Ponteiro para o pedido HTTP.
 */
void notAuthenticated(AsyncWebServerRequest *request) {
    Serial.println("Redirecionando para a página inicial, usuário não autenticado.");
    request->redirect("/");
}

/**
 * Processa o acesso à página do dashboard.
 *
 * @param request Ponteiro para o pedido HTTP.
 */
void handleDashboard(AsyncWebServerRequest *request) {
    if (isAuthenticated(request)) {
        // Acesso concedido ao dashboard
        Serial.println("Acesso ao dashboard concedido.");
        request->send(200, "text/plain", "Bem-vindo ao Dashboard!");
    } else {
        // Acesso negado, redireciona para a página inicial
        Serial.println("Acesso ao dashboard negado.");
        notAuthenticated(request);
    }
}
