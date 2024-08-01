#include "oleo.h" // Inclui o cabeçalho para a configuração da página de nível de óleo
#include <Arduino.h> // Inclui a biblioteca Arduino.h que fornece funções e definições básicas para a plataforma Arduino
#include <WebServer.h> // Inclui a biblioteca WebServer.h que permite a criação de um servidor web

// Função para configurar a página de nível de óleo
// Esta função configura uma rota no servidor web para exibir uma página HTML quando acessada
void setupOleoPage(WebServer& server)
{
    // HTML da página de nível de óleo
    // Define a string contendo o código HTML da página de nível de óleo usando a sintaxe de string literal (R"()")
    String html = R"(
        <!DOCTYPE html>
        <html lang="pt-br">
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>Nível de Óleo</title>
            <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css">
            <style>
                body {
                    font-family: Arial, sans-serif;
                    background-color: #f8f9fa;
                    height: 100%;
                    margin: 0;
                    display: flex;
                    justify-content: center;
                    align-items: center;
                    flex-direction: column;
                }
                .container {
                    background-color: #ffffff;
                    padding: 20px;
                    border-radius: 5px;
                    box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
                    text-align: center;
                    max-width: 600px;
                    width: 100%;
                }
                .title {
                    font-size: 28px;
                    margin-bottom: 20px;
                    color: #007bff;
                }
                .btn-back {
                    margin-top: 20px;
                    background-color: #007bff;
                    color: white;
                }
            </style>
        </head>
        <body>
            <div class="container">
                <h2 class="title">PÁGINA EM CONSTRUÇÃO</h2>
                <a href="/dashboard" class="btn btn-back btn-block">Voltar para o Dashboard</a>
            </div>
        </body>
        </html>
    )";
    // O HTML define a estrutura da página, incluindo o cabeçalho, o corpo e o estilo da página

    // Configura o servidor para responder à requisição da página de nível de óleo
    // Configura uma rota "/oleo" no servidor web. Quando um cliente faz uma requisição GET para esta rota, a função lambda é executada
    server.on("/oleo", HTTP_GET, [html, &server]() mutable {
        server.send(200, "text/html", html);
        // Envia uma resposta HTTP 200 (OK) com o tipo de conteúdo "text/html" e o conteúdo da página HTML definida acima
    });
}
