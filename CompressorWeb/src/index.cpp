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
    <title>Login</title>
    <style>
        /* Estilos gerais da página */
        body {
            font-family: Arial, sans-serif;
            background-color: #f0f2f5;
            height: 100vh;
            display: flex;
            justify-content: center;
            align-items: center;
            margin: 0;
            padding: 60px; /* Remove o padding da página */
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
            margin-bottom: 20px;
            color: #007bff;
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
            background-color: #007bff;
        }
        .btn-primary:hover {
            background-color: #0056b3;
            transform: scale(1.05);
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
            background-color: #28a745; /* Verde */
        }
        .btn-creditos:hover {
            background-color: #218838;
            transform: scale(1.05);
        }
        .btn-gerenciamento {
            background-color: #17a2b8; /* Azul claro */
        }
        .btn-gerenciamento:hover {
            background-color: #138496;
            transform: scale(1.05);
        }
        .btn-dark-mode {
            background-color: #6f42c1; /* Roxo */
        }
        .btn-dark-mode:hover {
            background-color: #5a3796;
            transform: scale(1.05);
        }
        .btn-high-contrast {
            background-color: #e83e8c; /* Rosa */
        }
        .btn-high-contrast:hover {
            background-color: #c72571;
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
            background-color: #007bff;
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
            <button type="submit" class="btn btn-primary" aria-label="Entrar no sistema">Entrar</button>
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
