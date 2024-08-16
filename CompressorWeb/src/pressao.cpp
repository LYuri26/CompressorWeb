#include <ESPAsyncWebServer.h> // Biblioteca para criar e gerenciar um servidor web assíncrono no ESP32
#include "pressao.h"           // Cabeçalho para a configuração específica da página de nível de óleo

/**
 * Configura a página de nível de óleo no servidor web.
 *
 * @param server A instância do servidor web assíncrono fornecido para configuração.
 */
void setupPressaoPage(AsyncWebServer &server)
{
    // Define o conteúdo HTML da página de nível de óleo usando uma string literal crua (R"rawliteral()")
    // A string literal crua permite que você inclua o código HTML, CSS e JavaScript sem precisar escapar caracteres especiais.
    String html = R"rawliteral(
        <!DOCTYPE html> <!-- Declaração do tipo de documento HTML5, informa ao navegador que o documento é HTML5 -->
        <html lang="pt-br"> <!-- Define o idioma da página como Português Brasileiro -->
        <head>
            <meta charset="UTF-8"> <!-- Define a codificação de caracteres como UTF-8, que suporta todos os caracteres e símbolos internacionais -->
            <meta name="viewport" content="width=device-width, initial-scale=1.0"> <!-- Configura o viewport para dispositivos móveis, garantindo que a página se ajuste corretamente ao tamanho da tela -->
            <title>Nível de Óleo</title> <!-- Título da página que aparece na aba do navegador -->
            <style>
                body {
                    font-family: Arial, sans-serif; /* Define a fonte padrão da página para Arial e usa sans-serif como fonte secundária se Arial não estiver disponível */
                    background-color: #e9ecef; /* Define a cor de fundo da página como um cinza claro */
                    height: 100vh; /* Define a altura da página como 100% da altura da viewport (tela visível) */
                    margin: 0; /* Remove as margens padrão do navegador */
                    display: flex; /* Usa flexbox para layout, permitindo fácil centralização do conteúdo */
                    justify-content: center; /* Alinha o conteúdo horizontalmente no centro da página */
                    align-items: center; /* Alinha o conteúdo verticalmente no centro da página */
                }
                .container {
                    background-color: #ffffff; /* Define a cor de fundo do container como branco */
                    padding: 20px; /* Adiciona 20 pixels de espaçamento interno ao redor do conteúdo do container */
                    border-radius: 8px; /* Arredonda os cantos do container com um raio de 8 pixels */
                    box-shadow: 0 4px 8px rgba(0, 0, 0, 0.2); /* Adiciona uma sombra ao container para um efeito de profundidade */
                    text-align: center; /* Centraliza o texto dentro do container */
                    max-width: 600px; /* Define a largura máxima do container como 600 pixels */
                    width: 100%; /* Define a largura do container como 100% da largura disponível do pai */
                }
                .title {
                    font-size: 32px; /* Define o tamanho da fonte do título como 32 pixels */
                    margin-bottom: 20px; /* Adiciona 20 pixels de espaço abaixo do título */
                    color: #007bff; /* Define a cor do texto do título como azul */
                }
                .btn-back {
                    display: inline-block; /* Exibe o botão como um bloco inline, permitindo que o espaçamento e o tamanho sejam ajustados */
                    margin-top: 20px; /* Adiciona 20 pixels de espaço acima do botão */
                    padding: 10px 20px; /* Adiciona 10 pixels de espaçamento interno vertical e 20 pixels horizontal */
                    background-color: #007bff; /* Define a cor de fundo do botão como azul */
                    color: white; /* Define a cor do texto do botão como branco */
                    border: none; /* Remove a borda padrão do botão */
                    border-radius: 5px; /* Arredonda os cantos do botão com um raio de 5 pixels */
                    text-decoration: none; /* Remove o sublinhado do link para que se pareça com um botão */
                    font-size: 16px; /* Define o tamanho da fonte do botão como 16 pixels */
                    transition: background-color 0.3s; /* Adiciona uma transição suave de 0,3 segundos para a cor de fundo ao passar o mouse sobre o botão */
                }
                .btn-back:hover {
                    background-color: #0056b3; /* Altera a cor de fundo do botão para um tom mais escuro de azul quando o mouse está sobre ele */
                }
            </style>
        </head>
        <body>
            <div class="container"> <!-- Contêiner centralizado que contém o conteúdo principal da página -->
                <h2 class="title">PÁGINA EM CONSTRUÇÃO</h2> <!-- Título da página indicando que ela está em construção -->
                <a href="/dashboard" class="btn-back">Voltar para o Dashboard</a> <!-- Link estilizado como um botão para retornar ao dashboard -->
            </div>
        </body>
        </html>
    )rawliteral";

    // Configura a rota "/pressao" no servidor web para responder com o conteúdo HTML definido acima
    server.on("/pressao", HTTP_GET, [html](AsyncWebServerRequest *request)
              {
        // Envia uma resposta HTTP com o código 200 (OK), tipo de conteúdo "text/html", e o conteúdo da página HTML
        request->send(200, "text/html", html); });
}
