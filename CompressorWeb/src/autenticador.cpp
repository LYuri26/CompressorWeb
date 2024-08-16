#include "autenticador.h" // Inclui o cabeçalho para a funcionalidade de autenticação

// -------------------------------------------------------------------------
// Variáveis Globais
// -------------------------------------------------------------------------

String sessionId = "";              // ID da sessão atual do usuário, inicializado como uma string vazia
bool userLoggedIn = false;          // Status de login do usuário, inicializado como falso
String loggedInUser = "";           // Nome do usuário que está logado, inicializado como uma string vazia
unsigned long lastActivityTime = 0; // Tempo da última atividade do usuário, inicializado como 0

const unsigned long SESSION_TIMEOUT = 5 * 60 * 1000; // Tempo de expiração da sessão (5 minutos em milissegundos)

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
    // Cria um novo ID de sessão com base no tempo atual desde que o dispositivo foi ligado (em milissegundos), convertido para hexadecimal.
    String newSessionId = String(millis(), HEX);
    Serial.println("Gerando novo sessionId: " + newSessionId); // Imprime o novo ID de sessão para depuração
    return newSessionId;                                       // Retorna o novo ID de sessão
}

/**
 * Verifica se o usuário está autenticado com base no cookie de sessão.
 *
 * @param request Ponteiro para o pedido HTTP.
 * @return true se o usuário estiver autenticado, false caso contrário.
 */
bool isAuthenticated(AsyncWebServerRequest *request)
{
    // Verifica se o pedido HTTP contém o cabeçalho "Cookie"
    if (request->hasHeader("Cookie"))
    {
        String cookie = request->header("Cookie");        // Obtém o valor do cabeçalho "Cookie"
        int sessionIndex = cookie.indexOf("session_id="); // Encontra a posição de "session_id=" no cookie
        if (sessionIndex != -1)                           // Verifica se "session_id=" foi encontrado no cookie
        {
            // Extrai o valor do ID da sessão do cookie
            String sessionValue = cookie.substring(sessionIndex + 11);
            Serial.println("Cookie recebido: " + cookie);              // Imprime o valor do cookie para depuração
            Serial.println("ID de sessão do cookie: " + sessionValue); // Imprime o valor do ID da sessão do cookie para depuração

            // Compara o valor do ID da sessão do cookie com o ID da sessão atual
            if (sessionValue.equals(sessionId))
            {
                // Verifica se o tempo atual menos o tempo da última atividade é menor que o tempo de expiração da sessão
                unsigned long currentTime = millis();
                if (currentTime - lastActivityTime < SESSION_TIMEOUT)
                {
                    Serial.println("Sessão autenticada com sucesso."); // Imprime mensagem de sucesso para depuração
                    return true;                                       // Retorna verdadeiro se a sessão for válida
                }
                else
                {
                    Serial.println("Sessão expirou."); // Imprime mensagem de expiração da sessão para depuração
                }
            }
            else
            {
                Serial.println("ID de sessão do cookie não corresponde ao ID atual."); // Imprime mensagem de erro para depuração
            }
        }
        else
        {
            Serial.println("Cabeçalho de cookie não contém 'session_id'."); // Imprime mensagem de erro para depuração
        }
    }
    else
    {
        Serial.println("Cabeçalho de cookie não encontrado."); // Imprime mensagem de erro para depuração
    }
    return false; // Retorna falso se a sessão não for autenticada
}

/**
 * Atualiza o tempo da última atividade do usuário.
 */
