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
        <meta charset="UTF-8">
        <!-- Define a codificação de caracteres como UTF-8 -->
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <!-- Configura a viewport para dispositivos móveis -->
        <title>Dashboard</title> <!-- Define o título da página -->
        <style>
/* Estilos gerais para o corpo da página */
body {
    font-family: Arial, sans-serif; /* Define a fonte da página */
    background-color: #f8f9fa; /* Define a cor de fundo da página */
    margin: 0; /* Remove a margem da página */
    padding: 60px; /* Remove o padding da página */
    display: flex; /* Usa flexbox para layout */
    justify-content: center; /* Centraliza horizontalmente */
    align-items: center; /* Centraliza verticalmente */
    height: 100vh; /* Define a altura da página como 100% da altura da viewport */
    flex-direction: column; /* Alinha os itens na vertical */
    transition: background-color 0.3s, color 0.3s; /* Adiciona transições suaves */
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
    transition: background-color 0.3s, color 0.3s; /* Adiciona transições suaves */
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
.btn-motor1 { background-color: #1e90ff; } /* Cor de fundo do botão do motor 1 */
.btn-motor2 { background-color: #32cd32; } /* Cor de fundo do botão do motor 2 */
.btn-motor3 { background-color: #ff4500; } /* Cor de fundo do botão do motor 3 */

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

/* Estilos para os botões de alternância de temas */
#nightModeButton, #highContrastButton {
    width:100%;
    padding: 12px; /* Define o padding dos botões */
    font-size: 16px; /* Define o tamanho da fonte dos botões */
    margin: 10px auto; /* Define a margem dos botões */
    border: none; /* Remove a borda dos botões */
    border-radius: 8px; /* Adiciona bordas arredondadas aos botões */
    cursor: pointer; /* Muda o cursor para pointer ao passar sobre os botões */
    color: white; /* Define a cor do texto dos botões */
    transition: background-color 0.3s, transform 0.3s; /* Adiciona uma transição suave para cor de fundo e transformação */
}

/* Estilos específicos para o botão de modo noturno */
#nightModeButton {
    background-color: #6f42c1; /* Cor de fundo do botão de modo noturno */
}

/* Estilos específicos para o botão de alto contraste */
#highContrastButton {
    background-color: #e83e8c; /* Cor de fundo do botão de alto contraste */
}

/* Efeito de hover para os botões de alternância de temas */
#nightModeButton:hover, #highContrastButton:hover {
    opacity: 0.8; /* Define a opacidade dos botões ao passar o mouse */
    transform: scale(1.02); /* Aplica uma leve transformação de escala aos botões ao passar o mouse */
}

/* Modo noturno */
.night-mode {
    background-color: #121212; /* Cor de fundo do modo noturno */
    color: #e0e0e0; /* Cor do texto do modo noturno */
}
.night-mode .dashboard-container {
    background-color: #1e1e1e; /* Cor de fundo do container no modo noturno */
    color: #e0e0e0; /* Cor do texto no container no modo noturno */
}

/* Modo alto contraste */
.high-contrast {
    background-color: #000000; /* Cor de fundo do modo alto contraste */
    color: #ffffff; /* Cor do texto no modo alto contraste */
}
.high-contrast .dashboard-container {
    background-color: #000000; /* Cor de fundo do container no modo alto contraste */
    color: #ffffff; /* Cor do texto no container no modo alto contraste */
}

