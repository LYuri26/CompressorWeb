#include <WiFi.h>                   // Inclui a biblioteca WiFi para conectar-se a redes Wi-Fi e gerenciar a conexão
#include <ESPAsyncWebServer.h>      // Inclui a biblioteca para configurar um servidor web assíncrono no ESP32
#include <SPIFFS.h>                 // Inclui a biblioteca para acessar o sistema de arquivos SPIFFS (para armazenamento de arquivos no ESP32)
#include "wificonexao.h"            // Inclui o arquivo de cabeçalho com funções para conectar-se a redes Wi-Fi
#include "wifiinterface.h"          // Inclui o arquivo de cabeçalho para a interface e funções adicionais de gerenciamento de Wi-Fi

// -------------------------------------------------------------------------
// Função: setupWiFiGerenciadorPage
// Descrição: Configura as rotas e o comportamento da página de gerenciamento de Wi-Fi.
// -------------------------------------------------------------------------
void setupWiFiGerenciadorPage(AsyncWebServer &server) {
    // -------------------------------------------------------------------------
    // Inicializa o sistema de arquivos SPIFFS
    // -------------------------------------------------------------------------
    if (!SPIFFS.begin(true)) {          // Tenta iniciar o sistema de arquivos SPIFFS com formatação automática se necessário
        Serial.println("Falha ao iniciar o sistema de arquivos SPIFFS"); // Mensagem de erro no Serial Monitor se SPIFFS não conseguir iniciar
        return;                         // Interrompe a execução da função se SPIFFS não iniciar corretamente
    }

    // -------------------------------------------------------------------------
    // Rota para a página de gerenciamento de Wi-Fi
    // -------------------------------------------------------------------------
    server.on("/wifigerenciamento", HTTP_GET, [](AsyncWebServerRequest *request) {
        // Envia a página HTML de gerenciamento de Wi-Fi ao cliente
        request->send(200, "text/html", getWiFiManagementPage()); // Envia a resposta com status 200 (OK) e tipo de conteúdo "text/html"
    });

    // -------------------------------------------------------------------------
    // Rota para listar redes Wi-Fi salvas
    // -------------------------------------------------------------------------
    server.on("/listadewifi", HTTP_GET, [](AsyncWebServerRequest *request) {
        // Abre o arquivo "/wifiredes.txt" no sistema SPIFFS para leitura
        File file = SPIFFS.open("/wifiredes.txt", FILE_READ);
        if (!file) {                      // Verifica se o arquivo foi aberto com sucesso
            request->send(500, "text/plain", "Erro ao abrir o arquivo de redes Wi-Fi"); // Responde com erro se não conseguir abrir o arquivo
            return;                     // Interrompe a execução da função
        }

        // Lê o conteúdo do arquivo
        String networks = file.readString(); // Lê todo o conteúdo do arquivo como uma String
        file.close(); // Fecha o arquivo após leitura

        // Envia o conteúdo lido como resposta para o cliente
        request->send(200, "text/plain", networks); // Envia a resposta com status 200 (OK) e tipo de conteúdo "text/plain"
    });

    // -------------------------------------------------------------------------
    // Rota para salvar uma nova rede Wi-Fi
    // -------------------------------------------------------------------------
    server.on("/salvarwifi", HTTP_POST, [](AsyncWebServerRequest *request) {
        String ssid;       // Variável para armazenar o SSID da rede Wi-Fi
        String password;   // Variável para armazenar a senha da rede Wi-Fi

        if (request->hasParam("ssid", true) && request->hasParam("password", true)) { // Verifica se os parâmetros "ssid" e "password" estão presentes na requisição
            // Obtém o SSID e a senha do formulário enviado na requisição POST
            ssid = request->getParam("ssid", true)->value();
            password = request->getParam("password", true)->value();

            // Abre o arquivo "/wifiredes.txt" para leitura
            File file = SPIFFS.open("/wifiredes.txt", FILE_READ);
            if (!file) {                      // Verifica se o arquivo foi aberto com sucesso
                request->send(500, "text/plain", "Erro ao abrir o arquivo de redes Wi-Fi"); // Responde com erro se não conseguir abrir o arquivo
                return;                     // Interrompe a execução da função
            }

            // Lê o conteúdo do arquivo
            String content = file.readString(); // Lê todo o conteúdo do arquivo como uma String
            file.close(); // Fecha o arquivo após leitura

            String newContent = ""; // Variável para armazenar o novo conteúdo do arquivo
            bool ssidExists = false; // Flag para verificar se o SSID já existe no arquivo
            if (content.length() > 0) { // Verifica se o arquivo não está vazio
                // Processa o conteúdo existente para verificar se o SSID já está presente
                int start = 0;
                while (start < content.length()) {
                    int end = content.indexOf('\n', start); // Encontra a próxima nova linha
                    if (end == -1) {
                        end = content.length(); // Se não houver nova linha, define o final do conteúdo como o comprimento da String
                    }
                    String line = content.substring(start, end); // Extrai uma linha do conteúdo
                    int commaIndex = line.indexOf(','); // Encontra a posição da vírgula
                    String savedSSID = line.substring(0, commaIndex); // Extrai o SSID da linha
                    if (savedSSID == ssid) { // Verifica se o SSID extraído corresponde ao SSID enviado
                        newContent += ssid + "," + password + "\n"; // Atualiza a entrada existente com a nova senha
                        ssidExists = true; // Marca que o SSID foi encontrado
                    } else {
                        newContent += line + "\n"; // Mantém a entrada existente que não precisa ser atualizada
                    }
                    start = end + 1; // Move o início para a próxima linha
                }
            }

            if (!ssidExists) {
                newContent += ssid + "," + password + "\n"; // Adiciona uma nova entrada se o SSID não existir
            }

            // Abre o arquivo "/wifiredes.txt" para escrita
            file = SPIFFS.open("/wifiredes.txt", FILE_WRITE);
            if (!file) {                      // Verifica se o arquivo foi aberto com sucesso
                request->send(500, "text/plain", "Erro ao abrir o arquivo para escrita"); // Responde com erro se não conseguir abrir o arquivo
                return;                     // Interrompe a execução da função
            }
            file.print(newContent); // Escreve o conteúdo atualizado no arquivo
            file.close(); // Fecha o arquivo após escrita

            // Tenta conectar à nova rede Wi-Fi salva
            connectToWiFi(ssid.c_str(), password.c_str()); // Chama a função para conectar à nova rede Wi-Fi

            // Redireciona para a página de gerenciamento de Wi-Fi
            request->redirect("/wifigerenciamento"); // Redireciona o cliente para a página de gerenciamento de Wi-Fi
        } else {
            request->send(400, "text/plain", "Dados ausentes."); // Responde com erro se os dados do formulário estiverem ausentes
        }
    });

    // -------------------------------------------------------------------------
    // Rota para deletar uma rede Wi-Fi salva
    // -------------------------------------------------------------------------
    server.on("/excluirwifi", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (request->hasParam("ssid")) { // Verifica se o parâmetro "ssid" está presente na requisição
            // Obtém o SSID a ser excluído
            String ssidToDelete = request->getParam("ssid")->value();

            // Abre o arquivo "/wifiredes.txt" para leitura
            File file = SPIFFS.open("/wifiredes.txt", FILE_READ);
            if (!file) {                      // Verifica se o arquivo foi aberto com sucesso
                request->send(500, "text/plain", "Erro ao abrir o arquivo de redes Wi-Fi"); // Responde com erro se não conseguir abrir o arquivo
                return;                     // Interrompe a execução da função
            }

            // Lê o conteúdo do arquivo
            String content = file.readString(); // Lê todo o conteúdo do arquivo como uma String
            file.close(); // Fecha o arquivo após leitura

            String newContent = ""; // Variável para armazenar o novo conteúdo do arquivo
            int start = 0;
            while (start < content.length()) {
                int end = content.indexOf('\n', start); // Encontra a próxima nova linha
                if (end == -1) {
                    end = content.length(); // Se não houver nova linha, define o final do conteúdo como o comprimento da String
                }
                String line = content.substring(start, end); // Extrai uma linha do conteúdo
                int commaIndex = line.indexOf(','); // Encontra a posição da vírgula
                String savedSSID = line.substring(0, commaIndex); // Extrai o SSID da linha
                if (savedSSID != ssidToDelete) { // Verifica se o SSID extraído é diferente do SSID a ser excluído
                    newContent += line + "\n"; // Mantém a entrada que não deve ser excluída
                }
                start = end + 1; // Move o início para a próxima linha
            }

            // Abre o arquivo "/wifiredes.txt" para escrita
            file = SPIFFS.open("/wifiredes.txt", FILE_WRITE);
            if (!file) {                      // Verifica se o arquivo foi aberto com sucesso
                request->send(500, "text/plain", "Erro ao abrir o arquivo para escrita"); // Responde com erro se não conseguir abrir o arquivo
                return;                     // Interrompe a execução da função
            }
            file.print(newContent); // Escreve o conteúdo atualizado no arquivo
            file.close(); // Fecha o arquivo após escrita

            // Redireciona para a página de gerenciamento de Wi-Fi
            request->redirect("/wifigerenciamento"); // Redireciona o cliente para a página de gerenciamento de Wi-Fi
        } else {
            request->send(400, "text/plain", "SSID ausente."); // Responde com erro se o SSID estiver ausente na requisição
        }
    });

    // -------------------------------------------------------------------------
    // Rota para obter o IP da rede conectada
    // -------------------------------------------------------------------------
    server.on("/ipdarede", HTTP_GET, [](AsyncWebServerRequest *request) {
        String ip = WiFi.localIP().toString(); // Obtém o IP local da rede conectada e converte para String
        String jsonResponse = "{\"ip\":\"" + ip + "\"}"; // Formata a resposta como um objeto JSON com o IP
        request->send(200, "application/json", jsonResponse); // Envia a resposta com status 200 (OK) e tipo de conteúdo "application/json"
    });
}
