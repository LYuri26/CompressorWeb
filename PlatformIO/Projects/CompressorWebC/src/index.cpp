#include "index.h"  // Inclui o cabeçalho para a configuração da página inicial
#include <WebServer.h>  // Inclui a biblioteca para criar um servidor web no ESP32

// Função para configurar a página inicial do servidor web
void setupIndexPage(WebServer& server) {
    // Cria uma string HTML para a página de login
    String html = R"(
        <!DOCTYPE html> <!-- Declara o tipo de documento como HTML5 -->
        <html lang="pt-br"> <!-- Define o idioma da página como português do Brasil -->
        <head>
            <meta charset="UTF-8"> <!-- Define a codificação de caracteres para UTF-8 -->
            <meta name="viewport" content="width=device-width, initial-scale=1.0"> <!-- Configura o viewport para dispositivos móveis -->
            <title>Login</title> <!-- Define o título da página que aparece na aba do navegador -->
            <!-- Inclui o Bootstrap para estilização da página -->
            <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css">
            <style>
                /* Estilos personalizados para a página */
                body {
                    font-family: Arial, sans-serif; /* Define a fonte do corpo da página */
                    background-color: #f8f9fa; /* Define a cor de fundo da página */
                    height: 100vh; /* Define a altura da página como 100% da altura da viewport */
                    display: flex; /* Usa flexbox para alinhar o conteúdo */
                    justify-content: center; /* Centraliza o conteúdo horizontalmente */
                    align-items: center; /* Centraliza o conteúdo verticalmente */
                    margin: 0; /* Remove a margem padrão */
                    padding: 0; /* Remove o preenchimento padrão */
                }

                .login-container {
                    background-color: #ffffff; /* Define a cor de fundo do contêiner de login */
                    padding: 20px; /* Define o preenchimento interno do contêiner */
                    border-radius: 5px; /* Adiciona bordas arredondadas ao contêiner */
                    box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); /* Adiciona uma sombra sutil ao contêiner */
                    width: 100%; /* Define a largura do contêiner como 100% */
                    max-width: 400px; /* Define a largura máxima do contêiner */
                    text-align: center; /* Alinha o texto no centro do contêiner */
                }

                .login-title {
                    font-size: 24px; /* Define o tamanho da fonte do título */
                    margin-bottom: 20px; /* Adiciona uma margem inferior ao título */
                    color: #007bff; /* Define a cor do texto do título */
                }

                .footer {
                    position: fixed; /* Faz o rodapé fixo na parte inferior da página */
                    bottom: 0; /* Define a posição inferior do rodapé */
                    width: 100%; /* Define a largura do rodapé como 100% */
                    background-color: #007bff; /* Define a cor de fundo do rodapé */
                    color: white; /* Define a cor do texto do rodapé */
                    text-align: center; /* Alinha o texto no centro do rodapé */
                    padding: 10px 0; /* Define o preenchimento interno do rodapé */
                    font-size: 14px; /* Define o tamanho da fonte do texto do rodapé */
                }
            </style>
        </head>
        <body>
            <div class="login-container"> <!-- Contêiner para o formulário de login -->
                <h2 class="login-title">Faça o Login</h2> <!-- Título do formulário de login -->
                <form action="/login" method="post"> <!-- Formulário de login que envia dados para a rota "/login" usando o método POST -->
                    <div class="form-group"> <!-- Grupo de campos do formulário -->
                        <input type="text" name="username" class="form-control" placeholder="Usuário" required> <!-- Campo para o nome de usuário -->
                    </div>
                    <div class="form-group"> <!-- Grupo de campos do formulário -->
                        <input type="password" name="password" class="form-control" placeholder="Senha" required> <!-- Campo para a senha -->
                    </div>
                    <button type="submit" class="btn btn-primary btn-block">Entrar</button> <!-- Botão para enviar o formulário -->
                    <div class="text-danger mt-2" id="error-message"></div> <!-- Div para exibir mensagens de erro -->
                </form>
                <button onclick="window.location.href='/creditos'" class="btn btn-secondary btn-block mt-3">Créditos</button> <!-- Botão para redirecionar para a página de créditos -->
            </div>
            <div class="footer"> <!-- Rodapé da página -->
                <p>Aplicação desenvolvida pela Turma de Informática Para Internet Trilhas de Futuro 2024</p> <!-- Texto de crédito no rodapé -->
                <p>Instrutor: Lenon Yuri</p> <!-- Texto de crédito no rodapé -->
            </div>

            <!-- Inclui bibliotecas JavaScript -->
            <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.5.1/jquery.min.js"></script> <!-- Inclui jQuery -->
            <script src="https://maxcdn.bootstrapcdn.com/bootstrap/4.5.2/js/bootstrap.min.js"></script> <!-- Inclui JavaScript do Bootstrap -->

            <script>
                const errorMessage = document.getElementById('error-message'); // Obtém o elemento para exibir a mensagem de erro
                const urlParams = new URLSearchParams(window.location.search); // Obtém os parâmetros da URL
                const loginFailed = urlParams.get('login_failed'); // Obtém o parâmetro 'login_failed' da URL

                if (loginFailed) {
                    errorMessage.textContent = 'Usuário ou senha incorretos. Tente novamente.'; // Exibe mensagem de erro se o login falhar
                }
            </script>
        </body>
        </html>
    )";

    // Configura a rota "/" para responder com a página HTML definida
    server.on("/", HTTP_GET, [html, &server]() {
        server.send(200, "text/html", html); // Envia a resposta com o código de status 200 (OK) e o conteúdo HTML
    });
}
