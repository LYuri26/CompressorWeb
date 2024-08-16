#include <ESPAsyncWebServer.h> // Biblioteca para criar um servidor web assíncrono com suporte a requisições HTTP e WebSocket
#include "umidade.h"           // Cabeçalho para a configuração da página de umidade, deve conter a declaração da função setupUmidadePage

// -------------------------------------------------------------------------
// Função para Configurar a Página de Umidade
// -------------------------------------------------------------------------
void setupUmidadePage(AsyncWebServer &server)
{
    // Define o conteúdo HTML da página de umidade
    String html = R"(
        <!DOCTYPE html>
        <html lang="pt-br">
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>Umidade</title>
            <style>
                /* Estilos CSS personalizados para a página */
                body {
                    font-family: Arial, sans-serif; /* Fonte padrão para a página, Arial é a fonte principal e sans-serif é a alternativa */
                    background-color: #e9ecef; /* Cor de fundo da página, um tom claro de cinza */
                    height: 100%; /* Define a altura do corpo da página para 100% da altura da tela */
                    margin: 0; /* Remove a margem padrão do navegador para evitar espaços em branco */
                    display: flex; /* Usa Flexbox para o layout da página */
                    justify-content: center; /* Alinha o conteúdo horizontalmente no centro da página */
                    align-items: center; /* Alinha o conteúdo verticalmente no centro da página */
                    flex-direction: column; /* Organiza os itens no layout em coluna, um abaixo do outro */
                }
                .container {
                    background-color: #ffffff; /* Cor de fundo do contêiner, branco */
                    padding: 20px; /* Espaçamento interno do contêiner para evitar que o conteúdo encoste nas bordas */
                    border-radius: 8px; /* Bordas arredondadas do contêiner */
                    box-shadow: 0 4px 8px rgba(0, 0, 0, 0.2); /* Adiciona uma sombra ao contêiner para criar um efeito de profundidade */
                    text-align: center; /* Centraliza o texto dentro do contêiner */
                    max-width: 600px; /* Define a largura máxima do contêiner */
                    width: 100%; /* Faz com que o contêiner ocupe toda a largura disponível dentro da largura máxima */
                }
                .title {
                    font-size: 32px; /* Tamanho da fonte do título */
                    margin-bottom: 20px; /* Espaçamento abaixo do título */
                    color: #007bff; /* Cor do texto do título, um tom de azul */
                }
                .btn-back {
                    display: inline-block; /* Faz com que o link seja exibido como um bloco em linha */
                    margin-top: 20px; /* Espaçamento acima do botão para separá-lo do conteúdo acima */
                    padding: 10px 20px; /* Espaçamento interno do botão para tornar o clique mais fácil */
                    background-color: #007bff; /* Cor de fundo do botão, azul */
                    color: white; /* Cor do texto do botão, branco */
                    border: none; /* Remove a borda padrão do botão */
                    border-radius: 5px; /* Bordas arredondadas do botão */
                    text-decoration: none; /* Remove o sublinhado do link */
                    font-size: 16px; /* Tamanho da fonte do botão */
                    transition: background-color 0.3s; /* Transição suave para a cor de fundo ao passar o mouse sobre o botão */
                }
                .btn-back:hover {
                    background-color: #0056b3; /* Cor de fundo do botão quando o mouse passa sobre ele, um tom mais escuro de azul */
                }
            </style>
        </head>
        <body>
            <div class="container">
                <h2 class="title">PÁGINA EM CONSTRUÇÃO</h2>
                <!-- Link para voltar ao dashboard -->
                <a href="/dashboard" class="btn-back">Voltar para o Dashboard</a>
            </div>
        </body>
        </html>
    )";

    // -------------------------------------------------------------------------
    // Configura a rota para a página de umidade
    // -------------------------------------------------------------------------
    server.on("/umidade", HTTP_GET, [html](AsyncWebServerRequest *request)
              {
                  // Envia a resposta HTTP com o conteúdo HTML da página de umidade
                  request->send(200, "text/html", html); // Envia uma resposta HTTP 200 OK com o tipo de conteúdo "text/html" e o conteúdo HTML definido
              });
}
