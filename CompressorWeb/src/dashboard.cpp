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
    <link href="https://fonts.googleapis.com/css2?family=Playfair+Display:ital,wght@0,400..900;1,400..900&display=swap" rel="stylesheet">
    <title>Dashboard</title>
    <style>
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

        .dashboard-container {
            background-color: #ffffff;
            padding: 20px;
            border-radius: 8px;
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
            text-align: center;
            width: 90%;
            max-width: 400px;
            margin: auto;
            transition: background-color 0.3s, color 0.3s;
        }

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

        .button-group {
            margin-bottom: 20px;
        }

        .button-group p {
            margin: 5px 0;
            font-size: 16px;
            font-weight: bold;
            color: #333;
        }

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
            border: 2px solid rgba(0, 0, 0, 0.2);
            outline: none;
            position: relative;
            overflow: hidden;
            white-space: normal; /* Permite que o texto quebre em várias linhas */
            padding: 10px;
        }

        .round-button:active {
            transform: scale(0.95);
            box-shadow: 0 2px 5px rgba(0, 0, 0, 0.2) inset;
        }

        .btn-motor1 { background-color: #32cd32; } /* Verde */
        .btn-motor2 { background-color: #32cd32; } /* Verde */
        .btn-motor3 { background-color: #32cd32; } /* Verde */

        .btn-disabled { background-color: #6c757d; cursor: not-allowed; }
        .btn-maintenance { background-color: #ff8c00; } /* Amarelo Escuro */
        .btn-overload { background-color: #ff4500; } /* Vermelho */
        .btn-out-of-hours { background-color: #808080; } /* Cinza */

        #messageBox {
            margin-top: 20px;
            padding: 10px;
            border-radius: 5px;
            background-color: #f8f9fa;
            color: #333;
            font-size: 16px;
            text-align: left;
            transition: background-color 0.3s, color 0.3s;
        }

        #messageBox.alert { background-color: #fff3e0; color: #ef6c00; border: 2px solid #ef6c00; }
        #messageBox.error { background-color: #ffebee; color: #c62828; border: 2px solid #c62828; }
        #messageBox.success { background-color: #e8f5e9; color: #2e7d32; border: 2px solid #2e7d32; }
        #messageBox.info { background-color: #e3f2fd; color: #1565c0; border: 2px solid #1565c0; }
        #messageBox.warning { background-color: #fff8e1; color: #ff8f00; border: 2px solid #ff8f00; }

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

        .btn-link:hover { opacity: 0.8; transform: scale(1.02); }

        .footer {
            width: 100%;
            background-color: #ee641a;
            color: white;
            text-align: center;
            padding: 10px 0;
            font-size: 14px;
            margin-top: auto;
        }

        #nightModeButton {
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
        #nightModeButton:hover { background-color: #8e5204; transform: scale(1.05); }

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
            border-color: rgba(255, 255, 255, 0.2);
            color: white;
        }
        .night-mode .button-group p {
            color: white;
        }

        /* Responsividade */
        @media (max-width: 600px) {
            .dashboard-container {
                width: 95%;
                padding: 15px;
            }

            .round-button {
                width: 80px;
                height: 80px;
                font-size: 12px;
            }

            .button-group p {
                font-size: 14px;
            }

            .btn-link {
                font-size: 14px;
                padding: 10px;
            }

            #nightModeButton {
                font-size: 14px;
                padding: 10px;
            }

            .footer {
                font-size: 12px;
            }
        }

        @media (max-width: 400px) {
            .round-button {
                width: 70px;
                height: 70px;
                font-size: 11px;
            }

            .button-group p {
                font-size: 12px;
            }

            .btn-link {
                font-size: 12px;
                padding: 8px;
            }

            #nightModeButton {
                font-size: 12px;
                padding: 8px;
            }

            .footer {
                font-size: 10px;
            }
        }
    </style>
</head>
<body>
    <div class="dashboard-container">
        <h2 class="dashboard-title">Bem-vindo ao Dashboard</h2>

        <!-- Grupo de botão e etiqueta para o Motor Compressor -->
        <div class="button-group">
            <button class="btn btn-motor1 round-button" id="toggleButtonMotor1" aria-label="Controle do motor compressor">
                Carregando...
            </button>
            <p>Motor Compressor</p>
        </div>

        <!-- Grupo de botão e etiqueta para o Motor Ventilador -->
        <div class="button-group">
            <button class="btn btn-motor2 round-button" id="toggleButtonMotor2" aria-label="Controle do motor ventilador">
                Carregando...
            </button>
            <p>Motor Ventilador</p>
        </div>

        <!-- Grupo de botão e etiqueta para o Motor Secador -->
        <div class="button-group">
            <button class="btn btn-motor3 round-button" id="toggleButtonMotor3" aria-label="Controle do motor secador">
                Carregando...
            </button>
            <p>Motor Secador</p>
        </div>

        <div id="messageBox" role="alert"></div>

        <a href="/umidade" class="btn btn-link btn-link-umidade" aria-label="Página de umidade">Umidade</a>
        <a href="/pressao" class="btn btn-link btn-link-pressao" aria-label="Página de pressão">Pressão</a>
        <a href="/logout" class="btn btn-link btn-link-logout" aria-label="Logout">Logout</a>

        <button id="nightModeButton" aria-label="Modo Noturno">Modo Noturno</button>
    </div>

    <div class="footer">
        <p>Aplicação desenvolvida pela Turma de Informática Para Internet Trilhas de Futuro 2024</p>
        <p>Instrutor: Lenon Yuri</p>
    </div>

    <script>
        document.addEventListener('DOMContentLoaded', function () {
            const toggleButtonMotor1 = document.getElementById('toggleButtonMotor1');
            const toggleButtonMotor2 = document.getElementById('toggleButtonMotor2');
            const toggleButtonMotor3 = document.getElementById('toggleButtonMotor3');
            const messageBox = document.getElementById('messageBox');
            const nightModeButton = document.getElementById('nightModeButton');

            function updateButtonState() {
                fetch('/motor-state')
                    .then(response => response.json())
                    .then(data => {
                        let mensagem = '';
                        let mensagemTipo = '';

                        for (let i = 1; i <= 3; i++) {
                            const compressorLigado = data['compressorLigadoMotor' + i];
                            const sistemaEmManutencao = data.sistemaEmManutencao;
                            const sobrecarga = data['sobrecargaMotor' + i];
                            const now = new Date();
                            const horaAtual = now.getHours() + (now.getMinutes() / 60);

                            let estadoMotor = '';
                            let button = document.getElementById(`toggleButtonMotor${i}`);
                            let buttonClass = `btn-motor${i}`;

                            if (sobrecarga) {
                                button.innerHTML = 'Sobrecarga';
                                button.classList.add('btn-overload', 'btn-disabled');
                                button.classList.remove(buttonClass, 'btn-desligar');
                                estadoMotor = `Motor ${i} está com sobrecarga. Desativado.`;
                                mensagemTipo = 'error';
                            } else if (sistemaEmManutencao) {
                                button.innerHTML = 'Manutenção';
                                button.classList.add('btn-maintenance', 'btn-disabled');
                                button.classList.remove(buttonClass, 'btn-desligar');
                                estadoMotor = `Motor ${i} está em manutenção. Desativado.`;
                                mensagemTipo = 'warning';
                            } else if (horaAtual < 8 || horaAtual >= 22) {
                                button.innerHTML = 'Fora do Horário';
                                button.classList.add('btn-out-of-hours', 'btn-disabled');
                                button.classList.remove(buttonClass, 'btn-desligar');
                                estadoMotor = `Motor ${i} está fora do horário de funcionamento. Desativado.`;
                                mensagemTipo = 'alert';
                            } else {
                                if (compressorLigado) {
                                    button.innerHTML = 'Desligar';
                                    button.classList.add('btn-desligar');
                                    button.classList.remove(buttonClass);
                                    estadoMotor = `Motor ${i} está ligado.`;
                                    mensagemTipo = 'success';
                                } else {
                                    button.innerHTML = 'Ligar';
                                    button.classList.remove('btn-desligar', 'btn-disabled');
                                    button.classList.add(buttonClass);
                                    estadoMotor = `Motor ${i} está desligado.`;
                                    mensagemTipo = 'info';
                                }
                            }

                            mensagem += `${estadoMotor}<br>`;
                        }

                        if (mensagem) {
                            messageBox.innerHTML = mensagem;
                            messageBox.className = '';
                            if (mensagemTipo) {
                                messageBox.classList.add(mensagemTipo);
                            }
                        }
                    })
                    .catch(error => {
                        console.error('Erro ao obter estado do motor:', error);
                        messageBox.innerHTML = 'Erro ao carregar o estado do motor.';
                        messageBox.className = 'error';
                    });
            }

            function setupButtonClick(button, motor, buttonClass) {
                button.addEventListener('click', function (event) {
                    event.preventDefault();

                    if (button.classList.contains('btn-disabled')) {
                        return;
                    }

                    button.disabled = true;

                    const action = button.innerHTML.includes('Desligar') ? 'desligar' : 'ligar';

                    fetch(`/toggle?action=${action}&motor=${motor}`)
                        .then(response => response.text())
                        .then(() => {
                            updateButtonState();
                            setTimeout(() => {
                                button.disabled = false;
                            }, 1000);
                        })
                        .catch(error => {
                            console.error('Erro ao enviar comando para o motor:', error);
                            messageBox.innerHTML = 'Erro ao enviar comando para o motor.';
                            messageBox.className = 'error';
                            button.disabled = false;
                        });
                });
            }

            setupButtonClick(toggleButtonMotor1, '1', 'btn-motor1');
            setupButtonClick(toggleButtonMotor2, '2', 'btn-motor2');
            setupButtonClick(toggleButtonMotor3, '3', 'btn-motor3');

            setInterval(() => {
                updateButtonState();
            }, 1000);

            updateButtonState();

            if (nightModeButton) {
                nightModeButton.addEventListener('click', () => {
                    document.body.classList.toggle('night-mode');
                    localStorage.setItem('theme', document.body.classList.contains('night-mode') ? 'night-mode' : '');
                });
            }

            const savedTheme = localStorage.getItem('theme');
            if (savedTheme) {
                document.body.classList.add(savedTheme);
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