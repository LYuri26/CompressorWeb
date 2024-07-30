#include "dashboard.h"
#include <WebServer.h>

extern bool compressorLigado; // Declara uma variável externa para verificar o estado do compressor

// Função para configurar a página do dashboard
void setupDashboardPage(WebServer& server)
{
    // HTML da página do dashboard
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
                    background-color: #28a745;
                    color: white;
                }

                .btn-desligar {
                    background-color: #dc3545;
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
                <a href="#" class="btn btn-block mb-2" id="toggleButton">Desligar</a>
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
                // Código JavaScript para manipular o estado do botão e comunicação com o servidor
                document.addEventListener('DOMContentLoaded', function() {
                    var toggleButton = document.getElementById('toggleButton'); // Seleciona o botão de alternância
                    
                    function updateButtonState() {
                        // Faz uma requisição para obter o estado do compressor
                        fetch('/compressor-state')
                            .then(response => response.json()) // Converte a resposta para JSON
                            .then(data => {
                                var compressorLigado = data.compressorLigado;
                                if (compressorLigado) {
                                    // Atualiza o botão para mostrar "Desligar" se o compressor estiver ligado
                                    toggleButton.innerHTML = 'Desligar';
                                    toggleButton.classList.add('btn-desligar');
                                    toggleButton.classList.remove('btn-ligar');
                                } else {
                                    // Atualiza o botão para mostrar "Ligar" se o compressor estiver desligado
                                    toggleButton.innerHTML = 'Ligar';
                                    toggleButton.classList.add('btn-ligar');
                                    toggleButton.classList.remove('btn-desligar');
                                }
                            })
                            .catch(error => console.error('Erro ao obter estado inicial do compressor:', error)); // Tratamento de erros
                    }

                    updateButtonState(); // Atualiza o estado do botão na carga da página

                    // Adiciona um listener para o clique no botão
                    toggleButton.addEventListener('click', function(event) {
                        event.preventDefault(); // Previne o comportamento padrão do link
                        var action = toggleButton.innerHTML === 'Desligar' ? 'desligar' : 'ligar'; // Define a ação com base no texto do botão

                        // Faz uma requisição para alternar o estado do compressor
                        fetch('/toggle?action=' + action)
                            .then(response => response.text()) // Converte a resposta para texto
                            .then(data => {
                                console.log('Resposta do servidor:', data);
                                updateButtonState(); // Atualiza o estado do botão após a ação
                            })
                            .catch(error => console.error('Erro ao enviar requisição:', error)); // Tratamento de erros
                    });

                    // Verifica o estado do compressor a cada 5 segundos
                    setInterval(updateButtonState, 5000);
                });
            </script>
        </body>
        </html>
    )";

    // Configura o servidor para responder à requisição da página do dashboard
    server.on("/dashboard", HTTP_GET, [html, &server]() mutable {
        server.send(200, "text/html", html); // Envia a página HTML como resposta
    });

    // Configura o servidor para responder à requisição do estado do compressor
    server.on("/compressor-state", HTTP_GET, [&server]() {
        String stateJson = "{\"compressorLigado\":" + String(compressorLigado) + "}";
        server.send(200, "application/json", stateJson); // Envia o estado do compressor em formato JSON
    });
}

// Função para configurar a manipulação das ações de ligar/desligar o compressor
void handleToggleAction(WebServer& server)
{
    // Configura o servidor para responder à requisição de alternar o estado do compressor
    server.on("/toggle", HTTP_GET, [&server]() {
        String action = server.arg("action"); // Obtém a ação da requisição
        if (action == "ligar") {
            compressorLigado = true; // Liga o compressor
            server.send(200, "text/plain", "Compressor ligado!"); // Responde com mensagem de sucesso
        } else if (action == "desligar") {
            compressorLigado = false; // Desliga o compressor
            server.send(200, "text/plain", "Compressor desligado!"); // Responde com mensagem de sucesso
        } else {
            server.send(400, "text/plain", "Ação inválida!"); // Responde com erro se a ação for inválida
        }
    });
}
