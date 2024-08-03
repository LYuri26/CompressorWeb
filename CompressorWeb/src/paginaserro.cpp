#include <WebServer.h>

// Função para gerar o HTML para páginas de erro
String generateErrorPage(const String& title, const String& message, const String& linkText, const String& linkHref)
{
    return R"rawliteral(
<!DOCTYPE html>
<html lang="pt-br">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>)rawliteral" + title + R"rawliteral(</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background-color: #f4f4f4;
        }
        .container {
            text-align: center;
            background-color: #fff;
            padding: 20px;
            border-radius: 8px;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
        }
        h1 {
            color: #e74c3c;
        }
        p {
            color: #555;
        }
        a {
            text-decoration: none;
            color: #3498db;
            font-weight: bold;
            padding: 10px 20px;
            border: 2px solid #3498db;
            border-radius: 5px;
            display: inline-block;
        }
        a:hover {
            text-decoration: underline;
            background-color: #f0f0f0;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>)rawliteral" + title + R"rawliteral(</h1>
        <p>)rawliteral" + message + R"rawliteral(</p>
        <a href=")rawliteral" + linkHref + R"rawliteral(">)rawliteral" + linkText + R"rawliteral(</a>
    </div>
</body>
</html>
)rawliteral";
}

// Configura as rotas para as páginas de erro
void setupUsuarioJaLogadoPage(WebServer& server)
{
    server.on("/usuario-ja-logado", HTTP_GET, [&server]() {
        server.sendHeader("Content-Type", "text/html");
        server.send(403, "text/html", generateErrorPage("Usuário Já Logado", 
            "Há um usuário atualmente logado. Por favor, tente novamente mais tarde.", 
            "Acessar Tela Inicial", "/"));
    });
}

void setupCredenciaisInvalidasPage(WebServer& server)
{
    server.on("/credenciais-invalidas", HTTP_GET, [&server]() {
        server.sendHeader("Content-Type", "text/html");
        server.send(401, "text/html", generateErrorPage("Credenciais Inválidas", 
            "Credenciais inválidas. Por favor, verifique seu nome de usuário e senha.", 
            "Voltar ao Login", "/"));
    });
}

void setupAcessoInvalidoPage(WebServer& server)
{
    server.on("/acesso-invalido", HTTP_GET, [&server]() {
        server.sendHeader("Content-Type", "text/html");
        server.send(403, "text/html", generateErrorPage("Acesso Inválido", 
            "Você não tem permissão para acessar esta página. Por favor, faça login para continuar.", 
            "Voltar à Página Inicial", "/"));
    });
}

void setupNotFoundPage(WebServer& server)
{
    server.onNotFound([&server]() {
        server.sendHeader("Content-Type", "text/html");
        server.send(404, "text/html", generateErrorPage("Página Não Encontrada", 
            "Essa página não existe. Verifique o URL ou volte à página inicial.", 
            "Voltar à Página Inicial", "/"));
    });
}
