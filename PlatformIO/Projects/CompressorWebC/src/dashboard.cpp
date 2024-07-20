#include "dashboard.h"  // Inclui o cabeçalho para a configuração da página do dashboard

// Variável global para controlar o estado do compressor
extern bool compressorLigado;  // Declaração da variável global definida em outro arquivo

void setupDashboardPage(ESP8266WebServer& server) {
    // HTML para a página do dashboard
    String html = R"(
        <!DOCTYPE html>
        <html lang="pt-br">
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>Dashboard</title>
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

                .dashboard-container {
                    background-color: #ffffff;
                    padding: 20px;
                    border-radius: 5px;
                    box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
                    width: 100%;
                    max-width: 600px;
                    text-align: center;
                    margin-top: 20px;
                }

                .dashboard-title {
                    font-size: 28px;
                    margin-bottom: 20px;
                    color: #007bff;
                }

                .btn-ligar {
                    background-color: #28a745; /* Verde */
                    color: white;
                }

                .btn-desligar {
                    background-color: #dc3545; /* Vermelho */
                    color: white;
                }

                .footer {
                    width: 100%;
                    background-color: #007bff;
                    color: white;
                    text-align: center;
                    padding: 10px 0;
                    position: fixed;
                    bottom: 0;
                    font-size: 14px;
                }
            </style>
        </head>
        <body>
            <div class="dashboard-container">
                <h2 class="dashboard-title">Bem-vindo ao Dashboard</h2>
                <a href="/toggle" class="btn btn-block mb-2 btn-ligar" id="toggleButton">Ligar</a>
                <a href="/humidity" class="btn btn-secondary btn-block mb-2">Umidade</a>
                <a href="/oil-level" class="btn btn-secondary btn-block mb-2">Nível de Óleo</a>
                <a href="/" class="btn btn-danger btn-block mt-3">Logout</a>
            </div>

            <div class="footer">
                <p>Aplicação desenvolvida pela Turma de Informática Para Internet Trilhas de Futuro 2024</p>
                <p>Instrutor: Lenon Yuri</p>
            </div>

            <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.5.1/jquery.min.js"></script>
            <script src="https://maxcdn.bootstrapcdn.com/bootstrap/4.5.2/js/bootstrap.min.js"></script>
            <script>
                var toggleButton = document.getElementById('toggleButton');
                toggleButton.addEventListener('click', function(event) {
                    event.preventDefault();
                    var action = toggleButton.innerHTML === 'Ligar' ? 'ligar' : 'desligar';
                    fetch('/toggle?action=' + action)
                        .then(response => response.text())
                        .then(data => {
                            console.log('Resposta do servidor:', data);
                            if (action === 'ligar') {
                                toggleButton.innerHTML = 'Desligar';
                                toggleButton.classList.remove('btn-ligar');
                                toggleButton.classList.add('btn-desligar');
                            } else {
                                toggleButton.innerHTML = 'Ligar';
                                toggleButton.classList.remove('btn-desligar');
                                toggleButton.classList.add('btn-ligar');
                            }
                        })
                        .catch(error => console.error('Erro ao enviar requisição:', error));
                });
            </script>
        </body>
        </html>
    )";

    // Configuração da rota "/dashboard" para responder com o HTML criado
    server.on("/dashboard", HTTP_GET, [html, &server]() mutable {
        server.send(200, "text/html", html);  // Envia a resposta HTTP com o HTML do dashboard
    });
}

void handleToggleAction(ESP8266WebServer& server) {
    // Rota para lidar com a ação de ligar/desligar o compressor
    server.on("/toggle", HTTP_GET, [&server]() {
        String action = server.arg("action");  // Obtém o valor do parâmetro "action" da URL
        if (action == "ligar") {
            // Lógica para ligar o compressor (simulação)
            compressorLigado = true;
            server.send(200, "text/plain", "Compressor ligado!");  // Responde com "Compressor ligado!" em texto simples
        } else if (action == "desligar") {
            // Lógica para desligar o compressor (simulação)
            compressorLigado = false;
            server.send(200, "text/plain", "Compressor desligado!");  // Responde com "Compressor desligado!" em texto simples
        } else {
            server.send(400, "text/plain", "Ação inválida!");  // Responde com "Ação inválida!" se a ação não for reconhecida
        }
    });
}
