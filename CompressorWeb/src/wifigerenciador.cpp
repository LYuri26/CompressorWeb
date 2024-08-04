#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include "wificonexao.h"

// Função para configurar a página de gerenciamento de Wi-Fi
void setupWiFiManagementPage(AsyncWebServer &server) {
    if (!SPIFFS.begin(true)) {
        Serial.println("Falha ao iniciar o sistema de arquivos SPIFFS");
        return;
    }

    // Rota para a página de gerenciamento de Wi-Fi
    server.on("/wifi-management", HTTP_GET, [](AsyncWebServerRequest *request) {
        String html = R"rawliteral(
            <!DOCTYPE html>
            <html lang="pt-br">
            <head>
                <meta charset="UTF-8">
                <meta name="viewport" content="width=device-width, initial-scale=1.0">
                <title>Gerenciamento Wi-Fi</title>
                <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css">
                <style>
                    body {
                        font-family: Arial, sans-serif;
                        background-color: #f8f9fa;
                        height: 100vh;
                        margin: 0;
                        padding: 0;
                    }
                    .container {
                        background-color: #ffffff;
                        padding: 20px;
                        border-radius: 5px;
                        box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
                        width: 100%;
                        max-width: 600px;
                        margin: auto;
                    }
                    .footer {
                        position: fixed;
                        bottom: 0;
                        width: 100%;
                        background-color: #007bff;
                        color: white;
                        text-align: center;
                        padding: 10px 0;
                        font-size: 14px;
                    }
                    #saved-networks {
                        max-height: 300px;
                        overflow-y: auto;
                    }
                    @media (max-width: 768px) {
                        .container {
                            padding: 15px;
                        }
                    }
                </style>
            </head>
            <body>
                <div class="container">
                    <h2>Gerenciamento de Redes Wi-Fi</h2>
                    <form id="save-form" action="/save-wifi" method="post">
                        <div class="form-group">
                            <label for="ssid">SSID da Rede Wi-Fi:</label>
                            <input type="text" id="ssid" name="ssid" class="form-control" placeholder="Digite o SSID" required>
                        </div>
                        <div class="form-group">
                            <label for="password">Senha:</label>
                            <input type="password" id="password" name="password" class="form-control" placeholder="Digite a senha" required>
                        </div>
                        <button type="submit" class="btn btn-success btn-block">Salvar</button>
                    </form>
                    <hr>
                    <div id="saved-networks">Aguardando redes salvas...</div>
                </div>
                <div class="footer">
                    <p>Aplicação desenvolvida pela Turma de Informática Para Internet Trilhas de Futuro 2024</p>
                    <p>Instrutor: Lenon Yuri</p>
                </div>
                <script>
                    function fetchSavedNetworks() {
                        fetch('/list-saved-wifi')
                            .then(response => {
                                if (!response.ok) {
                                    throw new Error('Network response was not ok');
                                }
                                return response.text();
                            })
                            .then(data => {
                                var savedNetworks = document.getElementById('saved-networks');
                                savedNetworks.innerHTML = '';
                                var networks = data.trim().split('\\n');
                                if (networks.length > 0 && networks[0] !== "") {
                                    networks.forEach(network => {
                                        var parts = network.split(',');
                                        savedNetworks.innerHTML += '<p><strong>SSID:</strong> ' + parts[0] + ' <a href="/delete-wifi?ssid=' + parts[0] + '">Delete</a></p>';
                                    });
                                } else {
                                    savedNetworks.innerHTML = '<p>Nenhuma rede salva encontrada.</p>';
                                }
                            })
                            .catch(error => {
                                console.error('Erro ao buscar as redes salvas:', error);
                                var savedNetworks = document.getElementById('saved-networks');
                                savedNetworks.innerHTML = '<p>Erro ao buscar redes salvas.</p>';
                            });
                    }

                    document.addEventListener('DOMContentLoaded', function() {
                        fetchSavedNetworks();
                    });
                </script>
            </body>
            </html>
        )rawliteral";

        request->send(200, "text/html", html);
    });

    // Rota para listar redes Wi-Fi salvas
    server.on("/list-saved-wifi", HTTP_GET, [](AsyncWebServerRequest *request) {
        File file = SPIFFS.open("/wifiredes.txt", FILE_READ);
        if (!file) {
            request->send(500, "text/plain", "Erro ao abrir o arquivo de redes Wi-Fi");
            return;
        }

        String networks = file.readString();
        file.close();
        request->send(200, "text/plain", networks);
    });

    // Rota para salvar uma nova rede Wi-Fi
    server.on("/save-wifi", HTTP_POST, [](AsyncWebServerRequest *request) {
        String ssid;
        String password;

        if (request->hasParam("ssid", true) && request->hasParam("password", true)) {
            ssid = request->getParam("ssid", true)->value();
            password = request->getParam("password", true)->value();

            // Adicionar nova rede ao arquivo
            File file = SPIFFS.open("/wifiredes.txt", FILE_READ);
            String content = file.readString();
            file.close();

            String newContent = "";
            bool ssidExists = false;
            if (content.length() > 0) {
                int start = 0;
                while (start < content.length()) {
                    int end = content.indexOf('\n', start);
                    if (end == -1) {
                        end = content.length();
                    }
                    String line = content.substring(start, end);
                    int commaIndex = line.indexOf(',');
                    String savedSSID = line.substring(0, commaIndex);
                    if (savedSSID == ssid) {
                        newContent += ssid + "," + password + "\n";
                        ssidExists = true;
                    } else {
                        newContent += line + "\n";
                    }
                    start = end + 1;
                }
            }

            if (!ssidExists) {
                newContent += ssid + "," + password + "\n";
            }

            file = SPIFFS.open("/wifiredes.txt", FILE_WRITE);
            if (!file) {
                request->send(500, "text/plain", "Erro ao abrir o arquivo para escrita");
                return;
            }
            file.print(newContent);
            file.close();

            // Tentar conectar à nova rede salva
            connectToWiFi(ssid.c_str(), password.c_str());

            request->redirect("/wifi-management");
        } else {
            request->send(400, "text/plain", "Dados ausentes.");
        }
    });

    // Rota para deletar uma rede Wi-Fi salva
    server.on("/delete-wifi", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (request->hasParam("ssid")) {
            String ssidToDelete = request->getParam("ssid")->value();

            // Remover rede do arquivo
            File file = SPIFFS.open("/wifiredes.txt", FILE_READ);
            String content = file.readString();
            file.close();

            String newContent = "";
            int start = 0;
            while (start < content.length()) {
                int end = content.indexOf('\n', start);
                if (end == -1) {
                    end = content.length();
                }
                String line = content.substring(start, end);
                int commaIndex = line.indexOf(',');
                String savedSSID = line.substring(0, commaIndex);
                if (savedSSID != ssidToDelete) {
                    newContent += line + "\n";
                }
                start = end + 1;
            }

            file = SPIFFS.open("/wifiredes.txt", FILE_WRITE);
            if (!file) {
                request->send(500, "text/plain", "Erro ao abrir o arquivo para escrita");
                return;
            }
            file.print(newContent);
            file.close();

            request->redirect("/wifi-management");
        } else {
            request->send(400, "text/plain", "SSID ausente.");
        }
    });
}
