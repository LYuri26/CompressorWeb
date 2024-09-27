#include "autenticador.h" // Inclui o cabeçalho para a funcionalidade de autenticação

// -------------------------------------------------------------------------
// Variáveis Globais
// -------------------------------------------------------------------------

String sessionId = "";              // ID da sessão atual do usuário, inicializado como uma string vazia
bool userLoggedIn = false;          // Status de login do usuário, inicializado como falso
String loggedInUser = "";           // Nome do usuário que está logado, inicializado como uma string vazia
unsigned long lastActivityTime = 0; // Tempo da última atividade do usuário, inicializado como 0

const unsigned long SESSION_TIMEOUT = 2 * 60 * 1000; // Tempo de expiração da sessão (1 minuto em milissegundos)

// -------------------------------------------------------------------------
// Funções de Autenticação
// -------------------------------------------------------------------------

/**
 * Gera um novo ID de sessão baseado no valor atual de millis().
 *
 * @return String contendo o novo ID de sessão.
 */
String generateSessionId()
{
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
bool isAuthenticated(AsyncWebServerRequest *request)
{
    if (request->hasHeader("Cookie"))
    {
        String cookie = request->header("Cookie");
        int sessionIndex = cookie.indexOf("session_id=");
        if (sessionIndex != -1)
        {
            String sessionValue = cookie.substring(sessionIndex + 11);
            Serial.println("Cookie recebido: " + cookie);
            Serial.println("ID de sessão do cookie: " + sessionValue);

            if (sessionValue.equals(sessionId))
            {
                unsigned long currentTime = millis();
                if (currentTime - lastActivityTime < SESSION_TIMEOUT)
                {
                    Serial.println("Sessão autenticada com sucesso.");
                    return true;
                }
                else
                {
                    Serial.println("Sessão expirou.");
                    handleLogout(request); // Desconecta o usuário quando a sessão expira
                }
            }
            else
            {
                Serial.println("ID de sessão do cookie não corresponde ao ID atual.");
            }
        }
        else
        {
            Serial.println("Cabeçalho de cookie não contém 'session_id'.");
        }
    }
    else
    {
        Serial.println("Cabeçalho de cookie não encontrado.");
    }
    return false;
}

/**
 * Atualiza o tempo da última atividade do usuário.
 */
void updateLastActivityTime()
{
    lastActivityTime = millis();
}

/**
 * Processa a solicitação de login do usuário.
 *
 * @param request Ponteiro para o pedido HTTP.
 */
void handleLogin(AsyncWebServerRequest *request)
{
    String username;
    String password;

    if (request->hasParam("username", true))
    {
        username = request->getParam("username", true)->value();
    }
    else
    {
        Serial.println("Parâmetro 'username' não encontrado.");
        request->redirect("/?login_failed=true");
        return;
    }

    if (request->hasParam("password", true))
    {
        password = request->getParam("password", true)->value();
    }
    else
    {
        Serial.println("Parâmetro 'password' não encontrado.");
        request->redirect("/?login_failed=true");
        return;
    }

    Serial.println("Tentando login com usuário: " + username + " e senha: " + password);

    // Verifica se as credenciais são "fazenda" e "fazenda123"
    if (username == "EnergizAR" && password == "EnergizAR123")
    {
        Serial.println("Logout automático devido ao uso de credenciais específicas.");
        handleLogout(request); // Chama a função de logout
        return;
    }

    if (username == "admin" && password == "admin123")
    {
        if (!userLoggedIn)
        {
            userLoggedIn = true;
            loggedInUser = username;
            sessionId = generateSessionId();
            updateLastActivityTime();
            Serial.println("Login bem-sucedido para o usuário: " + username);
            Serial.println("ID de sessão definido: " + sessionId);

            AsyncWebServerResponse *response = request->beginResponse(302, "text/plain", "");
            response->addHeader("Set-Cookie", "session_id=" + sessionId + "; Path=/; HttpOnly");
            response->addHeader("Location", "/dashboard");
            request->send(response);
        }
        else
        {
            Serial.println("Tentativa de login para o usuário já logado: " + username);
            request->redirect("/usuario-ja-logado");
        }
    }
    else
    {
        Serial.println("Falha de login para o usuário: " + username);
        request->redirect("/?login_failed=true");
    }
}

/**
 * Processa a solicitação de logout do usuário.
 *
 * @param request Ponteiro para o pedido HTTP.
 */
void handleLogout(AsyncWebServerRequest *request)
{
    if (userLoggedIn)
    {
        Serial.println("Logout do usuário: " + loggedInUser);
        userLoggedIn = false;
        loggedInUser = "";
        sessionId = "";
        Serial.println("ID de sessão removido.");

        AsyncWebServerResponse *response = request->beginResponse(302, "text/plain", "");
        response->addHeader("Set-Cookie", "session_id=; expires=Thu, 01 Jan 1970 00:00:00 GMT; Path=/; HttpOnly");
        response->addHeader("Location", "/");
        request->send(response);
    }
    else
    {
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
void notAuthenticated(AsyncWebServerRequest *request)
{
    Serial.println("Redirecionando para a página inicial, usuário não autenticado.");
    request->redirect("/");
}

/**
 * Processa o acesso à página do dashboard.
 *
 * @param request Ponteiro para o pedido HTTP.
 */
void handleDashboard(AsyncWebServerRequest *request)
{
    if (isAuthenticated(request))
    {
        updateLastActivityTime();
        Serial.println("Acesso ao dashboard concedido.");
        request->send(200, "text/plain", "Bem-vindo ao Dashboard!");
    }
    else
    {
        Serial.println("Acesso ao dashboard negado.");
        notAuthenticated(request);
    }
}