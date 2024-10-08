#include <ESPAsyncWebServer.h> // Biblioteca para criar e gerenciar um servidor web assíncrono no ESP32
#include "paginaserro.h"       // Cabeçalho para a configuração específica das páginas de erro

/**
 * Gera uma página de erro em HTML com base nos parâmetros fornecidos.
 *
 * @param title O título da página de erro.
 * @param message A mensagem de erro a ser exibida.
 * @param linkText O texto do link de navegação.
 * @param linkHref O URL para o qual o link deve redirecionar.
 * @return O código HTML da página de erro.
 */
String generateErrorPage(const String &title, const String &message, const String &linkText, const String &linkHref)
{
    // Retorna o código HTML da página de erro formatado com os parâmetros fornecidos
    return R"rawliteral(
<!DOCTYPE html> <!-- Declaração do tipo de documento HTML5, que informa ao navegador que o conteúdo é HTML5 -->
<html lang="pt-br"> <!-- Define o idioma da página como Português Brasileiro -->
<head>
    <meta charset="UTF-8"> <!-- Define a codificação de caracteres como UTF-8 para suportar caracteres especiais e internacionais -->
    <meta name="viewport" content="width=device-width, initial-scale=1.0"> <!-- Configura o viewport para dispositivos móveis, ajustando o layout ao tamanho da tela -->
    <title>)rawliteral" +
           title + R"rawliteral(</title> <!-- Define o título da página que aparece na aba do navegador -->
    <style>
        body {
            font-family: Arial, sans-serif; /* Define a fonte padrão da página para Arial e usa sans-serif como fonte secundária */
            display: flex; /* Usa o modelo de layout flexbox para centralizar o conteúdo */
            justify-content: center; /* Alinha o conteúdo horizontalmente no centro da página */
            align-items: center; /* Alinha o conteúdo verticalmente no centro da página */
            height: 100vh; /* Define a altura da página como 100% da altura da viewport (tela visível) */
            margin: 0; /* Remove as margens padrão do navegador */
            background-color: #f4f4f4; /* Define a cor de fundo da página como cinza claro */
        }
        .container {
            text-align: center; /* Centraliza o texto dentro do container */
            background-color: #fff; /* Define a cor de fundo do container como branco */
            padding: 20px; /* Adiciona 20 pixels de espaçamento interno ao redor do conteúdo do container */
            border-radius: 8px; /* Arredonda os cantos do container com um raio de 8 pixels */
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1); /* Adiciona uma sombra ao container para um efeito de profundidade sutil */
            max-width: 600px; /* Define a largura máxima do container como 600 pixels */
            width: 100%; /* Define a largura do container como 100% da largura disponível do pai */
        }
        h1 {
            color: #e74c3c; /* Define a cor do texto do título como vermelho */
            font-size: 36px; /* Define o tamanho da fonte do título como 36 pixels */
            margin-bottom: 10px; /* Adiciona 10 pixels de espaço abaixo do título */
        }
        p {
            color: #555; /* Define a cor do texto dos parágrafos como cinza escuro */
            font-size: 18px; /* Define o tamanho da fonte dos parágrafos como 18 pixels */
            margin-bottom: 20px; /* Adiciona 20 pixels de espaço abaixo do parágrafo */
        }
        a {
            text-decoration: none; /* Remove o sublinhado dos links para estilizar como botões */
            color: #3498db; /* Define a cor do texto dos links como azul */
            font-weight: bold; /* Define o peso da fonte dos links como negrito */
            padding: 10px 20px; /* Adiciona 10 pixels de espaçamento interno vertical e 20 pixels horizontal ao link */
            border: 2px solid #3498db; /* Define uma borda azul ao redor do link */
            border-radius: 5px; /* Arredonda os cantos da borda do link com um raio de 5 pixels */
            display: inline-block; /* Faz com que o link se comporte como um bloco inline para melhor controle de espaçamento e layout */
            transition: all 0.3s; /* Adiciona uma transição suave de 0,3 segundos para todas as propriedades CSS ao passar o mouse */
        }
        a:hover {
            text-decoration: underline; /* Adiciona um sublinhado ao link quando o mouse passa sobre ele */
            background-color: #f0f0f0; /* Altera a cor de fundo do link quando o mouse passa sobre ele */
        }
    </style>
