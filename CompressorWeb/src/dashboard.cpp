#include "dashboard.h"    // Inclui o cabeçalho para a configuração do dashboard
#include "autenticador.h" // Inclui o cabeçalho onde a variável userLoggedIn é declarada
#include <WebServer.h>    // Inclui a biblioteca para criar um servidor web no ESP32
#include <FS.h>           // Inclui a biblioteca para manipulação do sistema de arquivos
#include <SPIFFS.h>       // Inclui a biblioteca para usar o sistema de arquivos SPIFFS

extern bool compressorLigado; // Declara uma variável externa para verificar o estado do compressor (declarada em outro lugar)
extern bool userLoggedIn;     // Declara a variável externa para verificar se o usuário está logado (declarada em outro lugar)

// Função para configurar a página do dashboard
void setupDashboardPage(WebServer &server)
{
    server.on("/dashboard", HTTP_GET, [&server]()
              {
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
                    font-family: Arial, sans-serif; /* Define a fonte do corpo da página */
                    background-color: #f8f9fa; /* Define a cor de fundo da página */
                    height: 100%; /* Define a altura da página como 100% da altura do container pai */
                    margin: 0; /* Remove a margem padrão */
                    display: flex; /* Usa flexbox para alinhar o conteúdo */
                    justify-content: center; /* Centraliza o conteúdo horizontalmente */
                    align-items: center; /* Centraliza o conteúdo verticalmente */
                    flex-direction: column; /* Alinha os itens na coluna */
                }

                .dashboard-container {
                    background-color: #ffffff; /* Define a cor de fundo do contêiner do dashboard */
                    padding: 20px; /* Define o preenchimento interno do contêiner */
                    border-radius: 5px; /* Adiciona bordas arredondadas ao contêiner */
                    box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); /* Adiciona uma sombra sutil ao contêiner */
                    width: 100%; /* Define a largura do contêiner como 100% */
                    max-width: 600px; /* Define a largura máxima do contêiner */
                    text-align: center; /* Alinha o texto no centro do contêiner */
                    margin-top: 20px; /* Adiciona uma margem superior ao contêiner */
                }

                .dashboard-title {
                    font-size: 28px; /* Define o tamanho da fonte do título */
                    margin-bottom: 20px; /* Adiciona uma margem inferior ao título */
                    color: #007bff; /* Define a cor do texto do título */
                }

                .btn-ligar {
                    background-color: #28a745; /* Define a cor de fundo para o botão de ligar */
                    color: white; /* Define a cor do texto do botão */
                }

                .btn-desligar {
                    background-color: #dc3545; /* Define a cor de fundo para o botão de desligar */
                    color: white; /* Define a cor do texto do botão */
                }

                .footer {
                    width: 100%; /* Define a largura do rodapé como 100% */
                    background-color: #007bff; /* Define a cor de fundo do rodapé */
                    color: white; /* Define a cor do texto do rodapé */
                    text-align: center; /* Alinha o texto no centro do rodapé */
                    padding: 10px 0; /* Define o preenchimento interno do rodapé */
                    position: fixed; /* Faz o rodapé fixo na parte inferior da página */
                    bottom: 0; /* Define a posição inferior do rodapé */
                    font-size: 14px; /* Define o tamanho da fonte do texto do rodapé */
                }
            </style>
        </head>
        <body>
            <div class="dashboard-container">
                <h2 class="dashboard-title">Bem-vindo ao Dashboard</h2>
                <a href="#" class="btn btn-block mb-2" id="toggleButton">Carregando...</a>
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

        server.send(200, "text/html", html); });

    server.on("/compressor-state", HTTP_GET, [&server]()
              {
        String stateJson = "{\"compressorLigado\":" + String(compressorLigado) + "}";
        server.send(200, "application/json", stateJson); });
}

// Função para configurar a manipulação das ações de ligar/desligar o compressor
void handleToggleAction(WebServer &server)
{
    // Configura a rota "/toggle" para responder com base na ação solicitada
    server.on("/toggle", HTTP_GET, [&server]()
              {
        // Obtém o valor do parâmetro "action" da requisição
        String action = server.arg("action");
        // Verifica a ação e atualiza o estado do compressor
        if (action == "ligar") {
            compressorLigado = true; // Liga o compressor
            server.send(200, "text/plain", "Compressor ligado!"); // Envia uma resposta indicando que o compressor foi ligado
        } else if (action == "desligar") {
            compressorLigado = false; // Desliga o compressor
            server.send(200, "text/plain", "Compressor desligado!"); // Envia uma resposta indicando que o compressor foi desligado
        } else {
            // Se a ação não for válida, envia uma resposta de erro
            server.send(400, "text/plain", "Ação inválida!"); // Envia uma resposta com o código de status 400 (solicitação incorreta)
        } });
}
