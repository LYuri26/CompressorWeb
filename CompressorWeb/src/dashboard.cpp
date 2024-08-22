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
// Adiciona um evento que será executado quando o conteúdo da página for totalmente carregado.
document.addEventListener('DOMContentLoaded', function() {

    // Obtém referências aos botões de controle dos motores e à caixa de mensagem.
    var toggleButtonMotor1 = document.getElementById('toggleButtonMotor1');
    var toggleButtonMotor2 = document.getElementById('toggleButtonMotor2');
    var toggleButtonMotor3 = document.getElementById('toggleButtonMotor3');
    var messageBox = document.getElementById('messageBox');

    // Variável para armazenar o estado de manutenção anterior do sistema.
    var previousMaintenanceState = null;

    // Variáveis para armazenar o horário de ativação de cada motor.
    var activationTimeMotor1 = null;
    var activationTimeMotor2 = null;
    var activationTimeMotor3 = null;

    // Variáveis para armazenar o estado anterior dos botões antes da manutenção.
    var previousButtonStateMotor1 = { enabled: true, text: '' };
    var previousButtonStateMotor2 = { enabled: true, text: '' };
    var previousButtonStateMotor3 = { enabled: true, text: '' };

    // Função para atualizar o estado do botão (ativado/desativado) e a mensagem correspondente.
    function updateButtonState(button, motor, buttonClass) {
        // Faz uma requisição ao servidor para obter o estado atual dos motores e do sistema.
        fetch('/motor-state')
            .then(response => response.json())  // Converte a resposta do servidor para um objeto JSON.
            .then(data => {
                // Verifica se o motor específico está ligado e se o sistema está em manutenção.
                var compressorLigado = data['compressorLigadoMotor' + motor];
                var sistemaEmManutencao = data.sistemaEmManutencao;

                // Calcula a hora atual em formato decimal (ex.: 14.5 para 14:30).
                var horaAtual = new Date().getHours() + (new Date().getMinutes() / 60);

                // Inicializa uma variável para a mensagem a ser exibida ao usuário.
                let message = '';

                // Verifica se o sistema está em manutenção.
                if (sistemaEmManutencao) {
                    // Salva o estado atual do botão antes de desabilitá-lo para manutenção.
                    if (previousMaintenanceState === null || !previousMaintenanceState) {
                        saveButtonState(button, motor, buttonClass);
                    }
                    // Se o sistema está em manutenção, desabilita o botão e atualiza o texto.
                    button.innerHTML = 'Compressor ' + motor + ' em manutenção';
                    button.classList.add('btn-disabled');
                    button.classList.remove(buttonClass, 'btn-desligar');
                    messageBox.innerHTML = '';  // Limpa a caixa de mensagem.
                } else {
                    // Se o sistema não está em manutenção, verifica se o motor está ligado.
                    if (compressorLigado) {
                        // Atualiza o botão para permitir desligar o motor e adiciona a classe 'btn-desligar'.
                        button.innerHTML = 'Desligar ' + motor;
                        button.classList.add('btn-desligar');
                        button.classList.remove(buttonClass);

                        // Obtém o tempo de ativação do motor.
                        let activationTime = getActivationTime(motor);
                        
                        // Verifica se o motor foi ativado há menos de 1 hora.
                        if (activationTime && (new Date() - activationTime < 3600000)) {
                            // Se sim, desabilita o botão e exibe uma mensagem de espera.
                            button.classList.add('btn-disabled');
                            message = 'Desligue o motor quando o prazo de 1 hora estiver finalizado.';
                        } else {
                            // Caso contrário, habilita o botão e permite desligar o motor.
                            button.classList.remove('btn-disabled');
                            message = 'O compressor ' + motor + ' está ligado. Você pode desligá-lo quando o prazo de 1 hora estiver finalizado.';
                        }
                    } else {
                        // Se o motor não está ligado, atualiza o botão para permitir ligar o motor.
                        button.innerHTML = 'Ligar ' + motor;
                        button.classList.remove('btn-desligar', 'btn-disabled');
                        button.classList.add(buttonClass);
                        message = '';  // Limpa a mensagem.
                    }

                    // Verifica se o horário atual está fora do horário de funcionamento.
                    if (horaAtual < 7.5 || horaAtual >= 22.5) {
                        // Se sim, exibe uma mensagem indicando que o compressor está desligado.
                        message = 'Compressor ' + motor + ' desligado devido ao horário de funcionamento.';
                    } else if (horaAtual >= 7.5 && horaAtual < 8) {
                        // Exibe uma mensagem específica se o horário estiver no início do funcionamento.
                        message = 'Compressor ' + motor + ' ligado após o horário de funcionamento, desligue após o uso.';
                    }
                }

                // Atualiza a caixa de mensagem apenas se a mensagem mudou.
                if (messageBox.innerHTML !== message) {
                    messageBox.innerHTML = message;
                }

                // Verifica se o estado de manutenção mudou desde a última atualização.
                if (previousMaintenanceState !== null && previousMaintenanceState !== sistemaEmManutencao) {
                    if (!sistemaEmManutencao) {
                        // Se o sistema saiu da manutenção, atualiza a página inteira.
                        location.reload(); 
                    }
                }

                // Atualiza o estado anterior de manutenção com o estado atual.
                previousMaintenanceState = sistemaEmManutencao;
            })
            // Captura e exibe qualquer erro que ocorra ao tentar obter o estado do compressor.
            .catch(error => console.error('Erro ao obter estado inicial do compressor:', error));
    }

    // Função para configurar o comportamento do botão ao ser clicado.
    function setupButtonClick(button, motor, buttonClass) {
        // Adiciona um evento de clique ao botão.
        button.addEventListener('click', function(event) {
            // Previne o comportamento padrão do botão (ex.: enviar um formulário).
            event.preventDefault();

            // Se o botão estiver desativado, não faz nada ao clicar.
            if (button.classList.contains('btn-disabled')) {
                return;
            }

            // Determina se a ação será ligar ou desligar o motor com base no texto do botão.
            var action = button.innerHTML.includes('Desligar') ? 'desligar' : 'ligar';

            // Envia uma requisição ao servidor para realizar a ação (ligar/desligar) no motor específico.
            fetch('/toggle?action=' + action + '&motor=' + motor)
                .then(response => response.text())  // Converte a resposta do servidor para texto.
                .then(data => {
                    // Se a ação for ligar, registra o horário de ativação do motor.
                    if (action === 'ligar') {
                        setActivationTime(motor);  // Chama a função para armazenar o horário atual.
                        // Agenda uma atualização do estado do botão para daqui a 1 hora.
                        setTimeout(() => {
                            updateButtonState(button, motor, buttonClass);
                        }, 3600000);  // 3600000 milissegundos = 1 hora.
                    }
                    // Atualiza o estado do botão imediatamente após a ação ser realizada.
                    updateButtonState(button, motor, buttonClass);
                })
                // Captura e exibe qualquer erro que ocorra ao tentar realizar a ação no motor.
                .catch(error => console.error('Erro ao enviar requisição:', error));
        });
    }

    // Função para armazenar o horário de ativação do motor.
    function setActivationTime(motor) {
        let now = new Date();  // Obtém o horário atual.
        // Armazena o horário atual na variável correspondente ao motor.
        if (motor === '1') activationTimeMotor1 = now;
        else if (motor === '2') activationTimeMotor2 = now;
        else if (motor === '3') activationTimeMotor3 = now;
    }

    // Função para obter o horário de ativação do motor específico.
    function getActivationTime(motor) {
        // Retorna o horário de ativação correspondente ao motor solicitado.
        if (motor === '1') return activationTimeMotor1;
        else if (motor === '2') return activationTimeMotor2;
        else if (motor === '3') return activationTimeMotor3;
        return null;  // Retorna null se o motor não for válido.
    }

    // Função para salvar o estado do botão antes da manutenção.
    function saveButtonState(button, motor, buttonClass) {
        let state = { enabled: !button.classList.contains('btn-disabled'), text: button.innerHTML };
        if (motor === '1') previousButtonStateMotor1 = state;
        else if (motor === '2') previousButtonStateMotor2 = state;
        else if (motor === '3') previousButtonStateMotor3 = state;
    }

    // Função para restaurar o estado do botão após a manutenção.
    function restoreButtonState(button, motor, buttonClass) {
        let state;
        if (motor === '1') state = previousButtonStateMotor1;
        else if (motor === '2') state = previousButtonStateMotor2;
        else if (motor === '3') state = previousButtonStateMotor3;

        if (state.enabled) {
            button.classList.remove('btn-disabled');
            button.classList.add(buttonClass);
        } else {
            button.classList.add('btn-disabled');
        }
        button.innerHTML = state.text;
    }

    // Configura o comportamento dos botões ao serem clicados, associando cada botão ao respectivo motor.
    setupButtonClick(toggleButtonMotor1, '1', 'btn-motor1');
    setupButtonClick(toggleButtonMotor2, '2', 'btn-motor2');
    setupButtonClick(toggleButtonMotor3, '3', 'btn-motor3');

    // Configura um intervalo para atualizar o estado dos botões a cada 10 segundos.
    setInterval(() => {
        updateButtonState(toggleButtonMotor1, '1', 'btn-motor1');
        updateButtonState(toggleButtonMotor2, '2', 'btn-motor2');
        updateButtonState(toggleButtonMotor3, '3', 'btn-motor3');
    }, 10000);  // 10000 milissegundos = 10 segundos.

    // Atualiza o estado dos botões imediatamente ao carregar a página.
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
