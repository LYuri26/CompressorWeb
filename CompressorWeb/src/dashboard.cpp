#include <ESPAsyncWebServer.h> // Biblioteca para servidor web assíncrono
#include <FS.h>                // Biblioteca para sistema de arquivos
#include <SPIFFS.h>            // Biblioteca para sistema de arquivos SPIFFS
#include "dashboard.h"         // Inclui o cabeçalho para a configuração do dashboard
#include "ligadesliga.h"       // Inclui o cabeçalho para a configuração do compressor
#include "autenticador.h"      // Inclui o cabeçalho onde a variável userLoggedIn é declarada
#include "manutencao.h"        // Inclui o cabeçalho para a configuração de manutenção

// -------------------------------------------------------------------------
// Função para configurar a página do dashboard
// -------------------------------------------------------------------------

/**
 * Configura a rota para a página do dashboard no servidor Web.
 *
 * @param server A instância do servidor web assíncrono.
 */
void setupDashboardPage(AsyncWebServer &server)
{
    // Configura o endpoint para a página do dashboard
    server.on("/dashboard", HTTP_GET, [](AsyncWebServerRequest *request)
              {
                  // Verifica se o usuário está autenticado
                  if (!isAuthenticated(request))
                  {
                      redirectToAccessDenied(request); // Redireciona para a página de acesso negado se não estiver autenticado
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

        .btn-disabled {
            background-color: #ffa500; /* Cor de fundo do botão em manutenção */
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
    <div id="messageBox" class="mt-3"></div>
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
            var toggleButton = document.getElementById('toggleButton'); // Botão para ligar/desligar o compressor
            var messageBox = document.getElementById('messageBox');     // Caixa de mensagens
            var previousMaintenanceState = null;                        // Variável para armazenar o estado anterior de manutenção

            // Função para atualizar o estado do botão
            function updateButtonState() {
                fetch('/compressor-state') // Faz uma requisição para obter o estado do compressor
                    .then(response => response.json())
                    .then(data => {
                        var compressorLigado = data.compressorLigado; // Estado do compressor (ligado/desligado)
                        var sistemaEmManutencao = data.sistemaEmManutencao; // Estado de manutenção do sistema
                        var horaAtual = new Date().getHours() + (new Date().getMinutes() / 60); // Hora atual em formato decimal

                        if (sistemaEmManutencao) {
                            toggleButton.innerHTML = 'Compressor em manutenção'; // Texto do botão
                            toggleButton.classList.add('btn-disabled'); // Adiciona classe de botão desabilitado
                            toggleButton.classList.remove('btn-ligar', 'btn-desligar'); // Remove outras classes de botão
                            messageBox.innerHTML = ''; // Limpa mensagens anteriores
                        } else {
                            if (compressorLigado) {
                                toggleButton.innerHTML = 'Desligar'; // Texto do botão
                                toggleButton.classList.add('btn-desligar'); // Adiciona classe de botão para desligar
                                toggleButton.classList.remove('btn-ligar'); // Remove classe de botão para ligar
                                messageBox.innerHTML = 'O compressor acabou de ser ligado, aguarde o tempo limite para desligar novamente.'; // Mensagem
                            } else {
                                toggleButton.innerHTML = 'Ligar'; // Texto do botão
                                toggleButton.classList.add('btn-ligar'); // Adiciona classe de botão para ligar
                                toggleButton.classList.remove('btn-desligar'); // Remove classe de botão para desligar
                                messageBox.innerHTML = ''; // Limpa mensagens anteriores
                            }

                            // Mensagens baseadas no horário atual
                            if (horaAtual < 7.5 || horaAtual >= 22.5) { // Verifica se está fora do horário de funcionamento
                                messageBox.innerHTML = 'Compressor desligado devido ao horário de funcionamento.';
                            } else if (horaAtual >= 7.5 && horaAtual < 8) { // Verifica se está entre 7:30 e 8:00
                                messageBox.innerHTML = 'Compressor Ligado após o horário de funcionamento, desligue após o uso.';
                            }
                        }

                        // Recarregar a página se o estado de manutenção mudar de true para false
                        if (previousMaintenanceState !== null && previousMaintenanceState !== sistemaEmManutencao && !sistemaEmManutencao) {
                            location.reload(); // Recarrega a página
                        }

                        // Atualizar o estado anterior de manutenção
                        previousMaintenanceState = sistemaEmManutencao;
                    })
                    .catch(error => console.error('Erro ao obter estado inicial do compressor:', error)); // Exibe erro no console se houver
            }
                // Adiciona um evento de clique no botão para alternar o estado do compressor
                toggleButton.addEventListener('click', function(event) {
                    event.preventDefault(); // Previne o comportamento padrão do link

                    // Verifica se o botão está desabilitado e retorna se estiver
                    if (toggleButton.classList.contains('btn-disabled')) {
                        return; // Não faz nada se o botão estiver desativado
                    }

                    // Determina a ação com base no texto atual do botão
                    var action = toggleButton.innerHTML === 'Desligar' ? 'desligar' : 'ligar';

                    // Faz uma requisição para alternar o estado do compressor
                    fetch('/toggle?action=' + action)
                        .then(response => response.text()) // Obtém a resposta como texto
                        .then(data => {
                            console.log('Resposta do servidor:', data); // Exibe a resposta do servidor no console
                            updateButtonState(); // Atualiza o estado do botão após a ação
                        })
                        .catch(error => console.error('Erro ao enviar requisição:', error)); // Exibe erro no console se houver
                });

                // Atualiza o estado do botão a cada 5 segundos
                setInterval(updateButtonState, 5000);

                // Atualiza o estado do botão imediatamente ao carregar a página
                updateButtonState();
            });
        </script>
    </body>
</html>
        )rawliteral";

        // Envia a resposta HTML ao cliente
        request->send(200, "text/html", html); // Envia o conteúdo HTML como resposta ao cliente
    });

    // Configura o endpoint para retornar o estado do compressor em formato JSON
    server.on("/compressor-state", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        // Cria um JSON com o estado atual do compressor e do sistema de manutenção
        String stateJson = "{\"compressorLigado\":" + String(compressorLigado) + ",\"sistemaEmManutencao\":" + String(sistemaEmManutencao) + "}";
        // Envia o JSON como resposta
        request->send(200, "application/json", stateJson); // Envia o JSON como resposta ao cliente
    });
}
