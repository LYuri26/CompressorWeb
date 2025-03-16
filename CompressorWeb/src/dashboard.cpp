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
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <link rel="preconnect" href="https://fonts.googleapis.com" />
    <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin />
    <link href="https://fonts.googleapis.com/css2?family=Playfair+Display:ital,wght@0,400..900;1,400..900&display=swap"
        rel="stylesheet" />
    <title>Dashboard</title>
    <style>
        /* Estilos gerais do body e container */
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
            font-family: 'Playfair Display', serif;
            margin-bottom: 20px;
            background-image: linear-gradient(to bottom,
                    rgb(255, 221, 0),
                    rgba(246, 15, 15, 0.975),
                    rgb(249, 216, 28));
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

        /* Estilo base dos botões */
        .round-button {
            width: 100px;
            height: 100px;
            border-radius: 50%;
            display: flex;
            align-items: center;
            justify-content: center;
            text-align: center;
            font-size: clamp(0.6em, 3vw, 0.8em);
            /* Fonte responsiva */
            font-weight: bold;
            margin: 0 auto;
            cursor: pointer;
            transition: all 0.3s ease;
            color: white;
            /* Texto branco por padrão */
            border: 4px solid rgba(0, 0, 0, 0.2);
            outline: none;
            position: relative;
            overflow: hidden;
            white-space: normal;
            padding: 10px;
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.3);
            background-color: #32cd32;
            /* Cor padrão verde */
        }

        /* Efeito de pressionar o botão */
        .round-button:active {
            transform: scale(0.95);
            box-shadow: 0 2px 5px rgba(0, 0, 0, 0.2) inset;
        }

        /* Efeito de borda metálica */
        .round-button::before {
            content: '';
            position: absolute;
            top: -4px;
            left: -4px;
            right: -4px;
            bottom: -4px;
            border-radius: 50%;
            border: 4px solid rgba(255, 255, 255, 0.5);
            box-shadow: 0 0 10px rgba(255, 255, 255, 0.5);
        }

        /* Efeito de luz ao redor do botão quando ativo */
        .round-button.active::before {
            box-shadow: 0 0 20px rgba(255, 255, 255, 0.8);
        }

        /* Estilo para botões desativados */
        .btn-disabled {
            cursor: not-allowed;
            opacity: 0.6;
        }

        /* Estilo para manutenção */
        .btn-maintenance {
            background-color: #ff8c00;
            /* Laranja */
            color: white;
        }

        /* Estilo para sobrecarga */
        .btn-overload {
            background-color: #ff4500;
            /* Laranja escuro */
            color: white;
        }

        /* Estilo para fora do horário */
        .btn-out-of-hours {
            background-color: #000000;
            /* Preto */
            color: white;
        }

        /* Estilo para ligar */
        .btn-ligar {
            background-color: #32cd32;
            /* Verde */
            color: white;
        }

        /* Estilo para desligar */
        .btn-desligar {
            background-color: #ff4444;
            /* Vermelho */
            color: white;
        }

        /* Efeito de texto/ícone dentro do botão */
        .round-button span {
            font-size: clamp(0.6em, 3vw, 0.8em);
            /* Tamanho da fonte responsivo */
            font-weight: bold;
            text-transform: uppercase;
            color: inherit;
            /* Herda a cor do botão */
            white-space: normal;
            /* Permite que o texto quebre em várias linhas */
            text-align: center;
            /* Centraliza o texto */
            line-height: 1.3;
            /* Ajusta o espaçamento entre linhas */
            padding: 5px;
            /* Espaçamento interno */
            display: flex;
            align-items: center;
            justify-content: center;
            word-wrap: break-word;
            /* Quebra palavras longas */
        }

        /* Efeito de hover para botões ativos */
        .round-button:not(.btn-disabled):hover {
            opacity: 0.9;
            transform: scale(1.05);
        }

        .message-box {
            margin-top: 10px;
            padding: 10px;
            border-radius: 5px;
            font-size: 16px;
            text-align: left;
            transition: background-color 0.3s, color 0.3s;
        }

        /* Estilos padronizados para todos os messageBoxMotor */

        /* Alert - Amarelo e suas variações */
        #messageBoxMotor1.alert,
        #messageBoxMotor2.alert,
        #messageBoxMotor3.alert {
            background-color: #fff3e0;
            /* Amarelo claro */
            color: #ef6c00;
            /* Laranja escuro */
            border: 2px solid #ef6c00;
        }

        /* Error - Preto e suas variações */
        #messageBoxMotor1.error,
        #messageBoxMotor2.error,
        #messageBoxMotor3.error {
            background-color: #000000;
            /* Preto */
            color: #ffffff;
            /* Branco */
            border: 2px solid #333333;
            /* Cinza escuro */
            border-radius: 5px;
        }

        /* Success - Verde e suas variações */
        #messageBoxMotor1.success,
        #messageBoxMotor2.success,
        #messageBoxMotor3.success {
            background-color: #e8f5e9;
            /* Verde claro */
            color: #2e7d32;
            /* Verde escuro */
            border: 2px solid #2e7d32;
            border-radius: 5px;
        }

        /* Info - Cinza e suas variações */
        #messageBoxMotor1.info,
        #messageBoxMotor2.info,
        #messageBoxMotor3.info {
            background-color: #f5f5f5;
            /* Cinza claro */
            color: #616161;
            /* Cinza escuro */
            border: 2px solid #bdbdbd;
            /* Cinza médio */
            border-radius: 5px;
        }

        /* Warning - Vermelho e suas variações */
        #messageBoxMotor1.warning,
        #messageBoxMotor2.warning,
        #messageBoxMotor3.warning {
            background-color: #ffebee;
            /* Vermelho claro */
            color: #c62828;
            /* Vermelho escuro */
            border: 2px solid #c62828;
            border-radius: 5px;
        }

        /* Estilo dos links */
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

        .btn-link-umidade {
            background-color: #004085;
        }

        .btn-link-pressao {
            background-color: #344037;
        }

        .btn-link-logout {
            background-color: #c82333;
        }

        .btn-link:hover {
            opacity: 0.8;
            transform: scale(1.02);
        }

        /* Rodapé */
        .footer {
            width: 100%;
            background-color: #ee641a;
            color: white;
            text-align: center;
            padding: 10px 0;
            font-size: 14px;
            margin-top: auto;
        }

        /* Botão de modo noturno */
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

        #nightModeButton {
            background-color: rgb(234, 161, 15);
        }

        #nightModeButton:hover {
            background-color: #8e5204;
            transform: scale(1.05);
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
                font-size: 0.8em;
                /* Ajuste do tamanho da fonte */
            }

            .round-button span {
                font-size: 0.8em;
                /* Ajuste do tamanho da fonte */
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
                font-size: 0.7em;
                /* Ajuste do tamanho da fonte */
            }

            .round-button span {
                font-size: 0.7em;
                /* Ajuste do tamanho da fonte */
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

            .countdown {
                margin-top: 10px;
                font-size: 14px;
                font-weight: bold;
                color: #333;
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

        <div class="button-group">
            <button class="btn btn-motor1 round-button" id="toggleButtonMotor1"
                aria-label="Controle do motor compressor">
                <span>Carregando...</span>
            </button>
            <p>Motor Compressor</p>
            <div id="countdownMotor1" class="countdown"></div> <!-- Contagem regressiva para o Motor 1 -->
        </div>

        <div class="button-group">
            <button class="btn btn-motor2 round-button" id="toggleButtonMotor2"
                aria-label="Controle do motor ventilador">
                <span>Carregando...</span>
            </button>
            <p>Motor Ventilador</p>
            <div id="countdownMotor2" class="countdown"></div> <!-- Contagem regressiva para o Motor 2 -->
        </div>

        <div class="button-group">
            <button class="btn btn-motor3 round-button" id="toggleButtonMotor3" aria-label="Controle do motor secador">
                <span>Carregando...</span>
            </button>
            <p>Motor Secador</p>
            <div id="countdownMotor3" class="countdown"></div> <!-- Contagem regressiva para o Motor 3 -->
        </div>

        <div id="messageBoxMotor1" class="message-box" role="alert"></div>
        <div id="messageBoxMotor2" class="message-box" role="alert"></div>
        <div id="messageBoxMotor3" class="message-box" role="alert"></div>

        <a href="/umidade" class="btn btn-link btn-link-umidade" aria-label="Página de umidade">Umidade</a>
        <a href="/pressao" class="btn btn-link btn-link-pressao" aria-label="Página de pressão">Pressão</a>
        <a href="/logout" class="btn btn-link btn-link-logout" aria-label="Logout">Logout</a>

        <button id="nightModeButton" aria-label="Modo Noturno">
            Modo Noturno
        </button>
    </div>

    <div class="footer">
        <p>
            Aplicação desenvolvida pela Turma de Informática Para Internet Trilhas
            de Futuro 2024
        </p>
        <p>Instrutor: Lenon Yuri</p>
    </div>

    <script>
document.addEventListener('DOMContentLoaded', function () {
    // Seleciona os elementos do DOM
    const toggleButtons = [
        document.getElementById('toggleButtonMotor1'),
        document.getElementById('toggleButtonMotor2'),
        document.getElementById('toggleButtonMotor3')
    ];
    const countdownElements = [
        document.getElementById('countdownMotor1'),
        document.getElementById('countdownMotor2'),
        document.getElementById('countdownMotor3')
    ];
    const messageBoxes = [
        document.getElementById('messageBoxMotor1'),
        document.getElementById('messageBoxMotor2'),
        document.getElementById('messageBoxMotor3')
    ];
    const nightModeButton = document.getElementById('nightModeButton');

    // Função para iniciar a contagem regressiva
    function startCountdown(motorIndex, duration) {
        let timer = duration;
        const interval = setInterval(() => {
            const minutes = Math.floor(timer / 60);
            const seconds = timer % 60;
            countdownElements[motorIndex].textContent = `Aguardar: ${String(minutes).padStart(2, '0')}:${String(seconds).padStart(2, '0')}`;

            if (--timer < 0) {
                clearInterval(interval);
                countdownElements[motorIndex].textContent = '';
                toggleButtons[motorIndex].disabled = false;
            }
        }, 1000);
    }

    // Função para atualizar o estado dos botões e mensagens
// Função para atualizar o estado dos botões e mensagens
function updateButtonState() {
    fetch('/motor-state')
        .then(response => {
            if (!response.ok) throw new Error('Erro na requisição');
            return response.json();
        })
        .then(data => {
            for (let i = 0; i < 3; i++) {
                const compressorLigado = data[`compressorLigadoMotor${i + 1}`];
                const sistemaEmManutencao = data.sistemaEmManutencao;
                const sobrecarga = data[`sobrecargaMotor${i + 1}`];
                const horaAtual = new Date().getHours() + new Date().getMinutes() / 60;
                const motorTimerActive = data[`motorTimerActive${i + 1}`];
                const pressostatoAtivo = data[`pressostatoAtivoMotor${i + 1}`]; // Novo campo para pressostato

                // Atualiza o estado do botão
                if (sobrecarga) {
                    toggleButtons[i].innerHTML = '<span>Sobrecarga</span>';
                    toggleButtons[i].classList.add('btn-overload', 'btn-disabled');
                    toggleButtons[i].classList.remove('btn-desligar', 'btn-ligar');
                    messageBoxes[i].className = 'error';
                    messageBoxes[i].textContent = `Motor ${i + 1} está com sobrecarga. Desativado.`;
                } else if (sistemaEmManutencao) {
                    toggleButtons[i].innerHTML = '<span>Manutenção</span>';
                    toggleButtons[i].classList.add('btn-maintenance', 'btn-disabled');
                    toggleButtons[i].classList.remove('btn-desligar', 'btn-ligar');
                    messageBoxes[i].className = 'warning';
                    messageBoxes[i].textContent = `Motor ${i + 1} está em manutenção. Desativado.`;
                } else if (horaAtual < 8 || horaAtual >= 22) {
                    toggleButtons[i].innerHTML = '<span>Fora do Horário</span>';
                    toggleButtons[i].classList.add('btn-out-of-hours', 'btn-disabled');
                    toggleButtons[i].classList.remove('btn-desligar', 'btn-ligar');
                    messageBoxes[i].className = 'alert';
                    messageBoxes[i].textContent = `Motor ${i + 1} está fora do horário de funcionamento. Desativado.`;
                } else if (pressostatoAtivo && i === 0) { // Apenas para o motor compressor (i === 0)
                    toggleButtons[i].innerHTML = '<span>Pressostato Ativo</span>';
                    toggleButtons[i].classList.add('btn-disabled');
                    toggleButtons[i].classList.remove('btn-desligar', 'btn-ligar');
                    messageBoxes[i].className = 'info';
                    messageBoxes[i].textContent = `Pressostato ativo. Motor ${i + 1} bloqueado.`;
                } else {
                    if (compressorLigado) {
                        toggleButtons[i].innerHTML = '<span>Desligar</span>';
                        toggleButtons[i].classList.add('btn-desligar');
                        toggleButtons[i].classList.remove('btn-ligar');
                        messageBoxes[i].className = 'success';
                        messageBoxes[i].textContent = `Motor ${i + 1} está ligado.`;
                    } else {
                        toggleButtons[i].innerHTML = '<span>Ligar</span>';
                        toggleButtons[i].classList.add('btn-ligar');
                        toggleButtons[i].classList.remove('btn-desligar', 'btn-disabled');
                        messageBoxes[i].className = 'info';
                        messageBoxes[i].textContent = `Motor ${i + 1} está desligado.`;
                    }
                }

                // Verifica se o cronômetro está ativo
                if (motorTimerActive) {
                    toggleButtons[i].disabled = true;
                    toggleButtons[i].classList.add('btn-disabled');
                } else {
                    toggleButtons[i].disabled = false;
                    toggleButtons[i].classList.remove('btn-disabled');
                }
            }
        })
        .catch(error => {
            console.error('Erro ao obter estado do motor:', error);
            messageBoxes.forEach(box => {
                box.textContent = 'Erro ao carregar o estado do motor.';
                box.className = 'error';
            });
        });
}

    // Função para configurar o clique nos botões
    function setupButtonClick(button, motorIndex) {
        button.addEventListener('click', function (event) {
            event.preventDefault();

            if (button.classList.contains('btn-disabled')) return;

            button.disabled = true;

            const action = button.innerHTML.includes('Desligar') ? 'desligar' : 'ligar';
            const duration = action === 'ligar' ? 3600 : 600; // 10 minutos para ligar, 1 hora para desligar

            fetch(`/toggle?action=${action}&motor=${motorIndex + 1}`)
                .then(response => {
                    if (!response.ok) throw new Error('Erro ao enviar comando');
                    updateButtonState();
                    startCountdown(motorIndex, duration);
                })
                .catch(error => {
                    console.error('Erro ao enviar comando para o motor:', error);
                    button.disabled = false;
                });
        });
    }

    // Configura os botões
    toggleButtons.forEach((button, index) => setupButtonClick(button, index));

    // Atualiza o estado dos botões a cada 2 segundos
    setInterval(updateButtonState, 2000);

    // Atualiza o estado dos botões ao carregar a página
    updateButtonState();

    // Configura o botão de modo noturno
    if (nightModeButton) {
        nightModeButton.addEventListener('click', () => {
            document.body.classList.toggle('night-mode');
            localStorage.setItem('theme', document.body.classList.contains('night-mode') ? 'night-mode' : '');
        });
    }

    // Aplica o tema salvo (modo noturno) ao carregar a página
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
bool pressostatoAtivo = digitalRead(17) == HIGH; // Lê o estado do pressostato (G17)

String stateJson = "{\"compressorLigadoMotor1\":" + String(motoresLigados[0]) +
                ",\"compressorLigadoMotor2\":" + String(motoresLigados[1]) +
                ",\"compressorLigadoMotor3\":" + String(motoresLigados[2]) +
                ",\"sistemaEmManutencao\":" + String(sistemaEmManutencao) +
                ",\"sobrecargaMotor1\":" + String(sobrecargaDetectada[0]) +
                ",\"sobrecargaMotor2\":" + String(sobrecargaDetectada[1]) +
                ",\"sobrecargaMotor3\":" + String(sobrecargaDetectada[2]) +
                ",\"motorTimerActive1\":" + String(isMotorTimerActive(0)) +
                ",\"motorTimerActive2\":" + String(isMotorTimerActive(1)) +
                ",\"motorTimerActive3\":" + String(isMotorTimerActive(2)) +
                ",\"pressostatoAtivoMotor1\":" + String(pressostatoAtivo) + "}";
request->send(200, "application/json", stateJson); });
}