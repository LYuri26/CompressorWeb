#include "index.h"  // Inclui o cabeçalho para a configuração da página inicial
#include <WebServer.h>  // Inclui a biblioteca para criar um servidor web no ESP32

// Função para configurar a página inicial do servidor web
void setupIndexPage(WebServer& server) {
    String html = R"(
        <!DOCTYPE html>
        <html lang="pt-br">
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>Login</title>
            <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css">
            <style>
                /* Estilos personalizados para a página */
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

                .login-container {
                    background-color: #ffffff;
                    padding: 20px;
                    border-radius: 5px;
                    box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
                    width: 100%;
                    max-width: 400px;
                    text-align: center;
                }

                .login-title {
                    font-size: 24px;
                    margin-bottom: 20px;
                    color: #007bff;
                }

                .footer {
                    position: fixed;
                    bottom: 0;
                    width: 100%;
                    background-color: #007bff;
                    color: white;
                    text-align: center;
                    padding: 10px 0;
                    font-size: 14px;
                }
            </style>
        </head>
        <body>
            <div class="login-container">
                <h2 class="login-title">Faça o Login</h2>
                <form action="/login" method="post">
                    <div class="form-group">
                        <input type="text" name="username" class="form-control" placeholder="Usuário" required>
                    </div>
                    <div class="form-group">
                        <input type="password" name="password" class="form-control" placeholder="Senha" required>
                    </div>
                    <button type="submit" class="btn btn-primary btn-block">Entrar</button>
                    <div class="text-danger mt-2" id="error-message"></div>
                </form>
                <button onclick="window.location.href='/creditos'" class="btn btn-secondary btn-block mt-3">Créditos</button>
            </div>
            <div class="footer">
                <p>Aplicação desenvolvida pela Turma de Informática Para Internet Trilhas de Futuro 2024</p>
                <p>Instrutor: Lenon Yuri</p>
            </div>

            <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.5.1/jquery.min.js"></script>
            <script src="https://maxcdn.bootstrapcdn.com/bootstrap/4.5.2/js/bootstrap.min.js"></script>

            <script>
                const errorMessage = document.getElementById('error-message');
                const urlParams = new URLSearchParams(window.location.search);
                const loginFailed = urlParams.get('login_failed');

                if (loginFailed) {
                    errorMessage.textContent = 'Usuário ou senha incorretos. Tente novamente.';
                }
            </script>
        </body>
        </html>
    )";

    server.on("/", HTTP_GET, [html, &server]() {
        server.send(200, "text/html", html);
    });
}
