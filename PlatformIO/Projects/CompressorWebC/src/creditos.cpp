#include "creditos.h"  // Inclui o cabeçalho para a configuração da página de créditos

void setupCreditsPage(ESP8266WebServer& server) {
    // Variável local para armazenar o HTML da página de créditos
    String creditsHtml = R"(
        <!DOCTYPE html>
        <html lang="pt-br">
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>Créditos</title>
            <style>
                body {
                    font-family: Arial, sans-serif;
                    background-color: #f0f0f0;
                    margin: 0;
                    padding: 0;
                    display: flex;
                    justify-content: center;
                    align-items: center;
                    height: 100vh;
                }

                .credits-container {
                    background-color: #ffffff;
                    padding: 20px;
                    border-radius: 8px;
                    box-shadow: 0 0 8px rgba(0, 0, 0, 0.1);
                    text-align: center;
                    width: 90%;
                    max-width: 500px;
                }

                .credits-title {
                    font-size: 22px;
                    color: #007bff;
                    margin-bottom: 20px;
                }

                .credits-content h3 {
                    margin: 10px 0;
                    color: #333;
                }

                .btn-link {
                    display: inline-block;
                    background-color: #007bff;
                    color: #ffffff;
                    text-decoration: none;
                    padding: 10px 15px;
                    border-radius: 5px;
                    font-size: 14px;
                    margin: 5px;
                    transition: background-color 0.3s;
                }

                .btn-link:hover {
                    background-color: #0056b3;
                }

                .btn-link-voltar {
                    background-color: #28a745;
                    border: none;
                    color: #ffffff;
                    padding: 10px 20px;
                    font-size: 16px;
                    cursor: pointer;
                }

                .btn-link-voltar:hover {
                    background-color: #218838;
                }
            </style>
        </head>
        <body>
            <div class="credits-container">
                <h2 class="credits-title">Créditos</h2>
                <div class="credits-content">
                    <h3>Alunos</h3>
                    <a href="#" class="btn-link" target="_blank">Ali</a>
                    <a href="#" class="btn-link" target="_blank">Felipe</a>
                    <a href="#" class="btn-link" target="_blank">Paulo</a>
                    <a href="#" class="btn-link" target="_blank">Pietro</a>
                    <a href="#" class="btn-link" target="_blank">Yago</a>
                    <h3>Instrutores</h3>
                    <a href="https://github.com/LYuri26" class="btn-link" target="_blank">Lenon Yuri</a>
                    <a href="https://github.com/RoCkHeLuCk" class="btn-link" target="_blank">Franco M. A. Caixeta</a>
                </div>
                <button onclick="window.location.href='/'" class="btn-link-voltar">Voltar</button>
            </div>
        </body>
        </html>
    )";

    // Configuração da rota "/credits" para responder com o HTML criado
    server.on("/credits", HTTP_GET, [creditsHtml, &server]() {
        server.send(200, "text/html", creditsHtml);  // Envia a resposta HTTP com o HTML da página de créditos
    });
}
