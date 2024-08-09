#include "wificonexao.h"       // Inclui o arquivo de cabeçalho com funções de conexão Wi-Fi
#include <ESPAsyncWebServer.h> // Biblioteca para servidor web assíncrono
#include "wifiinterface.h"     // Inclui o cabeçalho deste arquivo

extern AsyncWebServer server; // Declaração externa do servidor

const String getWiFiGerenciamentoPage()
{
    String ipAddress = WiFi.localIP().toString(); // Obtém o IP do dispositivo
    String page = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-br">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Gerenciamento Wi-Fi</title>
    <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css">
    <style>
        /* CSS separado */

        /* Estilos para o corpo da página */
        body {
            font-family: Arial, sans-serif;
            background-color: #f8f9fa;
            margin: 0;
            padding: 0;
            display: flex;
            flex-direction: column;
            min-height: 100vh;
        }

        /* Estilos para o contêiner principal */
        .container {
            background-color: #ffffff;
            padding: 20px;
            border-radius: 5px;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
            width: 100%;
            max-width: 600px;
            margin: 30px auto;
            flex: 1;
        }

        /* Estilos para o rodapé */
        .footer {
            background-color: #007bff;
            color: white;
            text-align: center;
            padding: 10px 0;
            font-size: 14px;
            margin-top: 30px;
        }

        /* Estilos para a lista de redes salvas */
        #saved-networks {
            max-height: 300px;
            overflow-y: auto;
            margin-bottom: 20px;
        }

        /* Estilos para o botão de conectar */
        .btn-success {
            background-color: #28a745;
            color: white;
        }

        .btn-success:hover {
            background-color: #218838;
        }

        /* Estilos para o botão de voltar */
        .btn-blue {
            background-color: #007bff;
            color: white;
            border: none;
            padding: 10px 20px;
            font-size: 16px;
            cursor: pointer;
            border-radius: 3px;
            width: 100%;
        }

        .btn-blue:hover {
            background-color: #0056b3;
        }

        /* Estilos para o botão de deletar */
        .btn-custom-danger {
            background-color: #dc3545;
            color: white;
            border: none;
            padding: 5px 10px;
            font-size: 14px;
            cursor: pointer;
            border-radius: 3px;
            text-decoration: none;
        }

        .btn-custom-danger:hover {
            background-color: #c82333;
        }

        /* Estilos para o grupo de entrada com botão de alternar senha */
        .input-group {
            display: flex;
            align-items: center;
        }

        .input-group-append {
            margin-left: -1px;
        }

        /* Estilos para o contêiner de IP */
        #ip-address-container {
            text-align: center;
            margin-top: 20px;
        }

    </style>
</head>
<body>
    <div class="container">
        <h2>Gerenciamento de Redes Wi-Fi</h2>
        <div id="message" class="alert" role="alert"></div>
        <form id="save-form" action="/salvarwifi" method="post">
            <div class="form-group">
                <label for="ssid">SSID da Rede Wi-Fi:</label>
                <input type="text" id="ssid" name="ssid" class="form-control" placeholder="Digite o SSID" required>
            </div>
            <div class="form-group">
                <label for="password">Senha:</label>
                <div class="input-group">
                    <input type="password" id="password" name="password" class="form-control" placeholder="Digite a senha" required>
                    <div class="input-group-append">
                        <button class="btn btn-outline-secondary" type="button" id="toggle-password">Mostrar</button>
                    </div>
                </div>
            </div>
            <button type="submit" class="btn btn-success btn-block">Conectar</button>
            <button type="button" onclick="window.history.back()" class="btn-blue">Voltar</button>
        </form>
        <hr>
        <div id="saved-networks">Aguardando redes salvas...</div>
        <div id="ip-address-container">
            <h4>Endereço IP Atual:</h4>
            <p id="ip-address">Conectando...</p>
        </div>
    </div>
    <div class="footer">
        <p>Aplicação desenvolvida pela Turma de Informática Para Internet Trilhas de Futuro 2024</p>
        <p>Instrutor: Lenon Yuri</p>
    </div>
    <script>
        // -------------------------------------------------------------------------
        // Função para buscar o endereço IP e atualizar a interface
        // -------------------------------------------------------------------------
        function fetchIPAddress() {
            fetch('/ipaddress')
                .then(response => {
                    if (!response.ok) {
                        throw new Error('Network response was not ok');
                    }
                    return response.text(); // Converte a resposta em texto
                })
                .then(data => {
                    var ipAddressElement = document.getElementById('ip-address');
                    ipAddressElement.textContent = data; // Atualiza o elemento com o IP
                })
                .catch(error => {
                    console.error('Erro ao buscar o endereço IP:', error);
                    var ipAddressElement = document.getElementById('ip-address');
                    ipAddressElement.textContent = 'Erro ao buscar o IP';
                });
        }

        // -------------------------------------------------------------------------
        // Atualiza o endereço IP ao carregar a página e a cada 5 segundos
        // -------------------------------------------------------------------------
        document.addEventListener('DOMContentLoaded', function() {
            fetchIPAddress(); // Chama a função para buscar o IP na inicialização
            setInterval(fetchIPAddress, 5000); // Atualiza o IP a cada 5 segundos
        });
    </script>
</body>
</html>
    )rawliteral";
    return page;
}

// Função que retorna o endereço IP local do dispositivo
String getIPAddress()
{
    return WiFi.localIP().toString(); // Função que retorna o IP local como uma string
}

// Função para configurar as rotas relacionadas ao WiFi
void setupWiFiInterface()
{
    server.on("/ipaddress", HTTP_GET, [](AsyncWebServerRequest *request)
              {
                  request->send(200, "text/plain", getIPAddress()); // Responde com o endereço IP do dispositivo
              });

    // Você pode adicionar mais rotas específicas relacionadas ao WiFi aqui
}