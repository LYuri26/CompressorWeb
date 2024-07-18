#include "index.h"

void setupIndexPage(ESP8266WebServer& server) {
    // Variável local para o HTML da página inicial
    String html = R"(
        <!DOCTYPE html>
        <html lang="pt-br">
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>Login</title>
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

                .input-group {
                    margin-bottom: 15px;
                }

                .input-group input {
                    width: 100%;
                    padding: 10px;
                    border: 1px solid #ccc;
                    border-radius: 4px;
                    box-sizing: border-box;
                    font-size: 16px;
                }

                .btn-login {
                    background-color: #007bff;
                    color: white;
                    border: none;
                    padding: 10px 20px;
                    text-align: center;
                    text-decoration: none;
                    display: inline-block;
                    font-size: 16px;
                    border-radius: 4px;
                    cursor: pointer;
                    transition: background-color 0.3s ease;
                }

                .btn-login:hover {
                    background-color: #0056b3;
                }

                .error-message {
                    color: red;
                    font-size: 14px;
                    margin-top: 10px;
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
                    <div class="input-group">
                        <input type="text" name="username" placeholder="Usuário" required>
                    </div>
                    <div class="input-group">
                        <input type="password" name="password" placeholder="Senha" required>
                    </div>
                    <button type="submit" class="btn-login">Entrar</button>
                    <div class="error-message" id="error-message"></div>
                </form>
            </div>
            <div class="footer">
                <p>Aplicação desenvolvida pela Turma de Informática Para Internet Trilhas de Futuro 2024</p>
                <p>Instrutor: Lenon Yuri</p>
            </div>

            <script>
                // Script JavaScript para exibir mensagem de erro (opcional)
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

    // Configuração da rota "/" para responder com o HTML criado
    server.on("/", HTTP_GET, [html, &server]() {
        server.send(200, "text/html", html);
    });
}
