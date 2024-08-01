#include "index.h"  // Inclui o cabeçalho para a configuração da página inicial
#include <WebServer.h>  // Inclui a biblioteca para criar um servidor web no ESP32

// Função para configurar a página inicial do servidor web
void setupIndexPage(WebServer& server) {
    // Variável local para armazenar o HTML da página inicial
    String html = R"(
        <!DOCTYPE html> <!-- Define o tipo de documento como HTML5 -->
        <html lang="pt-br"> <!-- Define o idioma da página como português do Brasil -->
        <head>
            <meta charset="UTF-8"> <!-- Define a codificação de caracteres como UTF-8 -->
            <meta name="viewport" content="width=device-width, initial-scale=1.0"> <!-- Configura a visualização responsiva em dispositivos móveis -->
            <title>Login</title> <!-- Define o título da página que aparece na aba do navegador -->
            <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css"> <!-- Inclui o CSS do Bootstrap para estilização -->
            <style>
                /* Estilos personalizados para a página */
                body {
                    font-family: Arial, sans-serif; /* Define a fonte do corpo do documento */
                    background-color: #f8f9fa; /* Define a cor de fundo da página */
                    height: 100vh; /* Define a altura do corpo para 100% da altura da viewport */
                    display: flex; /* Define o layout como flexbox */
                    justify-content: center; /* Centraliza horizontalmente o conteúdo */
                    align-items: center; /* Centraliza verticalmente o conteúdo */
                    margin: 0; /* Remove a margem padrão do corpo */
                    padding: 0; /* Remove o padding padrão do corpo */
                }

                .login-container {
                    background-color: #ffffff; /* Define a cor de fundo do container de login */
                    padding: 20px; /* Adiciona padding ao redor do conteúdo do container */
                    border-radius: 5px; /* Adiciona bordas arredondadas ao container */
                    box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); /* Adiciona uma sombra suave ao container */
                    width: 100%; /* Define a largura do container para 100% do pai */
                    max-width: 400px; /* Define a largura máxima do container para 400 pixels */
                    text-align: center; /* Centraliza o texto dentro do container */
                }

                .login-title {
                    font-size: 24px; /* Define o tamanho da fonte do título de login */
                    margin-bottom: 20px; /* Adiciona uma margem inferior ao título */
                    color: #007bff; /* Define a cor do título */
                }

                .footer {
                    position: fixed; /* Define a posição do rodapé como fixa */
                    bottom: 0; /* Posiciona o rodapé no fundo da página */
                    width: 100%; /* Define a largura do rodapé para 100% da página */
                    background-color: #007bff; /* Define a cor de fundo do rodapé */
                    color: white; /* Define a cor do texto do rodapé */
                    text-align: center; /* Centraliza o texto do rodapé */
                    padding: 10px 0; /* Adiciona padding vertical ao rodapé */
                    font-size: 14px; /* Define o tamanho da fonte do texto do rodapé */
                }
            </style>
        </head>
        <body>
            <div class="login-container">
                <h2 class="login-title">Faça o Login</h2> <!-- Título da página de login -->
                <form action="/login" method="post"> <!-- Formulário de login que envia uma requisição POST para a rota "/login" -->
                    <div class="form-group">
                        <input type="text" name="username" class="form-control" placeholder="Usuário" required> <!-- Campo de entrada para o nome de usuário -->
                    </div>
                    <div class="form-group">
                        <input type="password" name="password" class="form-control" placeholder="Senha" required> <!-- Campo de entrada para a senha -->
                    </div>
                    <button type="submit" class="btn btn-primary btn-block">Entrar</button> <!-- Botão para submeter o formulário -->
                    <div class="text-danger mt-2" id="error-message"></div> <!-- Div para mensagens de erro -->
                </form>
                <button onclick="window.location.href='/credits'" class="btn btn-secondary btn-block mt-3">Créditos</button> <!-- Botão para navegar para a página de créditos -->
            </div>
            <div class="footer">
                <p>Aplicação desenvolvida pela Turma de Informática Para Internet Trilhas de Futuro 2024</p> <!-- Texto do rodapé com informações sobre o desenvolvimento da aplicação -->
                <p>Instrutor: Lenon Yuri</p> <!-- Texto do rodapé com o nome do instrutor -->
            </div>

            <!-- Inclusão dos scripts JavaScript do Bootstrap e jQuery -->
            <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.5.1/jquery.min.js"></script> <!-- Inclui a biblioteca jQuery -->
            <script src="https://maxcdn.bootstrapcdn.com/bootstrap/4.5.2/js/bootstrap.min.js"></script> <!-- Inclui o JavaScript do Bootstrap -->

            <script>
                // Script JavaScript para exibir mensagem de erro (opcional)
                const errorMessage = document.getElementById('error-message'); // Obtém a referência do elemento para a mensagem de erro
                const urlParams = new URLSearchParams(window.location.search); // Obtém os parâmetros da URL
                const loginFailed = urlParams.get('login_failed'); // Obtém o valor do parâmetro 'login_failed'

                if (loginFailed) {
                    errorMessage.textContent = 'Usuário ou senha incorretos. Tente novamente.'; // Define a mensagem de erro se o login falhar
                }
            </script>
        </body>
        </html>
    )";

    // Configuração da rota "/" para responder com o HTML criado
    server.on("/", HTTP_GET, [html, &server]() {
        server.send(200, "text/html", html);  // Envia a resposta HTTP com o HTML da página inicial
    });
}
