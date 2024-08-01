#include "umidade.h" // Inclui o cabeçalho para a configuração da página de umidade
#include <Arduino.h>
#include <WebServer.h>

// Função para configurar a página de umidade
void setupUmidadePage(WebServer& server)
{
    // HTML da página de umidade
    String html = R"(
        <!DOCTYPE html>
        <html lang="pt-br">
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>Umidade</title>
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

    // Configura o servidor para responder à requisição da página de umidade
    server.on("/umidade", HTTP_GET, [html, &server]() mutable {
        server.send(200, "text/html", html);
    });
}
