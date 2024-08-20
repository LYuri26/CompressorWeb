#include <ESPAsyncWebServer.h> // Inclui a biblioteca para o servidor web assíncrono
#include <FS.h>                // Inclui a biblioteca para sistema de arquivos
#include <SPIFFS.h>            // Inclui a biblioteca para o SPIFFS (SPI Flash File System)
#include "dashboard.h"         // Inclui o cabeçalho para as funções do dashboard
#include "ligadesliga.h"       // Inclui o cabeçalho para as funções de ligar/desligar motores
#include "autenticador.h"      // Inclui o cabeçalho para funções de autenticação
#include "manutencao.h"        // Inclui o cabeçalho para funções de manutenção

// -------------------------------------------------------------------------
// Função para configurar a página do dashboard e os endpoints relacionados
// -------------------------------------------------------------------------
void setupDashboardPage(AsyncWebServer &server)
{
    // Configura a rota para a página do dashboard
    server.on("/dashboard", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        // Verifica se o usuário está autenticado
        if (!isAuthenticated(request))
        {
            // Redireciona para a página de acesso negado se a autenticação falhar
            redirectToAccessDenied(request);
            return;
        }

        // HTML para a página do dashboard
        String html = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-br">
<head>
    <meta charset="UTF-8">  <!-- Define a codificação de caracteres como UTF-8 -->
    <meta name="viewport" content="width=device-width, initial-scale=1.0">  <!-- Configura a viewport para dispositivos móveis -->
    <title>Dashboard</title>  <!-- Define o título da página -->
    <style>
        /* Estilos gerais para o corpo da página */
        body {
            font-family: Arial, sans-serif; /* Define a fonte da página */
            background-color: #f8f9fa; /* Define a cor de fundo da página */
            margin: 0; /* Remove a margem da página */
            padding: 0; /* Remove o padding da página */
            display: flex; /* Usa flexbox para layout */
            justify-content: center; /* Centraliza horizontalmente */
            align-items: center; /* Centraliza verticalmente */
            height: 100vh; /* Define a altura da página como 100% da altura da viewport */
            flex-direction: column; /* Alinha os itens na vertical */
        }
        /* Estilos para o container do dashboard */
        .dashboard-container {
            background-color: #ffffff; /* Define a cor de fundo do container do dashboard */
            padding: 20px; /* Define o padding do container */
            border-radius: 8px; /* Adiciona bordas arredondadas */
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1); /* Adiciona sombra ao container */
            text-align: center; /* Alinha o texto ao centro */
            width: 100%; /* Define a largura como 100% */
            max-width: 400px; /* Define a largura máxima do container */
        }
        /* Estilos para o título do dashboard */
        .dashboard-title {
            font-size: 28px; /* Define o tamanho da fonte do título */
            margin-bottom: 20px; /* Define a margem inferior do título */
            color: #004085; /* Define a cor do texto do título */
        }
        /* Estilos para os botões */
        .btn {
            display: block; /* Define o botão como bloco para preencher a largura disponível */
            width: calc(100% - 24px); /* Define a largura do botão, subtraindo 24px para margem */
            padding: 12px; /* Define o padding do botão */
            font-size: 16px; /* Define o tamanho da fonte do botão */
            margin: 10px auto; /* Define a margem do botão */
            border: none; /* Remove a borda do botão */
            border-radius: 8px; /* Adiciona bordas arredondadas ao botão */
            cursor: pointer; /* Muda o cursor para pointer ao passar sobre o botão */
            color: white; /* Define a cor do texto do botão */
            transition: background-color 0.3s, transform 0.3s; /* Adiciona uma transição suave para cor de fundo e transformação */
        }
        /* Estilos para botões de motores específicos */
        .btn-motor1 { background-color: #04997d; } /* Cor de fundo do botão do motor 1 */
        .btn-motor2 { background-color: #2ae6d4; } /* Cor de fundo do botão do motor 2 */
        .btn-motor3 { background-color: #18df94; } /* Cor de fundo do botão do motor 3 */
        /* Estilos para o botão de desligar */
        .btn-desligar { background-color: #c82333; } /* Cor de fundo do botão de desligar */
        /* Estilos para o botão desativado */
        .btn-disabled {
            background-color: #6c757d; /* Cor de fundo do botão desativado */
            cursor: not-allowed; /* Muda o cursor para não permitido ao passar sobre o botão desativado */
        }
        /* Estilos para links */
        .btn-link {
            display: block; /* Define o link como bloco para preencher a largura disponível */
            width: calc(100% - 24px); /* Define a largura do link, subtraindo 24px para margem */
            padding: 12px; /* Define o padding do link */
            font-size: 16px; /* Define o tamanho da fonte do link */
            margin: 10px auto; /* Define a margem do link */
            border-radius: 8px; /* Adiciona bordas arredondadas ao link */
            text-decoration: none; /* Remove a decoração do texto do link */
            color: white; /* Define a cor do texto do link */
            transition: background-color 0.3s, transform 0.3s; /* Adiciona uma transição suave para cor de fundo e transformação */
        }
        /* Estilos para links específicos */
        .btn-link-umidade { background-color: #004085; } /* Cor de fundo do link de umidade */
        .btn-link-pressao { background-color: #343a40; } /* Cor de fundo do link de pressão */
        .btn-link-logout { background-color: #c82333; } /* Cor de fundo do link de logout */
        /* Efeito de hover para links */
        .btn-link:hover {
            opacity: 0.8; /* Define a opacidade do link ao passar o mouse */
            transform: scale(1.02); /* Aplica uma leve transformação de escala ao link ao passar o mouse */
        }
        /* Estilos para o rodapé da página */
        .footer {
            width: 100%; /* Define a largura do rodapé como 100% */
            background-color: #004085; /* Define a cor de fundo do rodapé */
            color: white; /* Define a cor do texto do rodapé */
            text-align: center; /* Alinha o texto ao centro */
            padding: 10px 0; /* Define o padding do rodapé */
            position: fixed; /* Fixa o rodapé na parte inferior da página */
            bottom: 0; /* Define a posição do rodapé no fundo da página */
            font-size: 14px; /* Define o tamanho da fonte do rodapé */
        }
        /* Estilos para a caixa de mensagem */
        #messageBox {
            margin-top: 15px; /* Define a margem superior da caixa de mensagem */
            font-size: 16px; /* Define o tamanho da fonte da caixa de mensagem */
            color: #c82333; /* Define a cor do texto da caixa de mensagem */
            margin-bottom: 90px; /* Define a margem inferior da caixa de mensagem */
        }
    </style>
</head>
<body>
    <div class="dashboard-container">  <!-- Container principal do dashboard -->
        <h2 class="dashboard-title">Bem-vindo ao Dashboard</h2>  <!-- Título do dashboard -->
        <button class="btn btn-motor1" id="toggleButtonMotor1">Carregando...</button>  <!-- Botão para o motor 1 -->
        <button class="btn btn-motor2" id="toggleButtonMotor2">Carregando...</button>  <!-- Botão para o motor 2 -->
        <button class="btn btn-motor3" id="toggleButtonMotor3">Carregando...</button>  <!-- Botão para o motor 3 -->
        <a href="/umidade" class="btn btn-link btn-link-umidade">Umidade</a>  <!-- Link para a página de umidade -->
        <a href="/pressao" class="btn btn-link btn-link-pressao">Pressão</a>  <!-- Link para a página de pressão -->
        <a href="/logout" class="btn btn-link btn-link-logout">Logout</a>  <!-- Link para logout -->
    </div>
    <div id="messageBox"></div>  <!-- Caixa de mensagem para mostrar alertas e informações -->
    <div class="footer">  <!-- Rodapé da página -->
        <p>Aplicação desenvolvida pela Turma de Informática Para Internet Trilhas de Futuro 2024</p>  <!-- Texto do rodapé -->
        <p>Instrutor: Lenon Yuri</p>  <!-- Texto do rodapé -->
    </div>
<script>
    // Este código é executado quando o conteúdo da página é totalmente carregado.
    document.addEventListener('DOMContentLoaded', function() {
        // Obtém referências aos botões de controle dos motores e à caixa de mensagem.
        var toggleButtonMotor1 = document.getElementById('toggleButtonMotor1');
        var toggleButtonMotor2 = document.getElementById('toggleButtonMotor2');
        var toggleButtonMotor3 = document.getElementById('toggleButtonMotor3');
        var messageBox = document.getElementById('messageBox');
        // Inicializa a variável para armazenar o estado anterior de manutenção.
        var previousMaintenanceState = null;

        // Função para atualizar o estado do botão com base nas informações do motor.
        function updateButtonState(button, motor, buttonClass) {
            // Faz uma requisição para obter o estado atual dos motores.
            fetch('/motor-state')
                .then(response => response.json())  // Converte a resposta da requisição para JSON.
                .then(data => {
                    // Obtém o estado do compressor e o estado de manutenção do sistema.
                    var compressorLigado = data['compressorLigadoMotor' + motor];
                    var sistemaEmManutencao = data.sistemaEmManutencao;
                    // Obtém a hora atual em formato decimal.
                    var horaAtual = new Date().getHours() + (new Date().getMinutes() / 60);

                    let message = ''; // Inicializa a mensagem a ser exibida na caixa de mensagem.
                    if (sistemaEmManutencao) {
                        // Se o sistema está em manutenção, atualiza o botão e a caixa de mensagem.
                        button.innerHTML = 'Compressor ' + motor + ' em manutenção';
                        button.classList.add('btn-disabled');  // Adiciona a classe que desativa o botão.
                        button.classList.remove(buttonClass, 'btn-desligar');  // Remove as classes de motor e de desligar.
                        messageBox.innerHTML = '';  // Limpa a caixa de mensagem.
                    } else {
                        // Se o sistema não está em manutenção, atualiza o botão com base no estado do compressor.
                        if (compressorLigado) {
                            button.innerHTML = 'Desligar ' + motor;
                            button.classList.add('btn-desligar');  // Adiciona a classe que indica que o compressor está ligado.
                            button.classList.remove(buttonClass);  // Remove a classe do motor.
                            message = 'O compressor ' + motor + ' acabou de ser ligado, aguarde o tempo limite para desligar novamente.';
                        } else {
                            button.innerHTML = 'Ligar ' + motor;
                            button.classList.remove('btn-desligar');  // Remove a classe de desligar.
                            button.classList.add(buttonClass);  // Adiciona a classe do motor.
                            message = '';  // Limpa a mensagem.
                        }

                        // Verifica o horário atual e define uma mensagem apropriada.
                        if (horaAtual < 7.5 || horaAtual >= 22.5) {
                            message = 'Compressor ' + motor + ' desligado devido ao horário de funcionamento.';
                        } else if (horaAtual >= 7.5 && horaAtual < 8) {
                            message = 'Compressor ' + motor + ' ligado após o horário de funcionamento, desligue após o uso.';
                        }
                    }

                    // Atualiza a caixa de mensagem apenas se a mensagem mudou.
                    if (messageBox.innerHTML !== message) {
                        messageBox.innerHTML = message;
                    }

                    // Verifica se o estado de manutenção mudou e evita recarregar a página desnecessariamente.
                    if (previousMaintenanceState !== null && previousMaintenanceState !== sistemaEmManutencao && !sistemaEmManutencao) {
                        // Evita recarregar a página todas as vezes.
                        // location.reload();
                    }

                    // Atualiza o estado anterior de manutenção.
                    previousMaintenanceState = sistemaEmManutencao;
                })
                .catch(error => console.error('Erro ao obter estado inicial do compressor:', error));  // Captura e exibe erros.
        }

        // Função para configurar o evento de clique do botão.
        function setupButtonClick(button, motor, buttonClass) {
            button.addEventListener('click', function(event) {
                event.preventDefault();  // Previne o comportamento padrão do botão, que é submeter um formulário ou seguir um link.
                if (button.classList.contains('btn-disabled')) {
                    return;  // Se o botão estiver desativado, não faz nada.
                }

                // Determina a ação a ser tomada com base no texto do botão.
                var action = button.innerHTML.includes('Desligar') ? 'desligar' : 'ligar';

                // Faz uma requisição para ligar ou desligar o motor.
                fetch('/toggle?action=' + action + '&motor=' + motor)
                    .then(response => response.text())  // Converte a resposta da requisição para texto.
                    .then(data => {
                        // Atualiza o estado do botão após a ação ser realizada.
                        updateButtonState(button, motor, buttonClass);
                    })
                    .catch(error => console.error('Erro ao enviar requisição:', error));  // Captura e exibe erros.
            });
        }

        // Configura o evento de clique para cada botão de motor.
        setupButtonClick(toggleButtonMotor1, '1', 'btn-motor1');
        setupButtonClick(toggleButtonMotor2, '2', 'btn-motor2');
        setupButtonClick(toggleButtonMotor3, '3', 'btn-motor3');

        // Atualiza o estado dos botões a cada 10 segundos.
        setInterval(() => {
            updateButtonState(toggleButtonMotor1, '1', 'btn-motor1');
            updateButtonState(toggleButtonMotor2, '2', 'btn-motor2');
            updateButtonState(toggleButtonMotor3, '3', 'btn-motor3');
        }, 10000);

        // Atualiza o estado dos botões ao carregar a página.
        updateButtonState(toggleButtonMotor1, '1', 'btn-motor1');
        updateButtonState(toggleButtonMotor2, '2', 'btn-motor2');
        updateButtonState(toggleButtonMotor3, '3', 'btn-motor3');
    });
</script>
</body>
</html>
        )rawliteral";

        // Envia o HTML da página do dashboard em resposta a uma requisição GET
        request->send(200, "text/html", html); });

    // Configura a rota para obter o estado dos motores
    server.on("/motor-state", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        // Cria um JSON com o estado dos motores e manutenção
        String stateJson = "{\"compressorLigadoMotor1\":" + String(motoresLigados[0]) +
                            ",\"compressorLigadoMotor2\":" + String(motoresLigados[1]) +
                            ",\"compressorLigadoMotor3\":" + String(motoresLigados[2]) +
                            ",\"sistemaEmManutencao\":" + String(sistemaEmManutencao) + "}";
        // Envia o JSON em resposta a uma requisição GET
        request->send(200, "application/json", stateJson); });
}
