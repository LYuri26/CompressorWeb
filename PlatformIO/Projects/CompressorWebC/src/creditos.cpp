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
                    background-color: #f8f9fa;
                    margin: 0;
                    padding: 0;
                    height: 100vh;
                    display: flex;
                    flex-direction: column;
                    justify-content: center;
                    align-items: center;
                }

                .credits-container {
                    background-color: #ffffff;
                    padding: 20px;
                    border-radius: 8px;
                    box-shadow: 0 0 15px rgba(0, 0, 0, 0.1);
                    width: 90%;
                    max-width: 700px;
                    text-align: center;
                }

                .credits-title {
                    font-size: 2rem;
                    margin-bottom: 20px;
                    color: #007bff;
                    font-weight: bold;
                }

                .credits-content {
                    font-size: 1rem;
                    color: #333333;
                    margin-bottom: 20px;
                }

                .btn-link, .btn-link-voltar {
                    background-color: #007bff; /* Azul */
                    color: white;
                    border: none;
                    padding: 10px 20px;
                    text-align: center;
                    text-decoration: none;
                    display: inline-block;
                    font-size: 1rem;
                    border-radius: 4px;
                    cursor: pointer;
                    margin: 5px;
                    transition: background-color 0.3s ease;
                    font-weight: bold;
                }

                .btn-link:hover, .btn-link-voltar:hover {
                    background-color: #0056b3; /* Azul escuro */
                }

                .btn-link-voltar {
                    background-color: #28a745; /* Verde */
                }

                .btn-link-voltar:hover {
                    background-color: #218838; /* Verde escuro */
                }

                .footer {
                    position: fixed;
                    bottom: 0;
                    width: 100%;
                    background-color: #007bff;
                    color: white;
                    text-align: center;
                    padding: 10px 0;
                    font-size: 14px;
                }
            </style>
        </head>
        <body>
            <div class="credits-container">
                <h2 class="credits-title">Créditos</h2>
                <div class="credits-content">
                    <h3>Alunos</h3>
                    <a href="#" class="btn-link" target="_blank">Felipe</a>
                    <a href="#" class="btn-link" target="_blank">Pietro</a>
                    <a href="#" class="btn-link" target="_blank">Davi</a>
                    <a href="#" class="btn-link" target="_blank">Paulo</a>
                    <a href="#" class="btn-link" target="_blank">Yago</a>
                    <h3>Instrutor</h3>
                    <a href="https://github.com/LYuri26" class="btn-link" target="_blank">Lenon Yuri</a>
                </div>
                <button onclick="window.location.href='/'" class="btn-link-voltar">Voltar</button>
            </div>
            <div class="footer">
                <p>Aplicação desenvolvida pela Turma de Informática Para Internet Trilhas de Futuro 2024</p>
                <p>Instrutor: Lenon Yuri</p>
            </div>
        </body>
        </html>
    )";

    // Configuração da rota "/credits" para responder com o HTML criado
    server.on("/credits", HTTP_GET, [creditsHtml, &server]() {
        server.send(200, "text/html", creditsHtml);  // Envia a resposta HTTP com o HTML da página de créditos
    });
}
