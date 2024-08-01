#include "autenticador.h"

// Variáveis globais
bool userLoggedIn = false; // Variável para verificar se um usuário está logado
String loggedInUser = "";  // Armazena o nome do usuário logado

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
            // Enviar a página informando que um usuário já está logado
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
        <p>Há um usuário atualmente logado. Por favor, tente novamente mais tarde.</p>
        <a href="/">Acessar Tela Inícial</a>
    </div>
</body>
</html>
)rawliteral");
        }
    }
    else
    {
        // Enviar a página de credenciais inválidas
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
        <h1>Login Falhou</h1>
        <p>Credenciais inválidas. Por favor, verifique seu nome de usuário e senha.</p>
        <a href="/">Voltar ao Login</a>
    </div>
</body>
</html>
)rawliteral");
    }
}

void handleLogout()
{
    if (userLoggedIn)
    {
        userLoggedIn = false;
        loggedInUser = "";
        server.sendHeader("Location", "/");
        server.send(302, "text/plain", "");
    }
    else
    {
        server.sendHeader("Location", "/");
        server.send(302, "text/plain", "");
    }
}
