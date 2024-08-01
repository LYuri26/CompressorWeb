#include "dashboard.h" // Inclui o cabeçalho para a configuração do dashboard
#include "autenticador.h" // Inclui o cabeçalho onde userLoggedIn é declarado
#include "autenticador.h"
#include <WebServer.h> // Biblioteca para criar um servidor web
#include <FS.h>       // Biblioteca para manipulação do sistema de arquivos
#include <SPIFFS.h>   // Biblioteca para usar o sistema de arquivos SPIFFS

extern bool compressorLigado; // Declara uma variável externa para verificar o estado do compressor
extern bool userLoggedIn; // Declara a variável externa para verificar se o usuário está logado

// Função para configurar a página do dashboard
void setupDashboardPage(WebServer& server)
{
    server.on("/dashboard", HTTP_GET, [&server]() {
        if (!userLoggedIn) {
            server.sendHeader("Location", "/acesso-invalido");
            server.send(302, "text/plain", "");
            return;
        }

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
                <a href="/umidade" class="btn btn-secondary btn-block mb-2">Umidade</a>
                <a href="/oleo" class="btn btn-secondary btn-block mb-2">Nível de Óleo</a>
                <a href="/logout" class="btn btn-danger btn-block mt-3">Logout</a>
            </div>

            <div class="footer">
                <p>Aplicação desenvolvida pela Turma de Informática Para Internet Trilhas de Futuro 2024</p>
                <p>Instrutor: Lenon Yuri</p>
            </div>

            <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.5.1/jquery.min.js"></script>
            <script src="https://maxcdn.bootstrapcdn.com/bootstrap/4.5.2/js/bootstrap.min.js"></script>
            <script>
                document.addEventListener('DOMContentLoaded', function() {
                    var toggleButton = document.getElementById('toggleButton');
                    
                    function updateButtonState() {
                        fetch('/compressor-state')
                            .then(response => response.json())
                            .then(data => {
                                var compressorLigado = data.compressorLigado;
                                if (compressorLigado) {
                                    toggleButton.innerHTML = 'Desligar';
                                    toggleButton.classList.add('btn-desligar');
                                    toggleButton.classList.remove('btn-ligar');
                                } else {
                                    toggleButton.innerHTML = 'Ligar';
                                    toggleButton.classList.add('btn-ligar');
                                    toggleButton.classList.remove('btn-desligar');
                                }
                            })
                            .catch(error => console.error('Erro ao obter estado inicial do compressor:', error));
                    }

                    updateButtonState();

                    toggleButton.addEventListener('click', function(event) {
                        event.preventDefault();
                        var action = toggleButton.innerHTML === 'Desligar' ? 'desligar' : 'ligar';

                        fetch('/toggle?action=' + action)
                            .then(response => response.text())
                            .then(data => {
                                console.log('Resposta do servidor:', data);
                                updateButtonState();
                            })
                            .catch(error => console.error('Erro ao enviar requisição:', error));
                    });

                    setInterval(updateButtonState, 5000);
                });
            </script>
        </body>
        </html>
        )";

        server.send(200, "text/html", html);
    });
    
    server.on("/compressor-state", HTTP_GET, [&server]() {
        String stateJson = "{\"compressorLigado\":" + String(compressorLigado) + "}";
        server.send(200, "application/json", stateJson);
    });
}

// Função para configurar a manipulação das ações de ligar/desligar o compressor
void handleToggleAction(WebServer& server)
{
    server.on("/toggle", HTTP_GET, [&server]() {
        String action = server.arg("action");
        if (action == "ligar") {
            compressorLigado = true;
            server.send(200, "text/plain", "Compressor ligado!");
        } else if (action == "desligar") {
            compressorLigado = false;
            server.send(200, "text/plain", "Compressor desligado!");
        } else {
            server.send(400, "text/plain", "Ação inválida!");
        }
    });
}
