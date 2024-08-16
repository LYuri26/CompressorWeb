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
void setupCreditosPage(AsyncWebServer &server)
{
    // Define o conteúdo HTML da página de créditos
    String creditsHtml = R"rawliteral(
        <!DOCTYPE html> <!-- Declara o tipo de documento como HTML5 -->
        <html lang="pt-br"> <!-- Define o idioma da página como português do Brasil -->
        <head>
            <meta charset="UTF-8"> <!-- Define a codificação de caracteres como UTF-8, garantindo suporte a caracteres especiais -->
            <meta name="viewport" content="width=device-width, initial-scale=1.0"> <!-- Configura o viewport para dispositivos móveis, ajustando a largura da página ao tamanho da tela -->
            <title>Créditos</title> <!-- Define o título da página que aparece na aba do navegador -->
            <style>
                /* Estilo da página */
                body {
                    font-family: Arial, sans-serif; /* Define a fonte padrão do texto para Arial e uma fonte genérica sem serifa */
                    background-color: #f0f0f0; /* Define a cor de fundo da página como um tom claro de cinza */
                    margin: 0; /* Remove a margem padrão ao redor da página */
                    padding: 0; /* Remove o padding padrão ao redor da página */
                    display: flex; /* Usa Flexbox para layout, centralizando o conteúdo da página */
                    justify-content: center; /* Alinha o conteúdo horizontalmente ao centro */
                    align-items: center; /* Alinha o conteúdo verticalmente ao centro */
                    height: 100vh; /* Define a altura da página para 100% da altura da visualização */
                }

                .credits-container {
                    background-color: #ffffff; /* Define a cor de fundo do container dos créditos como branco */
                    padding: 20px; /* Adiciona padding ao container para espaçamento interno */
                    border-radius: 8px; /* Adiciona bordas arredondadas ao container */
                    box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1); /* Adiciona uma sombra ao container para efeito de profundidade */
                    text-align: center; /* Centraliza o texto dentro do container */
                    width: 90%; /* Define a largura do container como 90% da largura da página */
                    max-width: 500px; /* Define a largura máxima do container para garantir que não fique muito largo */
                }

                .credits-title {
                    font-size: 24px; /* Define o tamanho da fonte do título */
                    color: #007bff; /* Define a cor do texto do título como azul */
                    margin-bottom: 20px; /* Adiciona uma margem inferior ao título */
                }

                .credits-content h3 {
                    margin: 15px 0; /* Adiciona uma margem superior e inferior aos sub-títulos */
                    color: #333; /* Define a cor do texto dos sub-títulos como um tom escuro de cinza */
                }

                .credits-content ul {
                    list-style-type: none; /* Remove as marcas padrão da lista */
                    padding: 0; /* Remove o padding padrão da lista */
                    display: flex; /* Usa Flexbox para organizar itens em linha */
                    justify-content: center; /* Centraliza os itens horizontalmente */
                    flex-wrap: wrap; /* Permite que os itens se quebrem em uma nova linha se necessário */
                }

                .credits-content ul li {
                    margin: 10px; /* Adiciona margem entre os itens da lista */
                }

                .credits-content li a {
                    display: inline-block; /* Faz com que o link se comporte como um bloco inline, permitindo padding e margens */
                    background-color: #007bff; /* Define a cor de fundo do botão como azul */
                    color: #ffffff; /* Define a cor do texto do botão como branco */
                    text-decoration: none; /* Remove o sublinhado do link */
                    padding: 12px 20px; /* Adiciona padding ao botão para espaçamento interno */
                    border-radius: 5px; /* Adiciona bordas arredondadas ao botão */
                    font-size: 16px; /* Define o tamanho da fonte do botão */
                    transition: background-color 0.3s, transform 0.3s; /* Adiciona uma transição suave na cor de fundo e efeito de transformação ao passar o mouse */
                }

                .credits-content li a:hover {
                    background-color: #0056b3; /* Altera a cor de fundo ao passar o mouse sobre o botão */
                    transform: scale(1.05); /* Aplica um efeito de zoom ao passar o mouse sobre o botão */
                }

                .btn-link-voltar {
                    background-color: #28a745; /* Define a cor de fundo do botão de voltar como verde */
                    border: none; /* Remove a borda padrão do botão */
                    color: #ffffff; /* Define a cor do texto do botão como branco */
                    padding: 12px 25px; /* Adiciona padding ao botão de voltar para espaçamento interno */
                    font-size: 16px; /* Define o tamanho da fonte do botão de voltar */
                    cursor: pointer; /* Muda o cursor para uma mão ao passar o mouse sobre o botão */
                    border-radius: 5px; /* Adiciona bordas arredondadas ao botão de voltar */
                    margin-top: 20px; /* Adiciona uma margem superior ao botão de voltar */
                    transition: background-color 0.3s; /* Adiciona uma transição suave na cor de fundo ao passar o mouse */
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
                    <ul>
                        <li><a href="#" target="_blank">Ali</a></li> <!-- Link para o nome do aluno, não tem URL definida -->
                        <li><a href="#" target="_blank">Felipe</a></li>
                        <li><a href="#" target="_blank">Paulo</a></li>
                        <li><a href="#" target="_blank">Pietro</a></li>
                        <li><a href="#" target="_blank">Yago</a></li>
                    </ul>
                    <h3>Instrutores</h3> <!-- Sub-título para a lista de instrutores -->
                    <ul>
                        <li><a href="https://github.com/LYuri26" target="_blank">Lenon Yuri</a></li> <!-- Link para o perfil do instrutor no GitHub -->
                        <li><a href="https://github.com/RoCkHeLuCk" target="_blank">Franco M. A. Caixeta</a></li> <!-- Link para o perfil do instrutor no GitHub -->
                    </ul>
                </div>
                <!-- Botão para voltar à página inicial -->
                <button onclick="window.location.href='/'" class="btn-link-voltar">Voltar</button> <!-- Define um botão que redireciona para a página inicial -->
            </div>
        </body>
        </html>
    )rawliteral";

    // -------------------------------------------------------------------------
    // Configura a rota "/creditos" para responder com o HTML da página de créditos
    // -------------------------------------------------------------------------
    server.on("/creditos", HTTP_GET, [creditsHtml](AsyncWebServerRequest *request)
              {
                  request->send(200, "text/html", creditsHtml); // Envia a resposta HTTP com o HTML da página de créditos
              });
}
