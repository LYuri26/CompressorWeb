#include "creditos.h"          // Inclui o cabeçalho para a configuração da página de créditos
#include <ESPAsyncWebServer.h> // Inclui a biblioteca para o servidor web assíncrono

// -------------------------------------------------------------------------
// Função para configurar a página de créditos
// -------------------------------------------------------------------------

/**
 * Configura a rota e o conteúdo da página de créditos no servidor web.
 *
 * @param server A instância do servidor web assíncrono.
 */
void setupCreditosPage(AsyncWebServer& server) {
    // Define o conteúdo HTML da página de créditos
    String creditsHtml = R"rawliteral(
        <!DOCTYPE html> <!-- Declara o tipo de documento como HTML5 -->
        <html lang="pt-br"> <!-- Define o idioma da página como português do Brasil -->
        <head>
            <meta charset="UTF-8"> <!-- Define a codificação de caracteres como UTF-8 -->
            <meta name="viewport" content="width=device-width, initial-scale=1.0"> <!-- Configura o viewport para dispositivos móveis -->
            <title>Créditos</title> <!-- Define o título da página que aparece na aba do navegador -->
            <style>
                /* Estilo da página */
                body {
                    font-family: Arial, sans-serif; /* Define a fonte padrão do texto */
                    background-color: #f0f0f0; /* Define a cor de fundo da página */
                    margin: 0; /* Remove a margem padrão ao redor da página */
                    padding: 0; /* Remove o padding padrão ao redor da página */
                    display: flex; /* Usa Flexbox para centralizar o conteúdo */
                    justify-content: center; /* Alinha o conteúdo horizontalmente ao centro */
                    align-items: center; /* Alinha o conteúdo verticalmente ao centro */
                    height: 100vh; /* Define a altura da página para 100% da altura da visualização */
                }

                .credits-container {
                    background-color: #ffffff; /* Define a cor de fundo do container dos créditos */
                    padding: 20px; /* Adiciona padding ao container para espaçamento interno */
                    border-radius: 8px; /* Adiciona bordas arredondadas ao container */
                    box-shadow: 0 0 8px rgba(0, 0, 0, 0.1); /* Adiciona uma sombra ao container para efeito de profundidade */
                    text-align: center; /* Centraliza o texto dentro do container */
                    width: 90%; /* Define a largura do container como 90% da largura da página */
                    max-width: 500px; /* Define a largura máxima do container */
                }

                .credits-title {
                    font-size: 22px; /* Define o tamanho da fonte do título */
                    color: #007bff; /* Define a cor do texto do título */
                    margin-bottom: 20px; /* Adiciona uma margem inferior ao título */
                }

                .credits-content h3 {
                    margin: 10px 0; /* Adiciona uma margem superior e inferior aos sub-títulos */
                    color: #333; /* Define a cor do texto dos sub-títulos */
                }

                .btn-link {
                    display: inline-block; /* Faz com que o link se comporte como um bloco inline */
                    background-color: #007bff; /* Define a cor de fundo do botão */
                    color: #ffffff; /* Define a cor do texto do botão */
                    text-decoration: none; /* Remove o sublinhado do link */
                    padding: 10px 15px; /* Adiciona padding ao botão para espaçamento interno */
                    border-radius: 5px; /* Adiciona bordas arredondadas ao botão */
                    font-size: 14px; /* Define o tamanho da fonte do botão */
                    margin: 5px; /* Adiciona uma margem ao redor do botão */
                    transition: background-color 0.3s; /* Adiciona uma transição suave na cor de fundo ao passar o mouse */
                }

                .btn-link:hover {
                    background-color: #0056b3; /* Altera a cor de fundo ao passar o mouse sobre o botão */
                }

                .btn-link-voltar {
                    background-color: #28a745; /* Define a cor de fundo do botão de voltar */
                    border: none; /* Remove a borda padrão do botão */
                    color: #ffffff; /* Define a cor do texto do botão */
                    padding: 10px 20px; /* Adiciona padding ao botão de voltar para espaçamento interno */
                    font-size: 16px; /* Define o tamanho da fonte do botão de voltar */
                    cursor: pointer; /* Muda o cursor para uma mão ao passar o mouse sobre o botão */
                }

                .btn-link-voltar:hover {
                    background-color: #218838; /* Altera a cor de fundo ao passar o mouse sobre o botão de voltar */
                }
            </style>
        </head>
        <body>
            <div class="credits-container"> <!-- Contêiner para a página de créditos -->
                <h2 class="credits-title">Créditos</h2> <!-- Título da página de créditos -->
                <div class="credits-content">
                    <h3>Alunos</h3> <!-- Sub-título para a lista de alunos -->
                    <a href="#" class="btn-link" target="_blank">Ali</a> <!-- Link para o aluno Ali -->
                    <a href="#" class="btn-link" target="_blank">Felipe</a> <!-- Link para o aluno Felipe -->
                    <a href="#" class="btn-link" target="_blank">Paulo</a> <!-- Link para o aluno Paulo -->
                    <a href="#" class="btn-link" target="_blank">Pietro</a> <!-- Link para o aluno Pietro -->
                    <a href="#" class="btn-link" target="_blank">Yago</a> <!-- Link para o aluno Yago -->
                    <h3>Instrutores</h3> <!-- Sub-título para a lista de instrutores -->
                    <a href="https://github.com/LYuri26" class="btn-link" target="_blank">Lenon Yuri</a> <!-- Link para o instrutor Lenon Yuri -->
                    <a href="https://github.com/RoCkHeLuCk" class="btn-link" target="_blank">Franco M. A. Caixeta</a> <!-- Link para o instrutor Franco M. A. Caixeta -->
                </div>
                <!-- Botão para voltar à página inicial -->
                <button onclick="window.location.href='/'" class="btn-link-voltar">Voltar</button>
            </div>
        </body>
        </html>
    )rawliteral";

    // Configura a rota "/creditos" para responder com o HTML da página de créditos
    server.on("/creditos", HTTP_GET, [creditsHtml](AsyncWebServerRequest *request) {
        request->send(200, "text/html", creditsHtml);  // Envia a resposta HTTP com o HTML da página de créditos
    });
}
