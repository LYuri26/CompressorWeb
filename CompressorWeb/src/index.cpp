#include <ESPAsyncWebServer.h> // Inclui a biblioteca para criar um servidor web assíncrono
#include "index.h"             // Inclui o cabeçalho que pode definir variáveis ou funções específicas para a página de login

// -------------------------------------------------------------------------
// Função para configurar a página de login
// -------------------------------------------------------------------------

/**
 * Configura a rota para a página de login no servidor Web.
 *
 * @param server A instância do servidor web assíncrono.
 */
void setupIndexPage(AsyncWebServer &server)
{
    // Define a rota para a URL raiz ("/") do servidor web
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        // Define o conteúdo HTML da página de login usando um literal RAW
        String html = R"rawliteral(
<!DOCTYPE html> <!-- Declara o tipo de documento como HTML5 -->
<html lang="pt-br"> <!-- Define o idioma da página como português do Brasil -->
<head>
    <meta charset="UTF-8"> <!-- Define o conjunto de caracteres como UTF-8 para suportar caracteres especiais -->
    <meta name="viewport" content="width=device-width, initial-scale=1.0"> <!-- Ajusta a escala da página para dispositivos móveis -->
    <title>Login</title> <!-- Define o título da aba do navegador -->
    <style>
        /* Estilos gerais da página */
        body {
            font-family: Arial, sans-serif; /* Define a fonte padrão para a página */
            background-color: #f0f2f5; /* Define a cor de fundo da página */
            height: 100vh; /* Define a altura da página como 100% da altura da viewport */
            display: flex; /* Usa Flexbox para alinhar o conteúdo */
            justify-content: center; /* Centraliza o conteúdo horizontalmente */
            align-items: center; /* Centraliza o conteúdo verticalmente */
            margin: 0; /* Remove a margem padrão */
            padding: 0; /* Remove o preenchimento padrão */
        }
        /* Estilos do container de login */
        .login-container {
            background-color: #ffffff; /* Define a cor de fundo do container de login */
            padding: 20px; /* Adiciona preenchimento interno ao container */
            border-radius: 8px; /* Adiciona bordas arredondadas ao container */
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1); /* Adiciona uma sombra leve ao container */
            width: 100%; /* Define a largura como 100% do container pai */
            max-width: 400px; /* Define a largura máxima do container */
            text-align: center; /* Centraliza o texto dentro do container */
        }
        .login-title {
            font-size: 24px; /* Define o tamanho da fonte do título */
            margin-bottom: 20px; /* Adiciona uma margem inferior ao título */
            color: #007bff; /* Define a cor do título */
        }
        .form-group {
            margin-bottom: 15px; /* Adiciona uma margem inferior aos grupos de formulários */
        }
        .form-control {
            width: 100%; /* Define a largura dos campos de entrada como 100% do container pai */
            padding: 10px; /* Adiciona preenchimento interno aos campos de entrada */
            border: 1px solid #ced4da; /* Define a borda dos campos de entrada */
            border-radius: 5px; /* Adiciona bordas arredondadas aos campos de entrada */
            box-sizing: border-box; /* Inclui a borda e o preenchimento no tamanho total do elemento */
        }
        .btn {
            display: inline-block; /* Define o botão como um bloco em linha */
            padding: 10px 20px; /* Adiciona preenchimento interno ao botão */
            font-size: 16px; /* Define o tamanho da fonte do botão */
            border-radius: 5px; /* Adiciona bordas arredondadas ao botão */
            cursor: pointer; /* Define o cursor como ponteiro ao passar sobre o botão */
            border: none; /* Remove a borda padrão do botão */
            color: #ffffff; /* Define a cor do texto do botão */
            text-align: center; /* Centraliza o texto dentro do botão */
            transition: background-color 0.3s, transform 0.3s; /* Adiciona uma transição suave para a cor de fundo e a transformação */
            width: 100%; /* Define a largura do botão como 100% do container pai */
            margin: 5px 0; /* Adiciona uma margem superior e inferior ao botão */
        }
        .btn-primary {
            background-color: #007bff; /* Define a cor de fundo primária do botão */
        }
        .btn-primary:hover {
            background-color: #0056b3; /* Define a cor de fundo ao passar o mouse sobre o botão */
            transform: scale(1.05); /* Aumenta o botão em 5% ao passar o mouse */
        }
        .btn-secondary {
            background-color: #6c757d; /* Define a cor de fundo secundária do botão */
        }
        .btn-secondary:hover {
            background-color: #5a6268; /* Define a cor de fundo ao passar o mouse sobre o botão */
            transform: scale(1.05); /* Aumenta o botão em 5% ao passar o mouse */
        }
        .btn-warning {
            background-color: #ffc107; /* Define a cor de fundo de aviso do botão */
            color: #212529; /* Define a cor do texto do botão */
        }
        .btn-warning:hover {
            background-color: #e0a800; /* Define a cor de fundo ao passar o mouse sobre o botão */
            transform: scale(1.05); /* Aumenta o botão em 5% ao passar o mouse */
        }
        .text-danger {
            color: #dc3545; /* Define a cor do texto de erro */
            margin-top: 10px; /* Adiciona uma margem superior ao texto de erro */
        }
        .footer {
            position: fixed; /* Fixar o rodapé na parte inferior da página */
            bottom: 0; /* Alinha o rodapé na parte inferior da página */
            width: 100%; /* Define a largura do rodapé como 100% da viewport */
            background-color: #007bff; /* Define a cor de fundo do rodapé */
            color: white; /* Define a cor do texto do rodapé */
            text-align: center; /* Centraliza o texto dentro do rodapé */
            padding: 10px 0; /* Adiciona preenchimento interno ao rodapé */
            font-size: 14px; /* Define o tamanho da fonte do rodapé */
        }
    </style>
