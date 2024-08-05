// -------------------------------------------------------------------------
// Configurações das Páginas de Erro
// -------------------------------------------------------------------------
#include <ESPAsyncWebServer.h>     // Biblioteca para criar um servidor web assíncrono
#include "paginaserro.h"           // Cabeçalho para a configuração das páginas de erro

/**
 * Gera uma página de erro em HTML com base nos parâmetros fornecidos.
 *
 * @param title O título da página de erro.
 * @param message A mensagem de erro a ser exibida.
 * @param linkText O texto do link de navegação.
 * @param linkHref O URL para o qual o link deve redirecionar.
 * @return O código HTML da página de erro.
 */
String generateErrorPage(const String& title, const String& message, const String& linkText, const String& linkHref)
{
    // Retorna o código HTML da página de erro formatado com os parâmetros fornecidos
    return R"rawliteral(
<!DOCTYPE html> <!-- Declaração do tipo de documento HTML5 -->
<html lang="pt-br"> <!-- Define o idioma da página como Português Brasileiro -->
<head>
    <meta charset="UTF-8"> <!-- Define a codificação de caracteres como UTF-8 -->
    <meta name="viewport" content="width=device-width, initial-scale=1.0"> <!-- Configura o viewport para dispositivos móveis -->
    <title>)rawliteral" + title + R"rawliteral(</title> <!-- Título da página -->
    <style>
        body {
            font-family: Arial, sans-serif; /* Define a fonte padrão da página */
            display: flex; /* Usa flexbox para centralizar o conteúdo */
            justify-content: center; /* Alinha horizontalmente no centro */
            align-items: center; /* Alinha verticalmente no centro */
            height: 100vh; /* Define a altura como 100% da viewport */
            margin: 0; /* Remove margens padrão do navegador */
            background-color: #f4f4f4; /* Cor de fundo da página */
        }
        .container {
            text-align: center; /* Centraliza o texto dentro do container */
            background-color: #fff; /* Cor de fundo do container */
            padding: 20px; /* Espaçamento interno do container */
            border-radius: 8px; /* Cantos arredondados do container */
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); /* Sombra do container */
        }
        h1 {
            color: #e74c3c; /* Cor do texto do título */
        }
        p {
            color: #555; /* Cor do texto dos parágrafos */
        }
        a {
            text-decoration: none; /* Remove o sublinhado dos links */
            color: #3498db; /* Cor do texto dos links */
            font-weight: bold; /* Define o peso da fonte como negrito */
            padding: 10px 20px; /* Espaçamento interno dos links */
            border: 2px solid #3498db; /* Borda do link */
            border-radius: 5px; /* Cantos arredondados da borda do link */
            display: inline-block; /* Faz com que o link se comporte como um bloco inline */
        }
        a:hover {
            text-decoration: underline; /* Sublinha o link ao passar o mouse */
            background-color: #f0f0f0; /* Altera a cor de fundo ao passar o mouse */
        }
    </style>
</head>
<body>
    <div class="container"> <!-- Container para o conteúdo da página de erro -->
        <h1>)rawliteral" + title + R"rawliteral(</h1> <!-- Exibe o título da página de erro -->
        <p>)rawliteral" + message + R"rawliteral(</p> <!-- Exibe a mensagem de erro -->
        <a href=")rawliteral" + linkHref + R"rawliteral(">)rawliteral" + linkText + R"rawliteral(</a> <!-- Link para navegação -->
    </div>
</body>
</html>
)rawliteral";
}

/**
 * Configura a rota para a página de erro "Usuário Já Logado".
 *
 * @param server A instância do servidor web assíncrono.
 */
void setupUsuarioJaLogadoPage(AsyncWebServer& server)
{
    // Define a rota "/usuario-ja-logado" para responder com a página de erro "Usuário Já Logado"
    server.on("/usuario-ja-logado", HTTP_GET, [](AsyncWebServerRequest *request){
        // Envia a resposta HTTP com o código de status 403 e o HTML gerado
        request->send(403, "text/html", generateErrorPage(
            "Usuário Já Logado", // Título da página de erro
            "Há um usuário atualmente logado. Por favor, tente novamente mais tarde.", // Mensagem de erro
            "Acessar Tela Inicial", // Texto do link
            "/" // URL para o link
        ));
    });
}

/**
 * Configura a rota para a página de erro "Credenciais Inválidas".
 *
 * @param server A instância do servidor web assíncrono.
 */
void setupCredenciaisInvalidasPage(AsyncWebServer& server)
{
    // Define a rota "/credenciais-invalidas" para responder com a página de erro "Credenciais Inválidas"
    server.on("/credenciais-invalidas", HTTP_GET, [](AsyncWebServerRequest *request){
        // Envia a resposta HTTP com o código de status 401 e o HTML gerado
        request->send(401, "text/html", generateErrorPage(
            "Credenciais Inválidas", // Título da página de erro
            "Credenciais inválidas. Por favor, verifique seu nome de usuário e senha.", // Mensagem de erro
            "Voltar ao Login", // Texto do link
            "/" // URL para o link
        ));
    });
}

/**
 * Configura a rota para a página de erro "Acesso Inválido".
 *
 * @param server A instância do servidor web assíncrono.
 */
void setupAcessoInvalidoPage(AsyncWebServer& server)
{
    // Define a rota "/acesso-invalido" para responder com a página de erro "Acesso Inválido"
    server.on("/acesso-invalido", HTTP_GET, [](AsyncWebServerRequest *request){
        // Envia a resposta HTTP com o código de status 403 e o HTML gerado
        request->send(403, "text/html", generateErrorPage(
            "Acesso Inválido", // Título da página de erro
            "Você não tem permissão para acessar esta página. Por favor, faça login para continuar.", // Mensagem de erro
            "Voltar à Página Inicial", // Texto do link
            "/" // URL para o link
        ));
    });
}

/**
 * Configura a rota para a página de erro "Página Não Encontrada".
 *
 * @param server A instância do servidor web assíncrono.
 */
void setupNotFoundPage(AsyncWebServer& server)
{
    // Define a rota padrão para responder com a página de erro "Página Não Encontrada"
    server.onNotFound([](AsyncWebServerRequest *request){
        // Envia a resposta HTTP com o código de status 404 e o HTML gerado
        request->send(404, "text/html", generateErrorPage(
            "Página Não Encontrada", // Título da página de erro
            "Essa página não existe. Verifique o URL ou volte à página inicial.", // Mensagem de erro
            "Voltar à Página Inicial", // Texto do link
            "/" // URL para o link
        ));
    });
}
