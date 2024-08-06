#include <ESPAsyncWebServer.h>
#include "index.h"

// -------------------------------------------------------------------------
// Função para configurar a página de login
// -------------------------------------------------------------------------

/**
 * Configura a rota para a página de login no servidor Web.
 *
 * @param server A instância do servidor web assíncrono.
 */
void setupIndexPage(AsyncWebServer& server) {
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        // Define o conteúdo HTML da página de login
        String html = R"rawliteral(
            <!DOCTYPE html>
            <html lang="pt-br">
            <head>
                <meta charset="UTF-8">
                <meta name="viewport" content="width=device-width, initial-scale=1.0">
                <title>Login</title>
                <!-- Inclui o Bootstrap para estilização -->
                <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css">
                <style>
                    /* Estilo da página */
                    body {
                        font-family: Arial, sans-serif; /* Define a fonte padrão da página */
                        background-color: #f8f9fa; /* Cor de fundo da página */
                        height: 100vh; /* Altura da página igual à altura da viewport */
                        display: flex; /* Usa flexbox para centralizar o conteúdo */
                        justify-content: center; /* Alinha horizontalmente no centro */
                        align-items: center; /* Alinha verticalmente no centro */
                        margin: 0; /* Remove margens padrão */
                        padding: 0; /* Remove o preenchimento padrão */
                    }
                    .login-container {
                        background-color: #ffffff; /* Cor de fundo do container de login */
                        padding: 20px; /* Espaçamento interno do container */
                        border-radius: 5px; /* Bordas arredondadas */
                        box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); /* Sombra do container */
                        width: 100%; /* Largura total do container */
                        max-width: 400px; /* Largura máxima do container */
                        text-align: center; /* Alinha o texto ao centro */
                    }
                    .login-title {
                        font-size: 24px; /* Tamanho da fonte do título */
                        margin-bottom: 20px; /* Espaçamento abaixo do título */
                        color: #007bff; /* Cor do título */
                    }
                    .footer {
                        position: fixed; /* Fixa o rodapé na parte inferior */
                        bottom: 0; /* Posiciona o rodapé na parte inferior */
                        width: 100%; /* Largura total do rodapé */
                        background-color: #007bff; /* Cor de fundo do rodapé */
                        color: white; /* Cor do texto do rodapé */
                        text-align: center; /* Alinha o texto ao centro */
                        padding: 10px 0; /* Espaçamento interno do rodapé */
                        font-size: 14px; /* Tamanho da fonte do rodapé */
                    }
                    .text-danger {
                        color: #dc3545; /* Cor do texto de erro */
                    }
                </style>
            </head>
            <body>
                <div class="login-container">
                    <h2 class="login-title">Faça o Login</h2>
                    <form action="/login" method="post" onsubmit="storeLoginData(event)">
                        <div class="form-group">
                            <input type="text" name="username" class="form-control" placeholder="Usuário" required>
                        </div>
                        <div class="form-group">
                            <input type="password" name="password" class="form-control" placeholder="Senha" required>
                        </div>
                        <button type="submit" class="btn btn-primary btn-block">Entrar</button>
                        <!-- Exibe mensagens de erro, se houver -->
                        <div class="text-danger mt-2">%ERROR_MESSAGE%</div>
                    </form>
                    <!-- Botões para navegação -->
                    <button onclick="window.location.href='/creditos'" class="btn btn-secondary btn-block mt-3">Créditos</button>
                    <button onclick="window.location.href='/wifigerenciamento'" class="btn btn-warning btn-block mt-3">Gerenciamento Wi-Fi</button>
                </div>
                <div class="footer">
                    <p>Aplicação desenvolvida pela Turma de Informática Para Internet Trilhas de Futuro 2024</p>
                    <p>Instrutor: Lenon Yuri</p>
                </div>
                <!-- Script para depuração de login -->
                <script>
                    /**
                     * Armazena os dados de login no console e envia o formulário.
                     *
                     * @param {Event} event O evento de envio do formulário.
                     */
                    function storeLoginData(event) {
                        event.preventDefault(); // Previne o envio padrão do formulário
                        var username = document.querySelector('input[name="username"]').value; // Obtém o valor do campo de usuário
                        var password = document.querySelector('input[name="password"]').value; // Obtém o valor do campo de senha
                        console.log('Tentando login com usuário: ' + username + ' e senha: ' + password); // Exibe os dados no console para depuração
                        event.target.submit(); // Envia o formulário após a depuração
                    }
                </script>
            </body>
            </html>
        )rawliteral";

        // Verifica se há um parâmetro de falha de login e ajusta a mensagem de erro
        String errorMessage;
        if (request->hasParam("login_failed")) {
            errorMessage = "Usuário ou senha incorretos. Tente novamente."; // Mensagem de erro se o login falhar
        } else {
            errorMessage = ""; // Sem mensagem de erro
        }
        
        // Substitui o placeholder da mensagem de erro no HTML
        html.replace("%ERROR_MESSAGE%", errorMessage);

        // Envia a resposta ao cliente com o conteúdo HTML
        request->send(200, "text/html", html);
    });
}