void updateLastActivityTime()
{
    lastActivityTime = millis(); // Atualiza o tempo da última atividade para o valor atual de millis()
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

    // Obtém o parâmetro "username" do pedido HTTP, se disponível
    if (request->hasParam("username", true))
    {
        username = request->getParam("username", true)->value();
    }
    else
    {
        Serial.println("Parâmetro 'username' não encontrado."); // Imprime mensagem de erro para depuração
        request->redirect("/?login_failed=true");               // Redireciona para a página de login com erro
        return;
    }

    // Obtém o parâmetro "password" do pedido HTTP, se disponível
    if (request->hasParam("password", true))
    {
        password = request->getParam("password", true)->value();
    }
    else
    {
        Serial.println("Parâmetro 'password' não encontrado."); // Imprime mensagem de erro para depuração
        request->redirect("/?login_failed=true");               // Redireciona para a página de login com erro
        return;
    }

    Serial.println("Tentando login com usuário: " + username + " e senha: " + password); // Imprime tentativa de login para depuração

    // Verifica se o usuário e senha correspondem aos valores esperados
    if (username == "admin" && password == "admin123")
    {
        // Verifica se o usuário não está logado
        if (!userLoggedIn)
        {
            userLoggedIn = true;                                              // Marca o usuário como logado
            loggedInUser = username;                                          // Define o nome do usuário logado
            sessionId = generateSessionId();                                  // Gera um novo ID de sessão
            updateLastActivityTime();                                         // Atualiza o tempo da última atividade
            Serial.println("Login bem-sucedido para o usuário: " + username); // Imprime mensagem de sucesso para depuração
            Serial.println("ID de sessão definido: " + sessionId);            // Imprime o ID de sessão para depuração

            // Cria uma resposta HTTP de redirecionamento para a página de dashboard
            AsyncWebServerResponse *response = request->beginResponse(302, "text/plain", "");
            response->addHeader("Set-Cookie", "session_id=" + sessionId + "; Path=/; HttpOnly"); // Define o cookie de sessão
            response->addHeader("Location", "/dashboard");                                       // Define o cabeçalho de redirecionamento para o dashboard
            request->send(response);                                                             // Envia a resposta HTTP
        }
        else
        {
            Serial.println("Tentativa de login para o usuário já logado: " + username); // Imprime mensagem de erro para depuração
            request->redirect("/usuario-ja-logado");                                    // Redireciona para a página de usuário já logado
        }
    }
    else
    {
        Serial.println("Falha de login para o usuário: " + username); // Imprime mensagem de falha para depuração
        request->redirect("/?login_failed=true");                     // Redireciona para a página de login com erro
    }
}

/**
 * Processa a solicitação de logout do usuário.
 *
 * @param request Ponteiro para o pedido HTTP.
 */
void handleLogout(AsyncWebServerRequest *request)
{
    // Verifica se o usuário está logado
    if (userLoggedIn)
    {
        Serial.println("Logout do usuário: " + loggedInUser); // Imprime mensagem de logout para depuração
        userLoggedIn = false;                                 // Marca o usuário como não logado
        loggedInUser = "";                                    // Limpa o nome do usuário logado
        sessionId = "";                                       // Limpa o ID de sessão
        Serial.println("ID de sessão removido.");             // Imprime mensagem de sessão removida para depuração

        // Cria uma resposta HTTP de redirecionamento para a página inicial
        AsyncWebServerResponse *response = request->beginResponse(302, "text/plain", "");
        response->addHeader("Set-Cookie", "session_id=; expires=Thu, 01 Jan 1970 00:00:00 GMT; Path=/; HttpOnly"); // Define um cookie expirado para remover a sessão
        response->addHeader("Location", "/");                                                                      // Define o cabeçalho de redirecionamento para a página inicial
        request->send(response);                                                                                   // Envia a resposta HTTP
    }
    else
    {
        Serial.println("Tentativa de logout quando nenhum usuário está logado."); // Imprime mensagem de erro para depuração
        request->redirect("/");                                                   // Redireciona para a página inicial
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
    Serial.println("Redirecionando para a página inicial, usuário não autenticado."); // Imprime mensagem de redirecionamento para depuração
    request->redirect("/");                                                           // Redireciona para a página inicial
}

/**
 * Processa o acesso à página do dashboard.
 *
 * @param request Ponteiro para o pedido HTTP.
 */
void handleDashboard(AsyncWebServerRequest *request)
{
    // Verifica se o usuário está autenticado
    if (isAuthenticated(request))
    {
        updateLastActivityTime();                                    // Atualiza o tempo da última atividade
        Serial.println("Acesso ao dashboard concedido.");            // Imprime mensagem de acesso concedido para depuração
        request->send(200, "text/plain", "Bem-vindo ao Dashboard!"); // Envia resposta HTTP com status 200 e mensagem de boas-vindas
    }
    else
    {
        Serial.println("Acesso ao dashboard negado."); // Imprime mensagem de acesso negado para depuração
        notAuthenticated(request);                     // Redireciona para a página inicial
    }
}
