#include <ESPAsyncWebServer.h> // Biblioteca para criar um servidor web assíncrono
#include "umidade.h"           // Cabeçalho para a configuração da página de umidade

// -------------------------------------------------------------------------
// Função para Configurar a Página de Umidade
// -------------------------------------------------------------------------
void setupUmidadePage(AsyncWebServer& server) {
    // Define o conteúdo HTML da página de umidade
    String html = R"(
        <!DOCTYPE html>
        <html lang="pt-br">
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>Umidade</title>
            <!-- Link para o CSS do Bootstrap para estilizar a página -->
            <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css">
            <style>
                /* Estilos CSS personalizados para a página */
                body {
                    font-family: Arial, sans-serif; /* Fonte padrão para a página */
                    background-color: #f8f9fa; /* Cor de fundo da página */
                    height: 100%; /* Altura total da página */
                    margin: 0; /* Remove a margem padrão */
                    display: flex; /* Usado para centralizar o conteúdo */
                    justify-content: center; /* Alinha o conteúdo horizontalmente */
                    align-items: center; /* Alinha o conteúdo verticalmente */
                    flex-direction: column; /* Alinha os itens em coluna */
                }
                .container {
                    background-color: #ffffff; /* Cor de fundo do contêiner */
                    padding: 20px; /* Espaçamento interno do contêiner */
                    border-radius: 5px; /* Bordas arredondadas do contêiner */
                    box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); /* Sombra do contêiner */
                    text-align: center; /* Alinha o texto ao centro */
                    max-width: 600px; /* Largura máxima do contêiner */
                    width: 100%; /* Largura total do contêiner */
                }
                .title {
                    font-size: 28px; /* Tamanho da fonte do título */
                    margin-bottom: 20px; /* Espaçamento abaixo do título */
                    color: #007bff; /* Cor do texto do título */
                }
                .btn-back {
                    margin-top: 20px; /* Espaçamento acima do botão */
                    background-color: #007bff; /* Cor de fundo do botão */
                    color: white; /* Cor do texto do botão */
                }
            </style>
        </head>
        <body>
            <div class="container">
                <h2 class="title">PÁGINA EM CONSTRUÇÃO</h2>
                <!-- Link para voltar ao dashboard -->
                <a href="/dashboard" class="btn btn-back btn-block">Voltar para o Dashboard</a>
            </div>
        </body>
        </html>
    )";

    // Configura a rota para a página de umidade
    server.on("/umidade", HTTP_GET, [html](AsyncWebServerRequest *request){
        // Envia a resposta HTTP com o conteúdo HTML da página de umidade
        request->send(200, "text/html", html);
    });
}
