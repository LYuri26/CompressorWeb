#include "autenticador.h" // Inclui o cabeçalho onde as variáveis e funções de autenticação são declaradas

// Variáveis globais
bool userLoggedIn = false; // Variável global que indica se um usuário está logado. Inicialmente, está definida como falsa.
String loggedInUser = "";  // Variável global que armazena o nome do usuário logado. Inicialmente, está vazia.

void handleLogin()
{
    // Obtém os parâmetros "username" e "password" da solicitação HTTP GET
    String username = server.arg("username");
    String password = server.arg("password");

    // Verifica se o nome de usuário e a senha fornecidos são válidos
    if (username == "admin" && password == "admin123")
    {
        // Verifica se não há nenhum usuário logado
        if (!userLoggedIn)
        {
            userLoggedIn = true; // Define a variável userLoggedIn como verdadeira, indicando que o usuário está logado
            loggedInUser = username; // Armazena o nome do usuário logado
            server.sendHeader("Location", "/dashboard"); // Define o cabeçalho da resposta HTTP para redirecionar para a página do dashboard
            server.send(302, "text/plain", ""); // Envia uma resposta HTTP com código 302 (Found) e um corpo vazio, para redirecionar o usuário
        }
        else
        {
            // Se já há um usuário logado, exibe uma mensagem de erro
            server.sendHeader("Content-Type", "text/html"); // Define o cabeçalho da resposta HTTP para indicar que o conteúdo é HTML
            server.send(403, "text/html", R"rawliteral(
<!DOCTYPE html>
<html lang="pt-br">
<head>
    <meta charset="UTF-8"> <!-- Define a codificação de caracteres como UTF-8 -->
    <meta name="viewport" content="width=device-width, initial-scale=1.0"> <!-- Configura o viewport para dispositivos móveis -->
    <title>Usuário Já Logado</title> <!-- Define o título da página -->
    <style>
        body {
            font-family: Arial, sans-serif; /* Define a fonte padrão do texto */
            display: flex; /* Usa Flexbox para centralizar o conteúdo */
            justify-content: center; /* Alinha o conteúdo horizontalmente ao centro */
            align-items: center; /* Alinha o conteúdo verticalmente ao centro */
            height: 100vh; /* Define a altura da página para 100% da altura da visualização */
            margin: 0; /* Remove a margem padrão ao redor da página */
            background-color: #f4f4f4; /* Define a cor de fundo da página */
        }
        .container {
            text-align: center; /* Centraliza o texto dentro do container */
            background-color: #fff; /* Define a cor de fundo do container */
            padding: 20px; /* Adiciona padding ao container para espaçamento interno */
            border-radius: 8px; /* Adiciona bordas arredondadas ao container */
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); /* Adiciona uma sombra ao container para efeito de profundidade */
        }
        h1 {
            color: #e74c3c; /* Define a cor do título */
        }
        p {
            color: #555; /* Define a cor do texto do parágrafo */
        }
        a {
            text-decoration: none; /* Remove o sublinhado do link */
            color: #3498db; /* Define a cor do texto do link */
            font-weight: bold; /* Define o texto do link como negrito */
            padding: 10px 20px; /* Adiciona padding ao link */
            border: 2px solid #3498db; /* Adiciona uma borda ao redor do link */
            border-radius: 5px; /* Adiciona bordas arredondadas ao link */
            display: inline-block; /* Faz o link se comportar como um bloco inline */
        }
        a:hover {
            text-decoration: underline; /* Adiciona um sublinhado ao passar o mouse sobre o link */
            background-color: #f0f0f0; /* Altera a cor de fundo ao passar o mouse sobre o link */
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Usuário Já Logado</h1> <!-- Título da página de erro -->
        <p>Há um usuário atualmente logado. Por favor, tente novamente mais tarde.</p> <!-- Mensagem de erro informando que já há um usuário logado -->
        <a href="/">Acessar Tela Inícial</a> <!-- Link para voltar à tela inicial -->
    </div>
</body>
</html>
)rawliteral"); // Envia o HTML como resposta com código 403 (Forbidden) para indicar que o acesso é proibido
        }
    }
    else
    {
        // Se as credenciais forem inválidas, exibe uma mensagem de erro
        server.sendHeader("Content-Type", "text/html"); // Define o cabeçalho da resposta HTTP para indicar que o conteúdo é HTML
        server.send(401, "text/html", R"rawliteral(
<!DOCTYPE html>
<html lang="pt-br">
<head>
    <meta charset="UTF-8"> <!-- Define a codificação de caracteres como UTF-8 -->
    <meta name="viewport" content="width=device-width, initial-scale=1.0"> <!-- Configura o viewport para dispositivos móveis -->
    <title>Credenciais Inválidas</title> <!-- Define o título da página -->
    <style>
        body {
            font-family: Arial, sans-serif; /* Define a fonte padrão do texto */
            display: flex; /* Usa Flexbox para centralizar o conteúdo */
            justify-content: center; /* Alinha o conteúdo horizontalmente ao centro */
            align-items: center; /* Alinha o conteúdo verticalmente ao centro */
            height: 100vh; /* Define a altura da página para 100% da altura da visualização */
            margin: 0; /* Remove a margem padrão ao redor da página */
            background-color: #f4f4f4; /* Define a cor de fundo da página */
        }
        .container {
            text-align: center; /* Centraliza o texto dentro do container */
            background-color: #fff; /* Define a cor de fundo do container */
            padding: 20px; /* Adiciona padding ao container para espaçamento interno */
            border-radius: 8px; /* Adiciona bordas arredondadas ao container */
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); /* Adiciona uma sombra ao container para efeito de profundidade */
        }
        h1 {
            color: #e74c3c; /* Define a cor do título */
        }
        p {
            color: #555; /* Define a cor do texto do parágrafo */
        }
        a {
            text-decoration: none; /* Remove o sublinhado do link */
            color: #3498db; /* Define a cor do texto do link */
            font-weight: bold; /* Define o texto do link como negrito */
            padding: 10px 20px; /* Adiciona padding ao link */
            border: 2px solid #3498db; /* Adiciona uma borda ao redor do link */
            border-radius: 5px; /* Adiciona bordas arredondadas ao link */
            display: inline-block; /* Faz o link se comportar como um bloco inline */
        }
        a:hover {
            text-decoration: underline; /* Adiciona um sublinhado ao passar o mouse sobre o link */
            background-color: #f0f0f0; /* Altera a cor de fundo ao passar o mouse sobre o link */
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Login Falhou</h1> <!-- Título da página de erro -->
        <p>Credenciais inválidas. Por favor, verifique seu nome de usuário e senha.</p> <!-- Mensagem de erro informando que as credenciais são inválidas -->
        <a href="/">Voltar ao Login</a> <!-- Link para voltar à tela de login -->
    </div>
</body>
</html>
)rawliteral"); // Envia o HTML como resposta com código 401 (Unauthorized) para indicar que as credenciais são inválidas
    }
}

void handleLogout()
{
    // Verifica se há um usuário logado
    if (userLoggedIn)
    {
        userLoggedIn = false; // Define a variável userLoggedIn como falsa, indicando que o usuário foi deslogado
        loggedInUser = ""; // Limpa o nome do usuário logado
        server.sendHeader("Location", "/"); // Define o cabeçalho da resposta HTTP para redirecionar para a página inicial
        server.send(302, "text/plain", ""); // Envia uma resposta HTTP com código 302 (Found) e um corpo vazio, para redirecionar o usuário
    }
    else
    {
        // Se não há nenhum usuário logado, redireciona para a página inicial
        server.sendHeader("Location", "/"); // Define o cabeçalho da resposta HTTP para redirecionar para a página inicial
        server.send(302, "text/plain", ""); // Envia uma resposta HTTP com código 302 (Found) e um corpo vazio, para redirecionar o usuário
    }
}
