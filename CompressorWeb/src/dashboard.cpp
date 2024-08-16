#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <SPIFFS.h>
#include "dashboard.h"
#include "ligadesliga.h"
#include "autenticador.h"
#include "manutencao.h"

/**
 * Configura a rota para a página do dashboard e os endpoints relacionados.
 *
 * @param server A instância do servidor web assíncrono.
 */
void setupDashboardPage(AsyncWebServer &server)
{
    server.on("/dashboard", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        if (!isAuthenticated(request))
        {
            redirectToAccessDenied(request);
            return;
        }

        String html = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-br">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Dashboard</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f8f9fa;
            margin: 0;
            padding: 0;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            flex-direction: column;
        }
        .dashboard-container {
            background-color: #ffffff;
            padding: 20px;
            border-radius: 8px;
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
            text-align: center;
            width: 100%;
            max-width: 400px;
        }
        .dashboard-title {
            font-size: 28px;
            margin-bottom: 20px;
            color: #004085;
        }
        .btn {
            display: block;
            width: calc(100% - 24px);
            padding: 12px;
            font-size: 16px;
            margin: 10px auto;
            border: none;
            border-radius: 8px;
            cursor: pointer;
            color: white;
            transition: background-color 0.3s, transform 0.3s;
        }
        .btn-motor1 { background-color: #04997d; }
        .btn-motor2 { background-color: #2ae6d4; }
        .btn-motor3 { background-color: #18df94; }
        .btn-desligar { background-color: #c82333; }
        .btn-disabled {
            background-color: #6c757d;
            cursor: not-allowed;
        }
        .btn-link {
            display: block;
            width: calc(100% - 24px);
            padding: 12px;
            font-size: 16px;
            margin: 10px auto;
            border-radius: 8px;
            text-decoration: none;
            color: white;
            transition: background-color 0.3s, transform 0.3s;
        }
        .btn-link-umidade { background-color: #004085; }
        .btn-link-pressao { background-color: #343a40; }
        .btn-link-logout { background-color: #c82333; }
        .btn-link:hover {
            opacity: 0.8;
            transform: scale(1.02);
        }
        .footer {
            width: 100%;
            background-color: #004085;
            color: white;
            text-align: center;
            padding: 10px 0;
            position: fixed;
            bottom: 0;
            font-size: 14px;
        }
        #messageBox {
            margin-top: 15px;
            font-size: 16px;
            color: #c82333;
        }
    </style>
</head>
<body>
    <div class="dashboard-container">
        <h2 class="dashboard-title">Bem-vindo ao Dashboard</h2>
        <button class="btn btn-motor1" id="toggleButtonMotor1">Carregando...</button>
        <button class="btn btn-motor2" id="toggleButtonMotor2">Carregando...</button>
        <button class="btn btn-motor3" id="toggleButtonMotor3">Carregando...</button>
        <a href="/umidade" class="btn btn-link btn-link-umidade">Umidade</a>
        <a href="/pressao" class="btn btn-link btn-link-pressao">Pressão</a>
        <a href="/logout" class="btn btn-link btn-link-logout">Logout</a>
    </div>
    <div id="messageBox"></div>
    <div class="footer">
        <p>Aplicação desenvolvida pela Turma de Informática Para Internet Trilhas de Futuro 2024</p>
        <p>Instrutor: Lenon Yuri</p>
    </div>
    <script>
        document.addEventListener('DOMContentLoaded', function() {
            var toggleButtonMotor1 = document.getElementById('toggleButtonMotor1');
            var toggleButtonMotor2 = document.getElementById('toggleButtonMotor2');
            var toggleButtonMotor3 = document.getElementById('toggleButtonMotor3');
            var messageBox = document.getElementById('messageBox');
            var previousMaintenanceState = null;

            function updateButtonState(button, motor, buttonClass) {
                fetch('/motor-state')
                    .then(response => response.json())
                    .then(data => {
                        var compressorLigado = data['compressorLigadoMotor' + motor];
                        var sistemaEmManutencao = data.sistemaEmManutencao;
                        var horaAtual = new Date().getHours() + (new Date().getMinutes() / 60);

                        if (sistemaEmManutencao) {
                            button.innerHTML = 'Compressor ' + motor + ' em manutenção';
                            button.classList.add('btn-disabled');
                            button.classList.remove(buttonClass, 'btn-desligar');
                            messageBox.innerHTML = '';
                        } else {
                            if (compressorLigado) {
                                button.innerHTML = 'Desligar ' + motor;
                                button.classList.add('btn-desligar');
                                button.classList.remove(buttonClass);
                                messageBox.innerHTML = 'O compressor ' + motor + ' acabou de ser ligado, aguarde o tempo limite para desligar novamente.';
                            } else {
                                button.innerHTML = 'Ligar ' + motor;
                                button.classList.remove('btn-desligar');
                                button.classList.add(buttonClass);
                                messageBox.innerHTML = '';
                            }

                            if (horaAtual < 7.5 || horaAtual >= 22.5) {
                                messageBox.innerHTML = 'Compressor ' + motor + ' desligado devido ao horário de funcionamento.';
                            } else if (horaAtual >= 7.5 && horaAtual < 8) {
                                messageBox.innerHTML = 'Compressor ' + motor + ' ligado após o horário de funcionamento, desligue após o uso.';
                            }
                        }

                        if (previousMaintenanceState !== null && previousMaintenanceState !== sistemaEmManutencao && !sistemaEmManutencao) {
                            location.reload();
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
                            updateButtonState(button, motor, buttonClass);
                        })
                        .catch(error => console.error('Erro ao enviar requisição:', error));
                });
            }

            setupButtonClick(toggleButtonMotor1, '1', 'btn-motor1');
            setupButtonClick(toggleButtonMotor2, '2', 'btn-motor2');
            setupButtonClick(toggleButtonMotor3, '3', 'btn-motor3');

            setInterval(() => {
                updateButtonState(toggleButtonMotor1, '1', 'btn-motor1');
                updateButtonState(toggleButtonMotor2, '2', 'btn-motor2');
                updateButtonState(toggleButtonMotor3, '3', 'btn-motor3');
            }, 5000);

            updateButtonState(toggleButtonMotor1, '1', 'btn-motor1');
            updateButtonState(toggleButtonMotor2, '2', 'btn-motor2');
            updateButtonState(toggleButtonMotor3, '3', 'btn-motor3');
        });
    </script>
</body>
</html>
        )rawliteral";

        request->send(200, "text/html", html); });

    server.on("/motor-state", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        String stateJson = "{\"compressorLigadoMotor1\":" + String(motoresLigados[0]) +
                            ",\"compressorLigadoMotor2\":" + String(motoresLigados[1]) +
                            ",\"compressorLigadoMotor3\":" + String(motoresLigados[2]) +
                            ",\"sistemaEmManutencao\":" + String(sistemaEmManutencao) + "}";
        request->send(200, "application/json", stateJson); });
}
