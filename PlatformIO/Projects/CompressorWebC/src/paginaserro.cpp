#include "paginaserro.h"

void setupErrorPages(WebServer &server)
{
    // Página de acesso inválido
    server.on("/acesso-invalido", HTTP_GET, [&server]()
    {
        server.sendHeader("Content-Type", "text/html");
        server.send(403, "text/html", R"rawliteral(
<!DOCTYPE html>
<html lang="pt-br">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Acesso Inválido</title>
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
        <h1>Acesso Inválido</h1>
        <p>Você não tem permissão para acessar esta página. Por favor, faça login para continuar.</p>
        <a href="/">Voltar à Página Inicial</a>
    </div>
</body>
</html>
)rawliteral"); });

    // Página 404 - Não Encontrada
    server.onNotFound([&server]()
    {
        server.sendHeader("Content-Type", "text/html");
        server.send(404, "text/html", R"rawliteral(
<!DOCTYPE html>
<html lang="pt-br">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Página Não Encontrada</title>
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
        <h1>Página Não Encontrada</h1>
        <p>Essa página não existe. Verifique o URL ou volte à página inicial.</p>
        <a href="/">Voltar à Página Inicial</a>
    </div>
</body>
</html>
)rawliteral"); });

    // Adicione outras páginas de erro se necessário
}

// Função para enviar a página de "Usuário Já Logado"
void sendAlreadyLoggedInPage(WebServer &server)
{
    server.sendHeader("Content-Type", "text/html");
    server.send(403, "text/html", R"rawliteral(
<!DOCTYPE html>
<html lang="pt-br">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Usuário Já Logado</title>
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
        <h1>Usuário Já Logado</h1>
        <p>Você já está logado. Não é necessário fazer login novamente.</p>
        <a href="/dashboard">Ir para o Dashboard</a>
    </div>
</body>
</html>
)rawliteral");
}

// Função para enviar a página de "Credenciais Inválidas"
void sendInvalidCredentialsPage(WebServer &server)
{
    server.sendHeader("Content-Type", "text/html");
    server.send(401, "text/html", R"rawliteral(
<!DOCTYPE html>
<html lang="pt-br">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Credenciais Inválidas</title>
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
        <h1>Credenciais Inválidas</h1>
        <p>Nome de usuário ou senha incorretos. Tente novamente.</p>
        <a href="/">Voltar ao Login</a>
    </div>
</body>
</html>
)rawliteral");
}

// Função para enviar a página de "Erro de Logout"
void sendLogoutErrorPage(WebServer &server)
{
    server.sendHeader("Content-Type", "text/html");
    server.send(403, "text/html", R"rawliteral(
<!DOCTYPE html>
<html lang="pt-br">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Erro de Logout</title>
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
        <h1>Erro de Logout</h1>
        <p>Você não está logado. Não é possível realizar logout.</p>
        <a href="/">Voltar à Página Inicial</a>
    </div>
</body>
</html>
)rawliteral");
}
