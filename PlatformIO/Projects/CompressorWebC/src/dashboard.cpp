#include "dashboard.h" // Inclui o cabeçalho para a configuração do dashboard
#include "logado.h" // Inclui o cabeçalho para a configuração do dashboard
#include <WebServer.h> // Biblioteca para criar um servidor web
#include <FS.h>        // Biblioteca para manipulação do sistema de arquivos
#include <SPIFFS.h>    // Biblioteca para usar o sistema de arquivos SPIFFS


extern bool compressorLigado; // Declara uma variável externa para verificar o estado do compressor

// Função para obter o HTML da página do dashboard
String getDashboardPage() {
    return R"(
        <!DOCTYPE html>
        <html lang="pt-br">
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>Dashboard</title>
            <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css">
            <style>
                body {
                    font-family: Arial, sans-serif; /* Define a fonte do texto */
                    background-color: #f8f9fa; /* Define a cor de fundo da página */
                    height: 100%; /* Define a altura da página como 100% da tela */
                    margin: 0; /* Remove a margem padrão do corpo */
                    display: flex; /* Define a exibição do corpo como flexível */
                    justify-content: center; /* Centraliza o conteúdo horizontalmente */
                    align-items: center; /* Centraliza o conteúdo verticalmente */
                    flex-direction: column; /* Organiza o conteúdo na vertical */
                }

                .dashboard-container {
                    background-color: #ffffff; /* Define a cor de fundo da caixa do dashboard */
                    padding: 20px; /* Adiciona preenchimento interno de 20 pixels */
                    border-radius: 5px; /* Adiciona bordas arredondadas */
                    box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); /* Adiciona sombra ao redor da caixa */
                    width: 100%; /* Define a largura da caixa como 100% do seu contêiner pai */
                    max-width: 600px; /* Define a largura máxima da caixa como 600 pixels */
                    text-align: center; /* Alinha o texto ao centro */
                    margin-top: 20px; /* Adiciona uma margem superior de 20 pixels */
                }

                .dashboard-title {
                    font-size: 28px; /* Define o tamanho da fonte do título */
                    margin-bottom: 20px; /* Adiciona uma margem inferior de 20 pixels */
                    color: #007bff; /* Define a cor do texto do título */
                }

                .btn-ligar {
                    background-color: #28a745; /* Define a cor de fundo do botão de ligar */
                    color: white; /* Define a cor do texto do botão */
                }

                .btn-desligar {
                    background-color: #dc3545; /* Define a cor de fundo do botão de desligar */
                    color: white; /* Define a cor do texto do botão */
                }

                .footer {
                    width: 100%; /* Define a largura do rodapé como 100% do contêiner pai */
                    background-color: #007bff; /* Define a cor de fundo do rodapé */
                    color: white; /* Define a cor do texto do rodapé */
                    text-align: center; /* Alinha o texto ao centro */
                    padding: 10px 0; /* Adiciona preenchimento vertical de 10 pixels */
                    position: fixed; /* Define a posição do rodapé como fixa */
                    bottom: 0; /* Posiciona o rodapé na parte inferior da página */
                    font-size: 14px; /* Define o tamanho da fonte do texto do rodapé */
                }
            </style>
        </head>
        <body>
            <div class="dashboard-container">
                <h2 class="dashboard-title">Bem-vindo ao Dashboard</h2> <!-- Título da página -->
                <a href="#" class="btn btn-block mb-2" id="toggleButton">Desligar</a> <!-- Botão para ligar/desligar o compressor -->
                <a href="/umidade" class="btn btn-secondary btn-block mb-2">Umidade</a> <!-- Botão para visualizar a umidade -->
                <a href="/oleo" class="btn btn-secondary btn-block mb-2">Nível de Óleo</a> <!-- Botão para visualizar o nível de óleo -->
                <a href="/logout" class="btn btn-danger btn-block mt-3">Logout</a> <!-- Botão para logout -->
            </div>

            <div class="footer">
                <p>Aplicação desenvolvida pela Turma de Informática Para Internet Trilhas de Futuro 2024</p> <!-- Texto informativo no rodapé -->
                <p>Instrutor: Lenon Yuri</p> <!-- Nome do instrutor no rodapé -->
            </div>

            <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.5.1/jquery.min.js"></script> <!-- Inclui a biblioteca jQuery -->
            <script src="https://maxcdn.bootstrapcdn.com/bootstrap/4.5.2/js/bootstrap.min.js"></script> <!-- Inclui o JavaScript do Bootstrap -->
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

                    window.addEventListener('beforeunload', function() {
                        fetch('/logout', {
                            method: 'GET',
                            credentials: 'same-origin'
                        })
                        .then(response => response.text())
                        .then(data => console.log('Logout enviado:', data))
                        .catch(error => console.error('Erro ao enviar requisição de logout:', error));
                    });
                });
            </script>
        </body>
        </html>
    )";
}

// Função para configurar a página do dashboard
void setupDashboardPage(WebServer &server) {
    server.on("/dashboard", HTTP_GET, [&server]() {
        if (!isLoggedIn) {
            server.send(403, "text/plain", "Local inválido");
        } else {
            server.send(200, "text/html", getDashboardPage());
        }
    });

    server.on("/compressor-state", HTTP_GET, [&server]() {
        String stateJson = "{\"compressorLigado\":" + String(compressorLigado) + "}";
        server.send(200, "application/json", stateJson);
    });
}

// Função para configurar a manipulação das ações de ligar/desligar o compressor
void handleToggleAction(WebServer &server) {
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
