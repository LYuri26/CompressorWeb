#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <SPIFFS.h>
#include "dashboard.h"    // Inclui o cabeçalho para a configuração do dashboard
#include "ligadesliga.h"  // Inclui o cabeçalho para a configuração do compressor
#include "autenticador.h" // Inclui o cabeçalho onde a variável userLoggedIn é declarada

// -------------------------------------------------------------------------
// Função para configurar a página do dashboard
// -------------------------------------------------------------------------

/**
 * Configura a rota para a página do dashboard no servidor Web.
 *
 * @param server A instância do servidor web assíncrono.
 */
void setupDashboardPage(AsyncWebServer& server) {
    // Configura o endpoint para a página do dashboard
    server.on("/dashboard", HTTP_GET, [](AsyncWebServerRequest *request) {
        // Verifica se o usuário está autenticado
        if (!isAuthenticated(request)) {
            redirectToAccessDenied(request);
            return;
        }

        // Define o conteúdo HTML da página do dashboard
        String html = R"rawliteral(
            <!DOCTYPE html>
            <html lang="pt-br">
            <head>
                <meta charset="UTF-8">
                <meta name="viewport" content="width=device-width, initial-scale=1.0">
                <title>Dashboard</title>
                <!-- Inclui o Bootstrap para estilização -->
                <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css">
                <style>
                    /* Estilo da página */
                    body {
                        font-family: Arial, sans-serif; /* Define a fonte padrão da página */
                        background-color: #f8f9fa; /* Cor de fundo da página */
                        height: 100%; /* Altura da página igual ao tamanho do conteúdo */
                        margin: 0; /* Remove margens padrão */
                        display: flex; /* Usa flexbox para centralizar o conteúdo */
                        justify-content: center; /* Alinha horizontalmente no centro */
                        align-items: center; /* Alinha verticalmente no centro */
                        flex-direction: column; /* Alinha os itens em coluna */
                    }

                    .dashboard-container {
                        background-color: #ffffff; /* Cor de fundo do container do dashboard */
                        padding: 20px; /* Espaçamento interno do container */
                        border-radius: 5px; /* Bordas arredondadas */
                        box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); /* Sombra ao redor do container */
                        width: 100%; /* Largura total do container */
                        max-width: 600px; /* Largura máxima do container */
                        text-align: center; /* Alinha o texto ao centro */
                        margin-top: 20px; /* Espaçamento acima do container */
                    }

                    .dashboard-title {
                        font-size: 28px; /* Tamanho da fonte do título */
                        margin-bottom: 20px; /* Espaçamento abaixo do título */
                        color: #007bff; /* Cor do título */
                    }

                    .btn-ligar {
                        background-color: #28a745; /* Cor de fundo do botão de ligar */
                        color: white; /* Cor do texto do botão */
                    }

                    .btn-desligar {
                        background-color: #dc3545; /* Cor de fundo do botão de desligar */
                        color: white; /* Cor do texto do botão */
                    }

                    .footer {
                        width: 100%; /* Largura total do rodapé */
                        background-color: #007bff; /* Cor de fundo do rodapé */
                        color: white; /* Cor do texto do rodapé */
                        text-align: center; /* Alinha o texto ao centro */
                        padding: 10px 0; /* Espaçamento interno do rodapé */
                        position: fixed; /* Fixa o rodapé na parte inferior */
                        bottom: 0; /* Posiciona o rodapé na parte inferior */
                        font-size: 14px; /* Tamanho da fonte do rodapé */
                    }
                </style>
            </head>
            <body>
                <!-- Container principal do dashboard -->
                <div class="dashboard-container">
                    <h2 class="dashboard-title">Bem-vindo ao Dashboard</h2>
                    <!-- Botão para ligar/desligar o compressor -->
                    <a href="#" class="btn btn-block mb-2" id="toggleButton">Carregando...</a>
                    <!-- Links para outras páginas -->
                    <a href="/umidade" class="btn btn-secondary btn-block mb-2">Umidade</a>
                    <a href="/oleo" class="btn btn-secondary btn-block mb-2">Nível de Óleo</a>
                    <a href="/logout" class="btn btn-danger btn-block mt-3">Logout</a>
                </div>
                <!-- Rodapé da página -->
                <div class="footer">
                    <p>Aplicação desenvolvida pela Turma de Informática Para Internet Trilhas de Futuro 2024</p>
                    <p>Instrutor: Lenon Yuri</p>
                </div>
                <!-- Inclusão de jQuery e Bootstrap JavaScript -->
                <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.5.1/jquery.min.js"></script>
                <script src="https://maxcdn.bootstrapcdn.com/bootstrap/4.5.2/js/bootstrap.min.js"></script>
                <script>
                    document.addEventListener('DOMContentLoaded', function() {
                        var toggleButton = document.getElementById('toggleButton');

                        /**
                         * Atualiza o estado do botão com base no estado do compressor.
                         */
                        function updateButtonState() {
                            fetch('/compressor-state')
                                .then(response => response.json())
                                .then(data => {
                                    var compressorLigado = data.compressorLigado;
                                    if (compressorLigado) {
                                        toggleButton.innerHTML = 'Desligar'; // Atualiza o texto do botão
                                        toggleButton.classList.add('btn-desligar'); // Adiciona a classe para cor de fundo
                                        toggleButton.classList.remove('btn-ligar'); // Remove a classe anterior
                                    } else {
                                        toggleButton.innerHTML = 'Ligar'; // Atualiza o texto do botão
                                        toggleButton.classList.add('btn-ligar'); // Adiciona a classe para cor de fundo
                                        toggleButton.classList.remove('btn-desligar'); // Remove a classe anterior
                                    }
                                })
                                .catch(error => console.error('Erro ao obter estado inicial do compressor:', error));
                        }

                        /**
                         * Verifica a autenticação do usuário e redireciona se não estiver autenticado.
                         */
                        function checkAuthentication() {
                            fetch('/check-auth')
                                .then(response => response.json())
                                .then(data => {
                                    if (!data.authenticated) {
                                        window.location.href = '/acesso-invalido'; // Redireciona para a página de acesso inválido
                                    }
                                })
                                .catch(error => console.error('Erro ao verificar autenticação:', error));
                        }

                        /**
                         * Configura o botão para alternar o estado do compressor.
                         */
                        toggleButton.addEventListener('click', function(event) {
                            event.preventDefault(); // Previne o comportamento padrão do link
                            var action = toggleButton.innerHTML === 'Desligar' ? 'desligar' : 'ligar'; // Define a ação com base no texto do botão
                            fetch('/toggle?action=' + action)
                                .then(response => response.text())
                                .then(data => {
                                    console.log('Resposta do servidor:', data); // Exibe a resposta do servidor
                                    updateButtonState(); // Atualiza o estado do botão após a requisição
                                })
                                .catch(error => console.error('Erro ao enviar requisição:', error));
                        });

                        // Atualiza o estado do botão a cada 5 segundos
                        setInterval(updateButtonState, 5000);

                        // Inicializa o estado do botão e verifica a autenticação
                        updateButtonState();
                        checkAuthentication();
                    });
                </script>
            </body>
            </html>
        )rawliteral";

        // Envia a resposta HTML ao cliente
        request->send(200, "text/html", html);
    });

    // Configura o endpoint para retornar o estado do compressor em formato JSON
    server.on("/compressor-state", HTTP_GET, [](AsyncWebServerRequest *request) {
        // Cria um JSON com o estado atual do compressor
        String stateJson = "{\"compressorLigado\":" + String(compressorLigado) + "}";
        // Envia o JSON como resposta
        request->send(200, "application/json", stateJson);
    });
}
