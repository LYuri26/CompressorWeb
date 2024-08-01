#include <WebServer.h> // Inclua a biblioteca WebServer
#include "logado.h" // Inclua o cabeçalho correspondente

extern WebServer server; // Declare o servidor externamente
extern bool isLoggedIn; // Declare a variável isLoggedIn externamente

void handleUserAlreadyLoggedIn()
{
    String html = R"(
        <!DOCTYPE html>
        <html lang="pt-br">
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>Usuário Já Logado</title>
            <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css">
            <style>
                body {
                    font-family: Arial, sans-serif;
                    background-color: #f8f9fa;
                    height: 100vh;
                    display: flex;
                    justify-content: center;
                    align-items: center;
                    margin: 0;
                    padding: 0;
                }
                .container {
                    background-color: #ffffff;
                    padding: 20px;
                    border-radius: 5px;
                    box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
                    width: 100%;
                    max-width: 400px;
                    text-align: center;
                }
                .btn-back {
                    margin-top: 20px;
                }
            </style>
        </head>
        <body>
            <div class="container">
                <h2>Usuário Já Logado</h2>
                <p>Já existe um usuário logado. Por favor, faça logout antes de tentar novamente.</p>
                <a href="/" class="btn btn-primary btn-back">Voltar ao Login</a>
            </div>
        </body>
        </html>
    )"; // Adicione o delimitador correto aqui

    server.send(401, "text/html", html);
}
