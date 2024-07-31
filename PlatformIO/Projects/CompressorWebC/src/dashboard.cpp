#include "dashboard.h" // Inclui o cabeçalho para a configuração do dashboard
#include <WebServer.h> // Biblioteca para criar um servidor web
#include <FS.h>        // Biblioteca para manipulação do sistema de arquivos
#include <SPIFFS.h>    // Biblioteca para usar o sistema de arquivos SPIFFS

extern bool compressorLigado; // Declara uma variável externa para verificar o estado do compressor

// Função para configurar a página do dashboard
void setupDashboardPage(WebServer &server)
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
                    // Seleciona o botão de alternância
                    var toggleButton = document.getElementById('toggleButton'); 

                    function updateButtonState() {
                        // Faz uma requisição para obter o estado do compressor
                        fetch('/compressor-state')
                            .then(response => response.json()) // Converte a resposta para JSON
                            .then(data => {
                                var compressorLigado = data.compressorLigado; // Obtém o estado do compressor a partir dos dados JSON
                                if (compressorLigado) {
                                    // Atualiza o botão para mostrar "Desligar" se o compressor estiver ligado
                                    toggleButton.innerHTML = 'Desligar';
                                    toggleButton.classList.add('btn-desligar'); // Adiciona a classe de botão de desligar
                                    toggleButton.classList.remove('btn-ligar'); // Remove a classe de botão de ligar
                                } else {
                                    // Atualiza o botão para mostrar "Ligar" se o compressor estiver desligado
                                    toggleButton.innerHTML = 'Ligar';
                                    toggleButton.classList.add('btn-ligar'); // Adiciona a classe de botão de ligar
                                    toggleButton.classList.remove('btn-desligar'); // Remove a classe de botão de desligar
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
                                console.log('Resposta do servidor:', data); // Exibe a resposta do servidor no console
                                updateButtonState(); // Atualiza o estado do botão após a ação
                            })
                            .catch(error => console.error('Erro ao enviar requisição:', error)); // Tratamento de erros
                    });

                    // Verifica o estado do compressor a cada 5 segundos
                    setInterval(updateButtonState, 5000); // Chama a função updateButtonState a cada 5 segundos

                    // Adiciona um listener para o evento beforeunload
                    window.addEventListener('beforeunload', function() {
                        // Faz uma requisição para a rota de logout quando a página está prestes a ser fechada
                        fetch('/logout', {
                            method: 'GET',
                            credentials: 'same-origin' // Garante que cookies de sessão sejam enviados
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

    // Configura o servidor para responder à requisição da página do dashboard
    server.on("/dashboard", HTTP_GET, [html, &server]() mutable
              {
                  server.send(200, "text/html", html); // Envia a página HTML como resposta
              });

    // Configura o servidor para responder à requisição do estado do compressor
    server.on("/compressor-state", HTTP_GET, [&server]()
              {
                  String stateJson = "{\"compressorLigado\":" + String(compressorLigado) + "}"; // Cria um JSON com o estado do compressor
                  server.send(200, "application/json", stateJson);                              // Envia o estado do compressor em formato JSON
              });
}

// Função para configurar a manipulação das ações de ligar/desligar o compressor
void handleToggleAction(WebServer &server)
{
    // Configura o servidor para responder à requisição de alternar o estado do compressor
    server.on("/toggle", HTTP_GET, [&server]()
              {
        String action = server.arg("action"); // Obtém a ação da requisição
        if (action == "ligar") {
            compressorLigado = true; // Liga o compressor
            server.send(200, "text/plain", "Compressor ligado!"); // Responde com mensagem de sucesso
        } else if (action == "desligar") {
            compressorLigado = false; // Desliga o compressor
            server.send(200, "text/plain", "Compressor desligado!"); // Responde com mensagem de sucesso
        } else {
            server.send(400, "text/plain", "Ação inválida!"); // Responde com erro se a ação for inválida
        } });
}