</head>
<body>
    <div class="container"> <!-- Contêiner centralizado que contém o conteúdo da página de erro -->
        <h1>)rawliteral" +
           title + R"rawliteral(</h1> <!-- Exibe o título da página de erro -->
        <p>)rawliteral" +
           message + R"rawliteral(</p> <!-- Exibe a mensagem de erro fornecida -->
        <a href=")rawliteral" +
           linkHref + R"rawliteral(">)rawliteral" + linkText + R"rawliteral(</a> <!-- Link para navegação para a URL fornecida -->
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
void setupUsuarioJaLogadoPage(AsyncWebServer &server)
{
    // Define a rota "/usuario-ja-logado" para responder com a página de erro "Usuário Já Logado"
    server.on("/usuario-ja-logado", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        // Envia uma resposta HTTP com o código de status 403 (Forbidden) e o HTML gerado pela função generateErrorPage
        request->send(403, "text/html", generateErrorPage(
            "Usuário Já Logado", // Título da página de erro
            "Há um usuário atualmente logado. Por favor, tente novamente mais tarde.", // Mensagem de erro a ser exibida
            "Acessar Tela Inicial", // Texto do link
            "/" // URL para o link
        )); });
}

/**
 * Configura a rota para a página de erro "Credenciais Inválidas".
 *
 * @param server A instância do servidor web assíncrono.
 */
void setupCredenciaisInvalidasPage(AsyncWebServer &server)
{
    // Define a rota "/credenciais-invalidas" para responder com a página de erro "Credenciais Inválidas"
    server.on("/credenciais-invalidas", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        // Envia uma resposta HTTP com o código de status 401 (Unauthorized) e o HTML gerado pela função generateErrorPage
        request->send(401, "text/html", generateErrorPage(
            "Credenciais Inválidas", // Título da página de erro
            "Credenciais inválidas. Por favor, verifique seu nome de usuário e senha.", // Mensagem de erro a ser exibida
            "Voltar ao Login", // Texto do link
            "/" // URL para o link
        )); });
}

/**
 * Configura a rota para a página de erro "Acesso Inválido".
 *
 * @param server A instância do servidor web assíncrono.
 */
void setupAcessoInvalidoPage(AsyncWebServer &server)
{
    // Define a rota "/acesso-invalido" para responder com a página de erro "Acesso Inválido"
    server.on("/acesso-invalido", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        // Envia uma resposta HTTP com o código de status 403 (Forbidden) e o HTML gerado pela função generateErrorPage
        request->send(403, "text/html", generateErrorPage(
            "Acesso Inválido", // Título da página de erro
            "Você não tem permissão para acessar esta página. Por favor, faça login para continuar.", // Mensagem de erro a ser exibida
            "Voltar à Página Inicial", // Texto do link
            "/" // URL para o link
        )); });
}

/**
 * Configura a rota para a página de erro "Página Não Encontrada".
 *
 * @param server A instância do servidor web assíncrono.
 */
void setupNotFoundPage(AsyncWebServer &server)
{
    // Define a rota padrão para responder com a página de erro "Página Não Encontrada"
    server.onNotFound([](AsyncWebServerRequest *request)
                      {
        // Envia uma resposta HTTP com o código de status 404 (Not Found) e o HTML gerado pela função generateErrorPage
        request->send(404, "text/html", generateErrorPage(
            "Página Não Encontrada", // Título da página de erro
            "Essa página não existe. Verifique o URL ou volte à página inicial.", // Mensagem de erro a ser exibida
            "Voltar à Página Inicial", // Texto do link
            "/" // URL para o link
        )); });
}
