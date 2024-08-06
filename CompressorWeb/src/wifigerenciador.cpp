#include <WiFi.h>                   // Inclui a biblioteca WiFi para conectar-se a redes Wi-Fi
#include <ESPAsyncWebServer.h>      // Inclui a biblioteca para o servidor web assíncrono
#include <SPIFFS.h>                 // Inclui a biblioteca para o sistema de arquivos SPIFFS
#include "wificonexao.h"            // Inclui o arquivo de cabeçalho com funções de conexão Wi-Fi
#include "wifiinterface.h"

// Função para configurar a página de gerenciamento de Wi-Fi
void setupWiFiManagementPage(AsyncWebServer &server) {
    // -------------------------------------------------------------------------
    // Inicializa o sistema de arquivos SPIFFS
    // -------------------------------------------------------------------------
    if (!SPIFFS.begin(true)) {
        Serial.println("Falha ao iniciar o sistema de arquivos SPIFFS");
        return; // Interrompe a função se SPIFFS não iniciar
    }

    // -------------------------------------------------------------------------
    // Rota para a página de gerenciamento de Wi-Fi
    // -------------------------------------------------------------------------
    server.on("/wifigerenciamento", HTTP_GET, [](AsyncWebServerRequest *request) {
        // Envia a página de gerenciamento de Wi-Fi
        request->send(200, "text/html", getWiFiManagementPage());
    });

    // -------------------------------------------------------------------------
    // Rota para listar redes Wi-Fi salvas
    // -------------------------------------------------------------------------
    server.on("/listadewifi", HTTP_GET, [](AsyncWebServerRequest *request) {
        // Abre o arquivo de redes Wi-Fi para leitura
        File file = SPIFFS.open("/wifiredes.txt", FILE_READ);
        if (!file) {
            request->send(500, "text/plain", "Erro ao abrir o arquivo de redes Wi-Fi"); // Responde com erro se não conseguir abrir o arquivo
            return;
        }

        // Lê o conteúdo do arquivo
        String networks = file.readString();
        file.close(); // Fecha o arquivo após leitura
        request->send(200, "text/plain", networks); // Envia o conteúdo lido como resposta
    });

    // -------------------------------------------------------------------------
    // Rota para salvar uma nova rede Wi-Fi
    // -------------------------------------------------------------------------
    server.on("/salvarwifi", HTTP_POST, [](AsyncWebServerRequest *request) {
        String ssid;
        String password;

        if (request->hasParam("ssid", true) && request->hasParam("password", true)) {
            // Obtém o SSID e a senha do formulário
            ssid = request->getParam("ssid", true)->value();
            password = request->getParam("password", true)->value();

            // Abre o arquivo de redes Wi-Fi para leitura
            File file = SPIFFS.open("/wifiredes.txt", FILE_READ);
            if (!file) {
                request->send(500, "text/plain", "Erro ao abrir o arquivo de redes Wi-Fi"); // Responde com erro se não conseguir abrir o arquivo
                return;
            }

            // Lê o conteúdo do arquivo
            String content = file.readString();
            file.close(); // Fecha o arquivo após leitura

            String newContent = "";
            bool ssidExists = false;
            if (content.length() > 0) {
                // Processa o conteúdo existente para verificar se o SSID já está presente
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
                        newContent += ssid + "," + password + "\n"; // Atualiza a entrada existente com a nova senha
                        ssidExists = true;
                    } else {
                        newContent += line + "\n"; // Mantém a entrada existente
                    }
                    start = end + 1;
                }
            }

            if (!ssidExists) {
                newContent += ssid + "," + password + "\n"; // Adiciona nova entrada se o SSID não existir
            }

            // Abre o arquivo de redes Wi-Fi para escrita
            file = SPIFFS.open("/wifiredes.txt", FILE_WRITE);
            if (!file) {
                request->send(500, "text/plain", "Erro ao abrir o arquivo para escrita"); // Responde com erro se não conseguir abrir o arquivo
                return;
            }
            file.print(newContent); // Escreve o conteúdo atualizado no arquivo
            file.close(); // Fecha o arquivo após escrita

            // Tenta conectar à nova rede salva
            connectToWiFi(ssid.c_str(), password.c_str());

            request->redirect("/wifigerenciamento"); // Redireciona para a página de gerenciamento de Wi-Fi
        } else {
            request->send(400, "text/plain", "Dados ausentes."); // Responde com erro se os dados estiverem ausentes
        }
    });

    // -------------------------------------------------------------------------
    // Rota para deletar uma rede Wi-Fi salva
    // -------------------------------------------------------------------------
    server.on("/excluirwifi", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (request->hasParam("ssid")) {
            // Obtém o SSID a ser excluído
            String ssidToDelete = request->getParam("ssid")->value();

            // Abre o arquivo de redes Wi-Fi para leitura
            File file = SPIFFS.open("/wifiredes.txt", FILE_READ);
            if (!file) {
                request->send(500, "text/plain", "Erro ao abrir o arquivo de redes Wi-Fi"); // Responde com erro se não conseguir abrir o arquivo
                return;
            }

            // Lê o conteúdo do arquivo
            String content = file.readString();
            file.close(); // Fecha o arquivo após leitura

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
                    newContent += line + "\n"; // Mantém a entrada que não deve ser excluída
                }
                start = end + 1;
            }

            // Abre o arquivo de redes Wi-Fi para escrita
            file = SPIFFS.open("/wifiredes.txt", FILE_WRITE);
            if (!file) {
                request->send(500, "text/plain", "Erro ao abrir o arquivo para escrita"); // Responde com erro se não conseguir abrir o arquivo
                return;
            }
            file.print(newContent); // Escreve o conteúdo atualizado no arquivo
            file.close(); // Fecha o arquivo após escrita

            request->redirect("/wifigerenciamento"); // Redireciona para a página de gerenciamento de Wi-Fi
        } else {
            request->send(400, "text/plain", "SSID ausente."); // Responde com erro se o SSID estiver ausente
        }
    });
}