/* Estilos ARIA para acessibilidade */
.aria-live {
    position: absolute;
    left: -9999px;
    top: auto;
    width: 1px;
    height: 1px;
    overflow: hidden;
}

        </style>
    </head>
    <body>
        <div class="dashboard-container">
            <h2 class="dashboard-title">Bem-vindo ao Dashboard</h2>
            <button class="btn btn-motor1" id="toggleButtonMotor1"
                aria-label="Controle do motor 1">Carregando...</button>
            <button class="btn btn-motor2" id="toggleButtonMotor2"
                aria-label="Controle do motor 2">Carregando...</button>
            <button class="btn btn-motor3" id="toggleButtonMotor3"
                aria-label="Controle do motor 3">Carregando...</button>
            <a href="/umidade" class="btn btn-link btn-link-umidade"
                aria-label="Página de umidade">Umidade</a>
            <a href="/pressao" class="btn btn-link btn-link-pressao"
                aria-label="Página de pressão">Pressão</a>
            <a href="/logout" class="btn btn-link btn-link-logout"
                aria-label="Logout">Logout</a>
            <button id="nightModeButton" aria-label="Modo Noturno">Modo
                Noturno</button>
            <button id="highContrastButton" aria-label="Alto Contraste">Alto
                Contraste</button>
        </div>
        <div id="messageBox" role="alert"></div>
        <div class="footer">
            <p>Aplicação desenvolvida pela Turma de Informática Para Internet
                Trilhas de Futuro 2024</p>
            <p>Instrutor: Lenon Yuri</p>
        </div>
        <script>
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
                fetch('/motor-state')
                    .then(response => response.json())
                    .then(data => {
                        var compressorLigado = data['compressorLigadoMotor' + motor];
                        var sistemaEmManutencao = data.sistemaEmManutencao;
                        var horaAtual = new Date().getHours() + (new Date().getMinutes() / 60);
                        let message = '';
        
                        if (sistemaEmManutencao) {
                            if (previousMaintenanceState === null || !previousMaintenanceState) {
                                saveButtonState(button, motor, buttonClass);
                            }
                            button.innerHTML = 'Compressor ' + motor + ' em manutenção';
                            button.classList.add('btn-disabled');
                            button.classList.remove(buttonClass, 'btn-desligar');
                            messageBox.innerHTML = '';
                        } else {
                            if (compressorLigado) {
                                button.innerHTML = 'Desligar ' + motor;
                                button.classList.add('btn-desligar');
                                button.classList.remove(buttonClass);
                                let activationTime = getActivationTime(motor);
        
                                if (activationTime && (new Date() - activationTime < 3600000)) {
                                    button.classList.add('btn-disabled');
                                    message = 'Desligue o motor quando o prazo de 1 hora estiver finalizado.';
                                } else {
                                    button.classList.remove('btn-disabled');
                                    message = 'O compressor ' + motor + ' está ligado. Você pode desligá-lo quando o prazo de 1 hora estiver finalizado.';
                                }
                            } else {
                                button.innerHTML = 'Ligar ' + motor;
                                button.classList.remove('btn-desligar', 'btn-disabled');
                                button.classList.add(buttonClass);
                                message = '';
                            }
        
                            if (horaAtual < 7.5 || horaAtual >= 22.5) {
                                message = 'Compressor ' + motor + ' desligado devido ao horário de funcionamento.';
                            } else if (horaAtual >= 7.5 && horaAtual < 8) {
                                message = 'Compressor ' + motor + ' ligado após o horário de funcionamento, desligue após o uso.';
                            }
                        }
        
                        if (messageBox.innerHTML !== message) {
                            messageBox.innerHTML = message;
                        }
        
                        if (previousMaintenanceState !== null && previousMaintenanceState !== sistemaEmManutencao) {
                            if (!sistemaEmManutencao) {
                                location.reload();
                            }
                        }
        
                        previousMaintenanceState = sistemaEmManutencao;
                    })
                    .catch(error => console.error('Erro ao obter estado inicial do compressor:', error));
            }
        
            function setupButtonClick(button, motor, buttonClass) {
                button.addEventListener('click', function(event) {
                    event.preventDefault();
        
                    if (button.classList.contains('btn-disabled')) {
                        return;
                    }
        
                    var action = button.innerHTML.includes('Desligar') ? 'desligar' : 'ligar';
        
                    fetch('/toggle?action=' + action + '&motor=' + motor)
                        .then(response => response.text())
                        .then(data => {
                            if (action === 'ligar') {
                                setActivationTime(motor);
                                setTimeout(() => {
                                    updateButtonState(button, motor, buttonClass);
                                }, 3600000);
                            }
                            updateButtonState(button, motor, buttonClass);
                        })
                        .catch(error => console.error('Erro ao enviar comando para o compressor:', error));
                });
            }
        
            function setActivationTime(motor) {
                let now = new Date();
                if (motor === '1') activationTimeMotor1 = now;
                else if (motor === '2') activationTimeMotor2 = now;
                else if (motor === '3') activationTimeMotor3 = now;
                localStorage.setItem('activationTimeMotor' + motor, now.toISOString());
            }
        
            function getActivationTime(motor) {
                var activationTimes = {
                    1: localStorage.getItem('activationTimeMotor1'),
                    2: localStorage.getItem('activationTimeMotor2'),
                    3: localStorage.getItem('activationTimeMotor3')
                };
                return activationTimes[motor] ? new Date(activationTimes[motor]) : null;
            }
        
            function saveButtonState(button, motor, buttonClass) {
                let state = { enabled: !button.classList.contains('btn-disabled'), text: button.innerHTML };
                if (motor === '1') previousButtonStateMotor1 = state;
                else if (motor === '2') previousButtonStateMotor2 = state;
                else if (motor === '3') previousButtonStateMotor3 = state;
            }
        
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
        
            // Configura o comportamento dos botões de tema.
            function setupThemeButtons() {
                const nightModeButton = document.getElementById('nightModeButton');
                const highContrastButton = document.getElementById('highContrastButton');
        
                if (nightModeButton) {
                    nightModeButton.addEventListener('click', function() {
                        toggleTheme('night');
                    });
                }
        
                if (highContrastButton) {
                    highContrastButton.addEventListener('click', function() {
                        toggleTheme('high-contrast');
                    });
                }
            }
        
            function toggleTheme(theme) {
                if (theme === 'night') {
                    document.body.classList.toggle('night-mode');
                    document.body.classList.remove('high-contrast');
                } else if (theme === 'high-contrast') {
                    document.body.classList.toggle('high-contrast');
                    document.body.classList.remove('night-mode');
                }
            }
        
            // Configura o comportamento dos botões ao serem clicados.
            setupButtonClick(toggleButtonMotor1, '1', 'btn-motor1');
            setupButtonClick(toggleButtonMotor2, '2', 'btn-motor2');
            setupButtonClick(toggleButtonMotor3, '3', 'btn-motor3');
        
            // Configura os botões de tema.
            setupThemeButtons();
        
            // Configura um intervalo para atualizar o estado dos botões a cada 10 segundos.
            setInterval(() => {
                updateButtonState(toggleButtonMotor1, '1', 'btn-motor1');
                updateButtonState(toggleButtonMotor2, '2', 'btn-motor2');
                updateButtonState(toggleButtonMotor3, '3', 'btn-motor3');
            }, 10000);
        
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
