#include <ESPAsyncWebServer.h>
#include <EEPROM.h>
#include <WiFi.h>
#include "configuracaowifi.h"
#include "wificonexao.h"

// Função para configurar a página de configuração WiFi
void setupWiFiConfigPage(AsyncWebServer& server) {
    server.on("/configuracaowifi", HTTP_GET, [](AsyncWebServerRequest *request) {
        String html = R"rawliteral(
            <!DOCTYPE html>
            <html lang="pt-br">
            <head>
                <meta charset="UTF-8">
                <meta name="viewport" content="width=device-width, initial-scale=1.0">
                <title>Configuração WiFi</title>
                <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css">
                <style>
                    body {
                        font-family: Arial, sans-serif;
                        background-color: #f8f9fa;
                        height: 100vh;
                        display: flex;
                        justify-content: center;
                        align-items: center;
                        margin: 0;
                        padding: 0;
                    }
                    .config-container {
                        background-color: #ffffff;
                        padding: 30px;
                        border-radius: 8px;
                        box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
                        width: 100%;
                        max-width: 400px;
                        text-align: center;
                    }
                    .config-title {
                        font-size: 28px;
                        margin-bottom: 20px;
                        color: #007bff;
                        font-weight: 500;
                    }
                    .form-group {
                        margin-bottom: 15px;
                    }
                    .btn-secondary {
                        margin-top: 10px;
                    }
                </style>
            </head>
            <body>
                <div class="config-container">
                    <h2 class="config-title">Configuração WiFi</h2>
                    <form id="wifi-form" action="/save-wifi" method="post">
                        <div class="form-group">
                            <label for="network">SSID</label>
                            <input type="text" id="network" name="ssid" class="form-control form-control-lg" placeholder="Digite o SSID" required>
                        </div>
                        <div class="form-group">
                            <label for="password">Senha</label>
                            <input type="password" id="password" name="password" class="form-control form-control-lg" placeholder="Digite a Senha" required>
                        </div>
                        <button type="submit" class="btn btn-primary btn-lg btn-block">Salvar</button>
                        <button type="button" onclick="updateNetworks()" class="btn btn-secondary btn-lg btn-block mt-3">Atualizar Redes</button>
                        <button type="button" onclick="goHome()" class="btn btn-secondary btn-lg btn-block mt-3">Voltar</button>
                    </form>
                </div>
                <script>
                    function updateNetworks() {
                        fetch('/scan-networks')
                        .then(response => response.json())
                        .then(data => {
                            const networkInput = document.getElementById('network');
                            // Atualiza o valor do input com a primeira rede disponível, se houver
                            if (data.length > 0) {
                                networkInput.value = data[0];
                            }
                        });
                    }

                    function goHome() {
                        window.location.href = '/';
                    }

                    document.addEventListener('DOMContentLoaded', updateNetworks);
                </script>
            </body>
            </html>
        )rawliteral";

        // Envia a resposta ao cliente
        request->send(200, "text/html", html);
    });

    server.on("/scan-networks", HTTP_GET, [](AsyncWebServerRequest *request) {
        String jsonResponse = "[";
        int n = WiFi.scanNetworks();
        for (int i = 0; i < n; ++i) {
            if (i > 0) jsonResponse += ",";
            jsonResponse += "\"" + String(WiFi.SSID(i)) + "\"";
        }
        jsonResponse += "]";
        request->send(200, "application/json", jsonResponse);
    });

    server.on("/save-wifi", HTTP_POST, [](AsyncWebServerRequest *request) {
        String ssid;
        String password;

        if (request->hasParam("ssid", true)) {
            ssid = request->getParam("ssid", true)->value();
        }
        if (request->hasParam("password", true)) {
            password = request->getParam("password", true)->value();
        }

        // Salva as credenciais no sistema de arquivos (SPIFFS, EEPROM, etc.)
        EEPROM.begin(512);
        saveStringToEEPROM(0, ssid);
        saveStringToEEPROM(100, password);
        EEPROM.commit();

        // Conecta à rede WiFi
        WiFi.begin(ssid.c_str(), password.c_str());

        // Espera um pouco para tentar conectar
        unsigned long startAttemptTime = millis();
        while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
            delay(100);
        }

        String message;
        if (WiFi.status() == WL_CONNECTED) {
            message = "Conectado com sucesso! IP: " + WiFi.localIP().toString();
        } else {
            message = "Falha na conexão com o WiFi.";
        }

        request->send(200, "text/plain", message);
    });
}

// Função para salvar uma string na EEPROM
void saveStringToEEPROM(int startAddress, const String& str) {
    int len = str.length();
    for (int i = 0; i < len; i++) {
        EEPROM.write(startAddress + i, str[i]);
    }
    EEPROM.write(startAddress + len, '\0'); // Adiciona o terminador de string
}
