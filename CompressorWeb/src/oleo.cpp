// -------------------------------------------------------------------------
// Configurações da Página de Nível de Óleo
// -------------------------------------------------------------------------
#include <ESPAsyncWebServer.h>     // Biblioteca para criar um servidor web assíncrono
#include "oleo.h"                 // Cabeçalho para a configuração da página de nível de óleo

/**
 * Configura a página de nível de óleo no servidor web.
 *
 * @param server A instância do servidor web assíncrono.
 */
void setupOleoPage(AsyncWebServer& server)
{
    // Define o HTML da página de nível de óleo usando uma string literal (R"()")
    String html = R"rawliteral(
        <!DOCTYPE html> <!-- Declaração do tipo de documento HTML5 -->
        <html lang="pt-br"> <!-- Define o idioma da página como Português Brasileiro -->
        <head>
            <meta charset="UTF-8"> <!-- Define a codificação de caracteres como UTF-8 -->
            <meta name="viewport" content="width=device-width, initial-scale=1.0"> <!-- Configura o viewport para dispositivos móveis -->
            <title>Nível de Óleo</title> <!-- Título da página -->
            <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css"> <!-- Link para o Bootstrap CSS -->
            <style>
                body {
                    font-family: Arial, sans-serif; /* Define a fonte padrão da página */
                    background-color: #f8f9fa; /* Cor de fundo da página */
                    height: 100%; /* Define a altura como 100% da viewport */
                    margin: 0; /* Remove margens padrão do navegador */
                    display: flex; /* Usa flexbox para centralizar o conteúdo */
                    justify-content: center; /* Alinha horizontalmente no centro */
                    align-items: center; /* Alinha verticalmente no centro */
                    flex-direction: column; /* Organiza o conteúdo em coluna */
                }
                .container {
                    background-color: #ffffff; /* Cor de fundo do container */
                    padding: 20px; /* Espaçamento interno do container */
                    border-radius: 5px; /* Cantos arredondados do container */
                    box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); /* Sombra do container */
                    text-align: center; /* Centraliza o texto no container */
                    max-width: 600px; /* Largura máxima do container */
                    width: 100%; /* Largura do container */
                }
                .title {
                    font-size: 28px; /* Tamanho da fonte do título */
                    margin-bottom: 20px; /* Espaçamento abaixo do título */
                    color: #007bff; /* Cor do título */
                }
                .btn-back {
                    margin-top: 20px; /* Espaçamento acima do botão */
                    background-color: #007bff; /* Cor de fundo do botão */
                    color: white; /* Cor do texto do botão */
                }
            </style>
        </head>
        <body>
            <div class="container"> <!-- Container para o conteúdo da página -->
                <h2 class="title">PÁGINA EM CONSTRUÇÃO</h2> <!-- Título da página -->
                <a href="/dashboard" class="btn btn-back btn-block">Voltar para o Dashboard</a> <!-- Link para voltar ao dashboard -->
            </div>
        </body>
        </html>
    )rawliteral";

    // Configura o servidor para responder a requisições para a página de nível de óleo
    server.on("/oleo", HTTP_GET, [html](AsyncWebServerRequest *request) {
        // Envia uma resposta HTTP 200 (OK) com o tipo de conteúdo "text/html" e o conteúdo da página HTML definida
        request->send(200, "text/html", html);
    });
}