</head>
<body>
    <div class="login-container"> <!-- Container para o formulário de login -->
        <h2 class="login-title">Faça o Login</h2> <!-- Título da página de login -->
        <form action="/login" method="post" onsubmit="storeLoginData(event)"> <!-- Formulário de login com método POST -->
            <div class="form-group">
                <input type="text" name="username" class="form-control" placeholder="Usuário" required> <!-- Campo para nome de usuário -->
            </div>
            <div class="form-group">
                <input type="password" name="password" class="form-control" placeholder="Senha" required> <!-- Campo para senha -->
            </div>
            <button type="submit" class="btn btn-primary">Entrar</button> <!-- Botão de envio do formulário -->
            <!-- Exibe mensagens de erro, se houver -->
            <div class="text-danger">%ERROR_MESSAGE%</div> <!-- Placeholder para mensagens de erro -->
        </form>
        <!-- Botões para navegação -->
        <button onclick="window.location.href='/creditos'" class="btn btn-secondary">Créditos</button> <!-- Botão para navegar para a página de créditos -->
        <button onclick="window.location.href='/wifigerenciamento'" class="btn btn-warning">Gerenciamento Wi-Fi</button> <!-- Botão para navegar para a página de gerenciamento Wi-Fi -->
    </div>
    <div class="footer"> <!-- Rodapé da página -->
        <p>Aplicação desenvolvida pela Turma de Informática Para Internet Trilhas de Futuro 2024</p> <!-- Mensagem sobre o desenvolvimento da aplicação -->
        <p>Instrutor: Lenon Yuri</p> <!-- Nome do instrutor -->
    </div>
    <script>
        /**
         * Armazena os dados de login no console e envia o formulário.
         *
         * @param {Event} event O evento de envio do formulário.
         */
        function storeLoginData(event) {
            event.preventDefault(); // Previne o envio padrão do formulário para realizar ações antes do envio
            var username = document.querySelector('input[name="username"]').value; // Obtém o valor do campo de nome de usuário
            var password = document.querySelector('input[name="password"]').value; // Obtém o valor do campo de senha
            console.log('Tentando login com usuário: ' + username + ' e senha: ' + password); // Exibe os dados de login no console para depuração
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
        request->send(200, "text/html", html); }); // Responde ao pedido HTTP GET com o conteúdo HTML da página de login
}
