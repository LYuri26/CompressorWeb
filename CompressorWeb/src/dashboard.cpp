#include <ESPAsyncWebServer.h> // Inclui a biblioteca para o servidor web assíncrono
#include <FS.h>                // Inclui a biblioteca para sistema de arquivos
#include <SPIFFS.h>            // Inclui a biblioteca para o SPIFFS (SPI Flash File System)
#include "dashboard.h"         // Inclui o cabeçalho para as funções do dashboard
#include "ligadesliga.h"       // Inclui o cabeçalho para as funções de ligar/desligar motores
#include "autenticador.h"      // Inclui o cabeçalho para funções de autenticação
#include "manutencao.h"        // Inclui o cabeçalho para funções de manutenção
#include "sobrecarga.h"

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
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <link rel="preconnect" href="https://fonts.googleapis.com">
        <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
        <link
            href="https://fonts.googleapis.com/css2?family=Playfair+Display:ital,wght@0,400..900;1,400..900&display=swap"
            rel="stylesheet">
        <title>Dashboard</title>
        <style>
            /* Estilos gerais para o corpo da página */
            body {
                font-family: Arial, sans-serif;
                background-color: #f0f2f5;
                display: flex;
                flex-direction: column;
                align-items: center;
                justify-content: center;
                min-height: 100vh;
                margin: 0;
                padding: 0;
                transition: background-color 0.3s, color 0.3s;
            }
        
            /* Estilos para o container do dashboard */
            .dashboard-container {
                background-color: #ffffff;
                padding: 20px;
                border-radius: 8px;
                box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
                text-align: center;
                width: 100%;
                max-width: 400px;
                margin: auto;
                transition: background-color 0.3s, color 0.3s;
            }
        
            /* Estilos para o título do dashboard */
            .dashboard-title {
                font-size: 24px;
                font-family: "Playfair Display", serif;
                margin-bottom: 20px;
                background-image: linear-gradient(to bottom, rgb(255, 221, 0), rgba(246, 15, 15, 0.975), rgb(249, 216, 28));
                background-clip: text;
                -webkit-background-clip: text;
                -webkit-text-fill-color: transparent;
                color: rgb(242, 95, 22);
                font-weight: bold;
            }
        
            /* Estilos para o grupo de botão e etiqueta */
            .button-group {
                margin-bottom: 20px; /* Espaçamento entre os grupos */
            }
        
            .button-group p {
                margin: 5px 0; /* Espaçamento entre o botão e a etiqueta */
                font-size: 16px;
                font-weight: bold;
                color: #333;
            }
        
            /* Estilos para botões redondos */
            .round-button {
                width: 100px;
                height: 100px;
                border-radius: 50%;
                display: flex;
                align-items: center;
                justify-content: center;
                text-align: center;
                font-size: 14px;
                font-weight: bold;
                margin: 0 auto;
                cursor: pointer;
                transition: background-color 0.3s, transform 0.3s, box-shadow 0.3s;
                color: white;
                border: 2px solid rgba(0, 0, 0, 0.2); /* Borda adicionada */
                outline: none;
                position: relative;
                overflow: hidden;
                white-space: nowrap;
                padding: 10px;
            }
        
            /* Efeito de pressionar ao clicar */
            .round-button:active {
                transform: scale(0.95);
                box-shadow: 0 2px 5px rgba(0, 0, 0, 0.2) inset;
            }
        
            /* Ajuste de fonte para textos longos */
            .round-button.long-text {
                font-size: 12px;
                padding: 8px;
            }
        
            /* Estilos para botões de motores específicos */
            .btn-motor1 { background-color: #32cd32; }
            .btn-motor2 { background-color: #32cd32; }
            .btn-motor3 { background-color: #32cd32; }
        
            /* Estilos para botões desativados */
            .btn-disabled {
                background-color: #6c757d;
                cursor: not-allowed;
            }
        
            /* Estilos para botões em manutenção */
            .btn-maintenance { background-color: #ff8c00; }
        
            /* Estilos para botões com sobrecarga */
            .btn-overload { background-color: #ff4500; }
        
            /* Estilos para botões fora do horário */
            .btn-out-of-hours { background-color: #808080; }
        
            /* Estilos para a caixa de mensagem */
            #messageBox {
                margin-top: 20px;
                padding: 10px;
                border-radius: 5px;
                background-color: #f8f9fa;
                color: #333;
                font-size: 16px; /* Tamanho maior para mensagens */
                text-align: center;
                transition: background-color 0.3s, color 0.3s;
            }
        
            /* Mensagens de erro (vermelho) */
            #messageBox.error {
                background-color: #ffebee; /* Fundo claro para erro */
                color: #c62828; /* Texto vermelho escuro */
                border: 2px solid #c62828; /* Borda vermelha */
            }
        
            /* Mensagens de alerta (laranja) */
            #messageBox.alert {
                background-color: #fff3e0; /* Fundo claro para alerta */
                color: #ef6c00; /* Texto laranja escuro */
                border: 2px solid #ef6c00; /* Borda laranja */
            }
        
            /* Estilos para os botões de alternância de temas */
            #nightModeButton, #highContrastButton {
                width: 100%;
                padding: 12px;
                font-size: 16px;
                margin: 10px auto;
                border: none;
                border-radius: 8px;
                cursor: pointer;
                color: white;
                transition: background-color 0.3s, transform 0.3s;
            }
        
            #nightModeButton { background-color: rgb(234, 161, 15); }
            #highContrastButton { background-color: rgb(247, 214, 49); }
        
            #nightModeButton:hover { background-color: #8e5204; transform: scale(1.05); }
            #highContrastButton:hover { background-color: #8c7807; transform: scale(1.05); }
        
            /* Estilos para os botões de navegação */
            .btn-link {
                display: block;
                padding: 12px;
                font-size: 16px;
                margin: 10px auto;
                border: none;
                border-radius: 8px;
                cursor: pointer;
                color: white;
                text-decoration: none;
                transition: background-color 0.3s, transform 0.3s;
            }
        
            .btn-link-umidade { background-color: #004085; }
            .btn-link-pressao { background-color: #344037; }
            .btn-link-logout { background-color: #c82333; }
        
            .btn-link:hover {
                opacity: 0.8;
                transform: scale(1.02);
            }
        
            /* Modo noturno */
            .night-mode {
                background-color: #121212;
                color: #e0e0e0;
            }
            .night-mode .dashboard-container {
                background-color: #1e1e1e;
                color: #e0e0e0;
            }
            .night-mode #messageBox {
                background-color: #2c2c2c;
                color: #e0e0e0;
            }
            .night-mode .btn-link {
                background-color: #333333;
                color: #e0e0e0;
            }
            .night-mode .round-button {
                border-color: rgba(255, 255, 255, 0.2); /* Borda branca no modo noturno */
                color: white; /* Texto branco no modo noturno */
            }
            .night-mode .button-group p {
                color: white; /* Texto branco para etiquetas dos motores */
            }
        
            /* Modo alto contraste */
            .high-contrast {
                background-color: #000000;
                color: #ffffff;
            }
            .high-contrast .dashboard-container {
                background-color: #000000;
                color: #ffffff;
            }
            .high-contrast #messageBox {
                background-color: #333333;
                color: #ffffff;
            }
            .high-contrast .btn-link {
                background-color: #555555;
                color: #ffffff;
            }
            .high-contrast .round-button {
                border-color: rgba(255, 255, 255, 0.5); /* Borda branca no modo alto contraste */
                color: white; /* Texto branco no modo alto contraste */
            }
            .high-contrast .button-group p {
                color: white; /* Texto branco para etiquetas dos motores */
            }
        
            /* Estilos para o rodapé */
            .footer {
                width: 100%;
                background-color: #ee641a;
                color: white;
                text-align: center;
                padding: 10px 0;
                font-size: 14px;
                margin-top: auto;
            }
        </style>
    </head>
    <body>
        <div class="dashboard-container">
            <h2 class="dashboard-title">Bem-vindo ao Dashboard</h2>

            <!-- Grupo de botão e etiqueta para o Motor Compressor -->
            <div class="button-group">
                <button class="btn btn-motor1 round-button"
                    id="toggleButtonMotor1"
                    aria-label="Controle do motor compressor">
                    Carregando...
                </button>
                <p>Motor Compressor</p>
            </div>

            <!-- Grupo de botão e etiqueta para o Motor Ventilador -->
            <div class="button-group">
                <button class="btn btn-motor2 round-button"
                    id="toggleButtonMotor2"
                    aria-label="Controle do motor ventilador">
                    Carregando...
                </button>
                <p>Motor Ventilador</p>
            </div>

            <!-- Grupo de botão e etiqueta para o Motor Secador -->
            <div class="button-group">
                <button class="btn btn-motor3 round-button"
                    id="toggleButtonMotor3"
                    aria-label="Controle do motor secador">
                    Carregando...
                </button>
                <p>Motor Secador</p>
            </div>

            <div id="messageBox" role="alert"></div>

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

        <div class="footer">
            <p>Aplicação desenvolvida pela Turma de Informática Para Internet
                Trilhas de Futuro 2024</p>
            <p>Instrutor: Lenon Yuri</p>
        </div>

        <script>
            document.addEventListener('DOMContentLoaded', function () {
                // Referências aos elementos
                const toggleButtonMotor1 = document.getElementById('toggleButtonMotor1');
                const toggleButtonMotor2 = document.getElementById('toggleButtonMotor2');
                const toggleButtonMotor3 = document.getElementById('toggleButtonMotor3');
                const messageBox = document.getElementById('messageBox');
                const nightModeButton = document.getElementById('nightModeButton');
                const highContrastButton = document.getElementById('highContrastButton');
            
                // Função para atualizar o estado dos botões e exibir mensagens
                function updateButtonState(button, motor, buttonClass) {
                    fetch('/motor-state')
                        .then(response => response.json())
                        .then(data => {
                            const compressorLigado = data['compressorLigadoMotor' + motor];
                            const sistemaEmManutencao = data.sistemaEmManutencao;
                            const sobrecarga = data['sobrecargaMotor' + motor];
                            const now = new Date();
                            const horaAtual = now.getHours() + (now.getMinutes() / 60);
                
                            let mensagem = '';
                            let mensagemTipo = ''; // Tipo de mensagem: 'alert' ou 'error'
                
                            // Verifica o estado do motor e atualiza o botão
                            if (sobrecarga) {
                                button.innerHTML = 'Sobrecarga';
                                button.classList.add('btn-overload', 'btn-disabled');
                                button.classList.remove(buttonClass, 'btn-desligar');
                                mensagem = `Motor ${motor} está com sobrecarga. Desativado.`;
                                mensagemTipo = 'alert'; // Alerta em laranja
                            } else if (sistemaEmManutencao) {
                                button.innerHTML = 'Manutenção';
                                button.classList.add('btn-maintenance', 'btn-disabled');
                                button.classList.remove(buttonClass, 'btn-desligar');
                                mensagem = `Motor ${motor} está em manutenção. Desativado.`;
                                mensagemTipo = 'alert'; // Alerta em laranja
                            } else if (horaAtual < 8 || horaAtual >= 22) {
                                button.innerHTML = 'Fora do Horário';
                                button.classList.add('btn-out-of-hours', 'btn-disabled');
                                button.classList.remove(buttonClass, 'btn-desligar');
                                mensagem = `Motor ${motor} está fora do horário de funcionamento. Desativado.`;
                                mensagemTipo = ''; // Mensagem padrão
                            } else {
                                if (compressorLigado) {
                                    button.innerHTML = 'Desligar';
                                    button.classList.add('btn-desligar');
                                    button.classList.remove(buttonClass);
                                    mensagem = `Motor ${motor} está ligado.`;
                                    mensagemTipo = ''; // Mensagem padrão
                                } else {
                                    button.innerHTML = 'Ligar';
                                    button.classList.remove('btn-desligar', 'btn-disabled');
                                    button.classList.add(buttonClass);
                                    mensagem = `Motor ${motor} está desligado.`;
                                    mensagemTipo = ''; // Mensagem padrão
                                }
                            }
                
                            // Atualiza a mensagem no messageBox
                            if (mensagem) {
                                messageBox.innerHTML = mensagem; // Substitui o conteúdo existente
                                messageBox.className = ''; // Limpa classes anteriores
                                if (mensagemTipo) {
                                    messageBox.classList.add(mensagemTipo); // Adiciona a classe de tipo (alert ou error)
                                }
                            }
                        })
                        .catch(error => {
                            console.error('Erro ao obter estado do motor:', error);
                            messageBox.innerHTML = 'Erro ao carregar o estado do motor.';
                            messageBox.className = 'error'; // Erro em vermelho
                        });
                }
            
                // Função para configurar o clique nos botões
                function setupButtonClick(button, motor, buttonClass) {
                    button.addEventListener('click', function (event) {
                        event.preventDefault();
            
                        if (button.classList.contains('btn-disabled')) {
                            return; // Impede ação se o botão estiver desabilitado
                        }
            
                        const action = button.innerHTML.includes('Desligar') ? 'desligar' : 'ligar';
            
                        fetch(`/toggle?action=${action}&motor=${motor}`)
                            .then(response => response.text())
                            .then(() => updateButtonState(button, motor, buttonClass))
                            .catch(error => {
                                console.error('Erro ao enviar comando para o motor:', error);
                                messageBox.innerHTML = 'Erro ao enviar comando para o motor.';
                                messageBox.className = 'error'; // Erro em vermelho
                            });
                    });
                }
            
                // Configura os botões de controle dos motores
                setupButtonClick(toggleButtonMotor1, '1', 'btn-motor1');
                setupButtonClick(toggleButtonMotor2, '2', 'btn-motor2');
                setupButtonClick(toggleButtonMotor3, '3', 'btn-motor3');
            
                // Atualiza o estado dos botões a cada 5 segundos
                setInterval(() => {
                    updateButtonState(toggleButtonMotor1, '1', 'btn-motor1');
                    updateButtonState(toggleButtonMotor2, '2', 'btn-motor2');
                    updateButtonState(toggleButtonMotor3, '3', 'btn-motor3');
                }, 5000);
            
                // Atualiza o estado dos botões ao carregar a página
                updateButtonState(toggleButtonMotor1, '1', 'btn-motor1');
                updateButtonState(toggleButtonMotor2, '2', 'btn-motor2');
                updateButtonState(toggleButtonMotor3, '3', 'btn-motor3');
            
                // Função para alternar entre os temas
                function toggleTheme(theme) {
                    const isThemeActive = document.body.classList.contains(theme);
                    document.body.classList.remove('night-mode', 'high-contrast');
            
                    if (!isThemeActive) {
                        document.body.classList.add(theme);
                        localStorage.setItem('theme', theme);
                    } else {
                        localStorage.removeItem('theme');
                    }
                }
            
                // Aplica o tema salvo no localStorage ao carregar a página
                const savedTheme = localStorage.getItem('theme');
                if (savedTheme) {
                    document.body.classList.add(savedTheme);
                }
            
                // Configura os eventos de clique para os botões de tema
                if (nightModeButton) {
                    nightModeButton.addEventListener('click', () => toggleTheme('night-mode'));
                }
            
                if (highContrastButton) {
                    highContrastButton.addEventListener('click', () => toggleTheme('high-contrast'));
                }
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
// Cria um JSON com o estado dos motores, manutenção e sobrecarga
String stateJson = "{\"compressorLigadoMotor1\":" + String(motoresLigados[0]) +
                  ",\"compressorLigadoMotor2\":" + String(motoresLigados[1]) +
                  ",\"compressorLigadoMotor3\":" + String(motoresLigados[2]) +
                  ",\"sistemaEmManutencao\":" + String(sistemaEmManutencao) +
                  ",\"sobrecargaMotor1\":" + String(sobrecargaDetectada[0]) +
                  ",\"sobrecargaMotor2\":" + String(sobrecargaDetectada[1]) +
                  ",\"sobrecargaMotor3\":" + String(sobrecargaDetectada[2]) + "}";
// Envia o JSON em resposta a uma requisição GET
request->send(200, "application/json", stateJson); });
}