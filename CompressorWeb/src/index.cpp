#include <ESPAsyncWebServer.h>
#include "index.h"

void setupIndexPage(AsyncWebServer& server) {
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        String html = R"rawliteral(
            <!DOCTYPE html>
            <html lang="pt-br">
            <head>
                <meta charset="UTF-8">
                <meta name="viewport" content="width=device-width, initial-scale=1.0">
                <title>Login</title>
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
                    }
                    .login-container {
                        background-color: #ffffff;
                        padding: 30px;
                        border-radius: 8px;
                        box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
                        width: 100%;
                        max-width: 400px;
                        text-align: center;
                    }
                    .login-title {
                        font-size: 28px;
                        margin-bottom: 20px;
                        color: #007bff;
                        font-weight: 500;
                    }
                    .footer {
                        position: fixed;
                        bottom: 0;
                        width: 100%;
                        background-color: #007bff;
                        color: white;
                        text-align: center;
                        padding: 15px 0;
                        font-size: 14px;
                    }
                    .text-danger {
                        color: #dc3545;
                    }
                    .btn-secondary {
                        margin-top: 10px;
                    }
                </style>
            </head>
            <body>
                <div class="login-container">
                    <h2 class="login-title">Faça o Login</h2>
                    <form action="/login" method="post" onsubmit="storeLoginData(event)">
                        <div class="form-group">
                            <input type="text" name="username" class="form-control form-control-lg" placeholder="Usuário" required>
                        </div>
                        <div class="form-group">
                            <input type="password" name="password" class="form-control form-control-lg" placeholder="Senha" required>
                        </div>
                        <button type="submit" class="btn btn-primary btn-lg btn-block">Entrar</button>
                        <div class="text-danger mt-2">%ERROR_MESSAGE%</div>
                    </form>
                    <a href="/creditos" class="btn btn-secondary btn-lg btn-block mt-3">Créditos</a>
                    <a href="/configuracaowifi" class="btn btn-secondary btn-lg btn-block mt-3">Configuração WiFi</a>
                </div>
                <div class="footer">
                    <p>Aplicação desenvolvida pela Turma de Informática Para Internet Trilhas de Futuro 2024</p>
                    <p>Instrutor: Lenon Yuri</p>
                </div>
                <script>
                    // Função para armazenar dados de login no console
                    function storeLoginData(event) {
                        event.preventDefault();
                        var username = document.querySelector('input[name="username"]').value;
                        var password = document.querySelector('input[name="password"]').value;
                        console.log('Tentando login com usuário: ' + username + ' e senha: ' + password);
                        event.target.submit();
                    }
                </script>
            </body>
            </html>
        )rawliteral";

        // Verifica se há um parâmetro de falha de login e ajusta a mensagem de erro
        String errorMessage;
        if (request->hasParam("login_failed")) {
            errorMessage = "Usuário ou senha incorretos. Tente novamente.";
        } else {
            errorMessage = ""; // Sem mensagem de erro
        }
        
        // Substitui o placeholder da mensagem de erro no HTML
        html.replace("%ERROR_MESSAGE%", errorMessage);

        // Envia a resposta ao cliente
        request->send(200, "text/html", html);
    });
}
