#include <ESPAsyncWebServer.h> // Inclui a biblioteca necessária para criar um servidor web assíncrono no ESP8266/ESP32
#include <FS.h>                // Inclui a biblioteca para manipulação do sistema de arquivos
#include <SPIFFS.h>            // Inclui a biblioteca para usar o SPIFFS (SPI Flash File System)
#include "dashboard.h"         // Inclui o cabeçalho com definições específicas para o dashboard
#include "ligadesliga.h"       // Inclui o cabeçalho para a lógica de ligar e desligar dispositivos
#include "autenticador.h"      // Inclui o cabeçalho para autenticação de usuários
#include "manutencao.h"        // Inclui o cabeçalho para manutenção do sistema

// -------------------------------------------------------------------------
// Configura a rota para a página do dashboard
// -------------------------------------------------------------------------

/**
 * Configura a rota para a página do dashboard e os endpoints relacionados.
 *
 * @param server A instância do servidor web assíncrono.
 */
void setupDashboardPage(AsyncWebServer &server)
{
    // Define a rota para "/dashboard" e trata o pedido HTTP GET
    server.on("/dashboard", HTTP_GET, [](AsyncWebServerRequest *request)
              {
                  // Verifica se o usuário está autenticado
                  if (!isAuthenticated(request))
                  {
                      redirectToAccessDenied(request); // Redireciona para uma página de acesso negado se não estiver autenticado
                      return;
                  }

                  // HTML da página do dashboard
                  String html = R"rawliteral(
<!DOCTYPE html> <!-- Declara o tipo de documento como HTML5 -->
<html lang="pt-br"> <!-- Define o idioma da página como português do Brasil -->
<head>
    <meta charset="UTF-8"> <!-- Define o conjunto de caracteres como UTF-8 -->
    <meta name="viewport" content="width=device-width, initial-scale=1.0"> <!-- Ajusta a escala da página para dispositivos móveis -->
    <title>Dashboard</title> <!-- Define o título da aba do navegador -->
    <style>
        /* Estilo geral da página */
        body {
            font-family: Arial, sans-serif; /* Define a fonte padrão da página */
            background-color: #f8f9fa; /* Define a cor de fundo da página */
            margin: 0; /* Remove a margem padrão */
            padding: 0; /* Remove o preenchimento padrão */
            display: flex; /* Usa Flexbox para alinhamento */
            justify-content: center; /* Alinha o conteúdo horizontalmente no centro */
            align-items: center; /* Alinha o conteúdo verticalmente no centro */
            height: 100vh; /* Define a altura da página como 100% da altura da viewport */
            flex-direction: column; /* Organiza o layout na vertical */
        }

        /* Estilo do container do dashboard */
        .dashboard-container {
            background-color: #ffffff; /* Define a cor de fundo do container */
            padding: 20px; /* Adiciona preenchimento interno ao container */
            border-radius: 8px; /* Adiciona bordas arredondadas ao container */
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1); /* Adiciona uma sombra leve ao container */
            text-align: center; /* Centraliza o texto dentro do container */
            width: 100%; /* Define a largura do container como 100% do pai */
            max-width: 400px; /* Define a largura máxima do container */
        }

        /* Estilo do título do dashboard */
        .dashboard-title {
            font-size: 28px; /* Define o tamanho da fonte do título */
            margin-bottom: 20px; /* Adiciona uma margem inferior ao título */
            color: #004085; /* Define a cor do título */
        }

        /* Estilo geral dos botões */
        .btn {
            display: block; /* Define o botão como um bloco em linha */
            width: calc(100% - 24px); /* Ajusta a largura do botão para 100% menos a margem */
            padding: 12px; /* Adiciona preenchimento interno ao botão */
            font-size: 16px; /* Define o tamanho da fonte do botão */
            margin: 10px auto; /* Adiciona margem superior e inferior e centraliza o botão */
            border: none; /* Remove a borda padrão do botão */
            border-radius: 8px; /* Adiciona bordas arredondadas ao botão */
            cursor: pointer; /* Define o cursor como ponteiro ao passar sobre o botão */
            color: white; /* Define a cor do texto do botão como branca */
            transition: background-color 0.3s, transform 0.3s; /* Adiciona uma transição suave para a cor de fundo e transformação */
        }

        /* Estilo específico para os botões de motor */
        .btn-motor1 {
            background-color: #04997d; /* Define a cor de fundo para o botão do Motor1 */
        }

        .btn-motor2 {
            background-color: #2ae6d4; /* Define a cor de fundo para o botão do Motor2 */
        }

        .btn-motor3 {
            background-color: #18df94; /* Define a cor de fundo para o botão do Motor3 */
        }

        /* Estilo específico para o botão de desligar */
        .btn-desligar {
            background-color: #c82333; /* Define a cor de fundo para o botão de desligar */
        }

        /* Estilo para botões desativados */
        .btn-disabled {
            background-color: #6c757d; /* Define a cor de fundo para botões desativados */
            cursor: not-allowed; /* Define o cursor como não permitido para botões desativados */
        }

        /* Estilo geral dos links de botão */
        .btn-link {
            display: block; /* Define o link como um bloco em linha */
            width: calc(100% - 24px); /* Ajusta a largura do link para 100% menos a margem */
            padding: 12px; /* Adiciona preenchimento interno ao link */
            font-size: 16px; /* Define o tamanho da fonte do link */
            margin: 10px auto; /* Adiciona margem superior e inferior e centraliza o link */
            border-radius: 8px; /* Adiciona bordas arredondadas ao link */
            text-decoration: none; /* Remove o sublinhado padrão dos links */
            cursor: pointer; /* Define o cursor como ponteiro ao passar sobre o link */
            color: white; /* Define a cor do texto do link como branca */
            transition: background-color 0.3s, transform 0.3s; /* Adiciona uma transição suave para a cor de fundo e transformação */
        }

        /* Estilos específicos para links de botão */
        .btn-link-umidade {
            background-color: #004085; /* Define a cor de fundo para o link de umidade */
        }

        .btn-link-pressao {
            background-color: #343a40; /* Define a cor de fundo para o link de pressão */
        }

        .btn-link-logout {
            background-color: #c82333; /* Define a cor de fundo para o link de logout */
        }

        /* Efeito de hover para links de botão */
        .btn-link:hover {
            opacity: 0.8; /* Adiciona um efeito de opacidade ao passar o mouse sobre o link */
            transform: scale(1.02); /* Aplica um efeito de zoom ao passar o mouse sobre o link */
        }

        /* Estilo do rodapé */
        .footer {
            width: 100%; /* Define a largura do rodapé como 100% da viewport */
            background-color: #004085; /* Define a cor de fundo do rodapé */
            color: white; /* Define a cor do texto do rodapé como branca */
            text-align: center; /* Centraliza o texto dentro do rodapé */
            padding: 10px 0; /* Adiciona preenchimento interno ao rodapé */
            position: fixed; /* Fixar o rodapé na parte inferior da página */
            bottom: 0; /* Alinha o rodapé na parte inferior da página */
            font-size: 14px; /* Define o tamanho da fonte do rodapé */
        }

        /* Estilo da caixa de mensagens */
        #messageBox {
            margin-top: 15px; /* Adiciona uma margem superior à caixa de mensagens */
            font-size: 16px; /* Define o tamanho da fonte da caixa de mensagens */
            color: #c82333; /* Define a cor do texto da caixa de mensagens */
        }
    </style>
</head>
<body>
    <div class="dashboard-container"> <!-- Container para o conteúdo do dashboard -->
        <h2 class="dashboard-title">Bem-vindo ao Dashboard</h2> <!-- Título do dashboard -->
        <button class="btn btn-motor1" id="toggleButtonMotor1">Carregando...</button> <!-- Botão para o Motor1 -->
        <button class="btn btn-motor2" id="toggleButtonMotor2">Carregando...</button> <!-- Botão para o Motor2 -->
        <button class="btn btn-motor3" id="toggleButtonMotor3">Carregando...</button> <!-- Botão para o Motor3 -->
        <a href="/umidade" class="btn btn-link btn-link-umidade">Umidade</a> <!-- Link para a página de umidade -->
        <a href="/pressao" class="btn btn-link btn-link-pressao">Pressão</a> <!-- Link para a página de pressão -->
        <a href="/logout" class="btn btn-link btn-link-logout">Logout</a> <!-- Link para a página de logout -->
    </div>
    <div id="messageBox"></div> <!-- Caixa para exibir mensagens de status -->
    <div class="footer"> <!-- Rodapé da página -->
        <p>Aplicação desenvolvida pela Turma de Informática Para Internet Trilhas de Futuro 2024</p> <!-- Mensagem do rodapé -->
        <p>Instrutor: Lenon Yuri</p> <!-- Nome do instrutor -->
    </div>
    <script>
        // -------------------------------------------------------------------------
        // Script JavaScript para a interatividade da página
        // -------------------------------------------------------------------------

        document.addEventListener('DOMContentLoaded', function() {
            // Obtém referências aos elementos de botão e mensagem
            var toggleButtonMotor1 = document.getElementById('toggleButtonMotor1');
            var toggleButtonMotor2 = document.getElementById('toggleButtonMotor2');
            var toggleButtonMotor3 = document.getElementById('toggleButtonMotor3');
            var messageBox = document.getElementById('messageBox');
            var previousMaintenanceState = null; // Armazena o estado de manutenção anterior para comparação

            /**
             * Atualiza o estado do botão com base na resposta do servidor.
             *
             * @param button O botão a ser atualizado.
             * @param motor O identificador do motor.
             * @param buttonClass A classe do botão para aplicar o estilo.
             */
            function updateButtonState(button, motor, buttonClass) {
                fetch('/compressor-state') // Faz uma requisição GET para obter o estado do compressor
                    .then(response => response.json()) // Converte a resposta em JSON
                    .then(data => {
                        // Obtém o estado do compressor e do sistema de manutenção
                        var compressorLigado = data['compressorLigado' + motor];
                        var sistemaEmManutencao = data.sistemaEmManutencao;
                        var horaAtual = new Date().getHours() + (new Date().getMinutes() / 60); // Calcula a hora atual em decimal

                        // Atualiza o estado do botão com base no sistema de manutenção e no estado do compressor
                        if (sistemaEmManutencao) {
                            button.innerHTML = 'Compressor ' + motor + ' em manutenção'; // Mensagem para manutenção
                            button.classList.add('btn-disabled'); // Adiciona classe desativada
                            button.classList.remove(buttonClass, 'btn-desligar'); // Remove classes de botão ativo e de desligar
                            messageBox.innerHTML = ''; // Limpa a caixa de mensagem
                        } else {
                            if (compressorLigado) {
                                button.innerHTML = 'Desligar ' + motor; // Mensagem para desligar o compressor
                                button.classList.add('btn-desligar'); // Adiciona classe de desligar
                                button.classList.remove(buttonClass); // Remove a classe do botão ativo
                                messageBox.innerHTML = 'O compressor ' + motor + ' acabou de ser ligado, aguarde o tempo limite para desligar novamente.';
                            } else {
                                button.innerHTML = 'Ligar ' + motor; // Mensagem para ligar o compressor
                                button.classList.remove('btn-desligar'); // Remove a classe de desligar
                                button.classList.add(buttonClass); // Adiciona a classe do botão ativo
                                messageBox.innerHTML = ''; // Limpa a caixa de mensagem
                            }

                            // Adiciona mensagens de status baseadas no horário
                            if (horaAtual < 7.5 || horaAtual >= 22.5) {
                                messageBox.innerHTML = 'Compressor ' + motor + ' desligado devido ao horário de funcionamento.';
                            } else if (horaAtual >= 7.5 && horaAtual < 8) {
                                messageBox.innerHTML = 'Compressor ' + motor + ' ligado após o horário de funcionamento, desligue após o uso.';
                            }
                        }

                        // Recarrega a página se o estado de manutenção mudou e o sistema não está mais em manutenção
                        if (previousMaintenanceState !== null && previousMaintenanceState !== sistemaEmManutencao && !sistemaEmManutencao) {
                            location.reload(); // Recarrega a página para atualizar o estado dos botões
                        }

                        previousMaintenanceState = sistemaEmManutencao; // Atualiza o estado de manutenção anterior
                    })
                    .catch(error => console.error('Erro ao obter estado inicial do compressor:', error)); // Loga o erro se ocorrer
            }

            /**
             * Configura o evento de clique para um botão específico.
             *
             * @param button O botão a ser configurado.
             * @param motor O identificador do motor.
             * @param buttonClass A classe do botão para aplicar o estilo.
             */
            function setupButtonClick(button, motor, buttonClass) {
                button.addEventListener('click', function(event) {
                    event.preventDefault(); // Previne o comportamento padrão do link
                    if (button.classList.contains('btn-disabled')) {
                        return; // Não faz nada se o botão estiver desativado
                    }

                    var action = button.innerHTML.includes('Desligar') ? 'desligar' : 'ligar'; // Define a ação com base no texto do botão

                    fetch('/toggle?action=' + action + '&motor=' + motor) // Faz uma requisição para alternar o estado do motor
                        .then(response => response.text()) // Converte a resposta para texto
                        .then(data => {
                            console.log('Resposta do servidor:', data); // Loga a resposta do servidor
                            updateButtonState(button, motor, buttonClass); // Atualiza o estado do botão após a ação
                        })
                        .catch(error => console.error('Erro ao enviar requisição:', error)); // Loga o erro se ocorrer
                });
            }

            // Configura os eventos de clique para os botões dos motores
            setupButtonClick(toggleButtonMotor1, 'Motor1', 'btn-motor1');
            setupButtonClick(toggleButtonMotor2, 'Motor2', 'btn-motor2');
            setupButtonClick(toggleButtonMotor3, 'Motor3', 'btn-motor3');

            // Atualiza o estado dos botões a cada 5 segundos
            setInterval(() => {
                updateButtonState(toggleButtonMotor1, 'Motor1', 'btn-motor1');
                updateButtonState(toggleButtonMotor2, 'Motor2', 'btn-motor2');
                updateButtonState(toggleButtonMotor3, 'Motor3', 'btn-motor3');
            }, 5000);

            // Atualiza o estado dos botões ao carregar a página
            updateButtonState(toggleButtonMotor1, 'Motor1', 'btn-motor1');
            updateButtonState(toggleButtonMotor2, 'Motor2', 'btn-motor2');
            updateButtonState(toggleButtonMotor3, 'Motor3', 'btn-motor3');
        });
    </script>
</body>
</html>
        )rawliteral";

                  // Envia o HTML para o cliente com um status de resposta 200 (OK)
                  request->send(200, "text/html", html); });

    // -------------------------------------------------------------------------
    // Rota para obter o estado dos motores e do sistema de manutenção
    // -------------------------------------------------------------------------
    server.on("/compressor-state", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        // Monta o JSON com o estado dos motores e do sistema de manutenção
        String stateJson = "{\"compressorLigadoMotor1\":" + String(motoresLigados[0]) + 
                            ",\"compressorLigadoMotor2\":" + String(motoresLigados[1]) + 
                            ",\"compressorLigadoMotor3\":" + String(motoresLigados[2]) + 
                              ",\"sistemaEmManutencao\":" + String(sistemaEmManutencao) + "}";
        request->send(200, "application/json", stateJson); }); // Envia o JSON para o cliente com um status de resposta 200 (OK)
}
