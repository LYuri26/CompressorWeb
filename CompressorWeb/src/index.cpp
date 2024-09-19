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
<!DOCTYPE html>
<html lang="pt-br">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link rel="preconnect" href="https://fonts.googleapis.com">
<link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
<link href="https://fonts.googleapis.com/css2?family=Playfair+Display:ital,wght@0,400..900;1,400..900&display=swap" rel="stylesheet">

    <title>Login</title>
    <style>
        /* Estilos gerais da página */
body {
    font-family: Arial, sans-serif;
    background-color: #f0f2f5;
    display: flex; /* Usando flexbox */
    justify-content: center; /* Centraliza horizontalmente */
    align-items: center; /* Centraliza verticalmente */
    height: 100vh; /* O corpo terá altura de 100% da viewport */
    margin: 0;
    padding: 0; /* Remove o padding da página */
    transition: background-color 0.3s, color 0.3s;
}
        .dark-mode {
            background-color: #181818;
            color: #f0f0f0;
        }
        .high-contrast {
            background-color: #000;
            color: #fff;
        }
        .login-container {
            background-color: #ffffff;
            padding: 20px;
            border-radius: 8px;
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
            width: 100%;
            max-width: 400px;
            text-align: center;
            transition: background-color 0.3s, color 0.3s;
        }
        .dark-mode .login-container {
            background-color: #2a2a2a;
        }
        .high-contrast .login-container {
            background-color: #000;
        }
        .login-title {
            font-size: 24px;
            font-family: "Playfair Display", serif;
            margin-bottom: 20px;
            background-image: linear-gradient(to bottom, rgb(255, 221, 0), rgba(246, 15, 15, 0.975), rgb(249, 216, 28));
            background-clip: text;
            -webkit-background-clip: text; 
            -webkit-text-fill-color: transparent;
            color: rgb(242, 95, 22);
            font-weight: bold;
        }
        .dark-mode .login-title, .high-contrast .login-title {
            color: #1e90ff;
        }
        .form-group {
            margin-bottom: 15px;
        }
        .form-control {
            width: 100%;
            padding: 10px;
            border: 1px solid #ced4da;
            border-radius: 5px;
            box-sizing: border-box;
        }
        .dark-mode .form-control, .high-contrast .form-control {
            background-color: #333;
            color: #fff;
            border-color: #555;
        }
        .btn {
            display: inline-block;
            padding: 10px 20px;
            font-size: 16px;
            border-radius: 5px;
            cursor: pointer;
            border: none;
            color: #ffffff;
            text-align: center;
            transition: background-color 0.3s, transform 0.3s;
            width: 100%;
            margin: 5px 0;
        }
        
        .btn-primary {
            width: 100%;
            height: 38px;
            border: none;
            outline: none;
            color: #fff;
            background: #111;
            cursor: pointer;
            position: relative;
            z-index: 0;
            border-radius: 5px;
        }
        
        .btn-primary:before {
            content: '';
            background: linear-gradient(45deg, #ff0000, #ff7300, #fffb00, #48ff00, #00ffd5, #002bff, #7a00ff, #ff00c8, #ff0000);
            position: absolute;
            top: -2px;
            left:-2px;
            background-size: 600%;
            z-index: -1;
            filter: blur(7px);
            width: calc(100% + 4px);
            height: calc(100% + 4px);
            animation: glowing 20s linear infinite;
            opacity: 0;
            transition: opacity .3s ease-in-out;
            border-radius: 10px;
        }
        
        .btn-primary:active {
            color: #ffffff;
            font-weight: bold;
        }
        
        .btn-primary:active:after {
            background: transparent;
        }
        
        .btn-primary:hover:before {
            opacity: 1;
        }
        
        .btn-primary:after {
            z-index: -1;
            content: '';
            position: absolute;
            width: 100%;
            height: 100%;
            background: #111;
            left: 0;
            top: 0;
            border-radius: 10px;
        }
        
        @keyframes glowing {
            0% { background-position: 0 0; }
            50% { background-position: 400% 0; }
            100% { background-position: 0 0; }
        }
        
        .btn-secondary {
            background-color: #6c757d;
        }
        .btn-secondary:hover {
            background-color: #5a6268;
            transform: scale(1.05);
        }
        .btn-warning {
            background-color: #ffc107;
            color: #212529;
        }
        .btn-warning:hover {
            background-color: #e0a800;
            transform: scale(1.05);
        }
        /* Cores adicionais para garantir que cada botão tenha uma cor diferente */
        .btn-creditos {
            background-color: #ea0b0b; /* Verde */
        }
        .btn-creditos:hover {
            background-color: #7d0909;
            transform: scale(1.05);
        }
        .btn-gerenciamento {
            background-color: rgb(232, 106, 16); /* Azul claro */
        }
        .btn-gerenciamento:hover {
            background-color: rgb(149, 39, 5);
            transform: scale(1.05);
        }
        .btn-dark-mode {
            background-color: rgb(234, 161, 15); /* Roxo */
        }
        .btn-dark-mode:hover {
            background-color: #8e5204;
            transform: scale(1.05);
        }
        .btn-high-contrast {
            background-color: rgb(247, 214, 49); /* Rosa */
        }
        .btn-high-contrast:hover {
            background-color: #8c7807;
            transform: scale(1.05);
        }
        .text-danger {
            color: #dc3545;
            margin-top: 10px;
        }
        .footer {
            position: fixed;
            bottom: 0;
            width: 100%;
            background-color: #ee641a;
            color: white;
            text-align: center;
            padding: 10px 0;
            font-size: 14px;
        }
        .theme-toggle {
            margin: 10px 0;
        }
    </style>
</head>
<body>
    <div class="login-container" role="form" aria-labelledby="loginTitle">
        <h2 id="loginTitle" class="login-title">Faça o Login</h2>
        <form action="/login" method="post" onsubmit="storeLoginData(event)">
            <div class="form-group">
                <input type="text" name="username" class="form-control" placeholder="Usuário" aria-label="Usuário" required>
            </div>
            <div class="form-group">
                <input type="password" name="password" class="form-control" placeholder="Senha" aria-label="Senha" required>
            </div>
            <button type="submit" class="btn btn-primary" type="button" aria-label="Entrar">Entrar</button>
            <div class="text-danger">%ERROR_MESSAGE%</div>
        </form>
        <button onclick="window.location.href='/creditos'" class="btn btn-creditos" aria-label="Ir para créditos">Créditos</button>
        <button onclick="window.location.href='/wifigerenciamento'" class="btn btn-gerenciamento" aria-label="Ir para gerenciamento Wi-Fi">Gerenciamento Wi-Fi</button>
        
        <div class="theme-toggle">
            <button onclick="toggleDarkMode()" class="btn btn-dark-mode" aria-label="Ativar modo noturno">Modo Noturno</button>
            <button onclick="toggleHighContrast()" class="btn btn-high-contrast" aria-label="Ativar modo alto contraste">Alto Contraste</button>
        </div>
    </div>
    <div class="footer" role="contentinfo">
        <p>Aplicação desenvolvida pela Turma de Informática Para Internet Trilhas de Futuro 2024</p>
        <p>Instrutor: Lenon Yuri</p>
    </div>

    <script>
        function storeLoginData(event) {
            event.preventDefault();
            var username = document.querySelector('input[name="username"]').value;
            var password = document.querySelector('input[name="password"]').value;
            console.log('Tentando login com usuário: ' + username + ' e senha: ' + password);
            event.target.submit();
        }

        function toggleDarkMode() {
            document.body.classList.toggle('dark-mode');
        }

        function toggleHighContrast() {
            document.body.classList.toggle('high-contrast');
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
