#include <WiFi.h>              // Inclui a biblioteca WiFi, essencial para conectar-se e gerenciar conexões Wi-Fi no ESP32.
#include <ESPAsyncWebServer.h> // Inclui a biblioteca para configurar um servidor web assíncrono no ESP32, permitindo servir páginas web e lidar com requisições HTTP.
#include <SPIFFS.h>            // Inclui a biblioteca para acessar o sistema de arquivos SPIFFS, útil para armazenar e ler arquivos no ESP32.
#include "wificonexao.h"       // Inclui um arquivo de cabeçalho personalizado com funções para conectar-se a redes Wi-Fi.
#include "wifiinterface.h"     // Inclui um arquivo de cabeçalho personalizado com funções adicionais de gerenciamento de Wi-Fi e interface.

// -------------------------------------------------------------------------
// Função: setupWiFiGerenciadorPage
// Descrição: Configura as rotas e o comportamento da página de gerenciamento de Wi-Fi.
// Parâmetros:
//  - AsyncWebServer &server: Referência para o servidor web assíncrono que servirá as páginas e rotas definidas.
// -------------------------------------------------------------------------
void setupWiFiGerenciadorPage(AsyncWebServer &server)
{
    // -------------------------------------------------------------------------
    // Inicializa o sistema de arquivos SPIFFS.
    // -------------------------------------------------------------------------
    if (!SPIFFS.begin(true))
    {                                                                    // Tenta iniciar o sistema de arquivos SPIFFS com formatação automática caso necessário.
        Serial.println("Falha ao iniciar o sistema de arquivos SPIFFS"); // Mensagem de erro se SPIFFS não conseguir iniciar, exibida no monitor serial.
        return;                                                          // Interrompe a execução da função se SPIFFS não iniciar corretamente.
    }

    // -------------------------------------------------------------------------
    // Rota para a página de gerenciamento de Wi-Fi.
    // -------------------------------------------------------------------------
    server.on("/wifigerenciamento", HTTP_GET, [](AsyncWebServerRequest *request)
              {
                  request->send(200, "text/html", getWiFiManagementPage()); // Envia ao cliente a página HTML de gerenciamento de Wi-Fi com status 200 (OK).
              });

    // -------------------------------------------------------------------------
    // Rota para listar redes Wi-Fi salvas.
    // -------------------------------------------------------------------------
    server.on("/listadewifi", HTTP_GET, [](AsyncWebServerRequest *request)
              {
                  File file = SPIFFS.open("/wifiredes.txt", FILE_READ);
                  if (!file)
                  {                                                                               // Verifica se o arquivo foi aberto com sucesso.
                      request->send(500, "text/plain", "Erro ao abrir o arquivo de redes Wi-Fi"); // Responde com erro (500) se não conseguir abrir o arquivo.
                      return;                                                                     // Interrompe a execução da função em caso de erro.
                  }

                  String networks = file.readString(); // Lê todo o conteúdo do arquivo como uma String.
                  file.close();                        // Fecha o arquivo após a leitura para liberar recursos.

                  request->send(200, "text/plain", networks); // Envia o conteúdo lido como resposta ao cliente com status 200 (OK).
              });

    // -------------------------------------------------------------------------
    // Rota para salvar uma nova rede Wi-Fi.
    // -------------------------------------------------------------------------
    server.on("/salvarwifi", HTTP_POST, [](AsyncWebServerRequest *request)
              {
        if (request->hasParam("ssid", true) && request->hasParam("password", true)) 
        {   // Verifica se os parâmetros "ssid" e "password" estão presentes e são enviados via POST.
            String ssid = request->getParam("ssid", true)->value();       // Obtém o SSID enviado pelo formulário.
            String password = request->getParam("password", true)->value(); // Obtém a senha enviada pelo formulário.

            File file = SPIFFS.open("/wifiredes.txt", FILE_READ);
            if (!file) 
            {   // Verifica se o arquivo foi aberto com sucesso para leitura.
                request->send(500, "text/plain", "Erro ao abrir o arquivo de redes Wi-Fi"); // Responde com erro (500) se não conseguir abrir o arquivo.
                return; // Interrompe a execução da função em caso de erro.
            }

            String content = file.readString(); // Lê o conteúdo do arquivo como uma String.
            file.close(); // Fecha o arquivo após leitura.

            String newContent; // String que armazenará o novo conteúdo atualizado do arquivo.
            bool ssidExists = false; // Flag para verificar se o SSID já existe no arquivo.
            int start = 0;
            while (start < content.length()) 
            {   // Percorre o conteúdo do arquivo linha por linha.
                int end = content.indexOf('\n', start);
                if (end == -1) 
                {   // Se não houver mais quebras de linha, define o fim da leitura como o final do conteúdo.
                    end = content.length();
                }
                String line = content.substring(start, end); // Extrai a linha atual.
                int commaIndex = line.indexOf(','); // Localiza a vírgula que separa o SSID da senha.
                String savedSSID = line.substring(0, commaIndex); // Obtém o SSID salvo.
                if (savedSSID == ssid) 
                {   // Se o SSID já existe, atualiza a senha correspondente.
                    newContent += ssid + "," + password + "\n";
                    ssidExists = true; // Marca que o SSID foi encontrado.
                } 
                else 
                {   // Se o SSID não corresponde, mantém a linha original.
                    newContent += line + "\n";
                }
                start = end + 1; // Move o ponto de início para a próxima linha.
            }

            if (!ssidExists) 
            {   // Se o SSID não foi encontrado, adiciona uma nova entrada ao conteúdo.
                newContent += ssid + "," + password + "\n";
            }

            file = SPIFFS.open("/wifiredes.txt", FILE_WRITE);
            if (!file) 
            {   // Verifica se o arquivo foi aberto com sucesso para escrita.
                request->send(500, "text/plain", "Erro ao abrir o arquivo para escrita"); // Responde com erro (500) se não conseguir abrir o arquivo para escrita.
                return; // Interrompe a execução da função em caso de erro.
            }
            file.print(newContent); // Escreve o conteúdo atualizado no arquivo.
            file.close(); // Fecha o arquivo após a escrita.

            connectToWiFi(ssid.c_str(), password.c_str()); // Tenta conectar à nova rede Wi-Fi salva usando o SSID e a senha.

            request->redirect("/wifigerenciamento"); // Redireciona o cliente para a página de gerenciamento de Wi-Fi após salvar.
        } 
        else 
        {   // Se os parâmetros SSID ou senha estiverem ausentes.
            request->send(400, "text/plain", "Dados ausentes."); // Responde com erro (400) indicando que os dados do formulário estão incompletos.
        } });

    // -------------------------------------------------------------------------
    // Rota para deletar uma rede Wi-Fi salva.
    // -------------------------------------------------------------------------
    server.on("/excluirwifi", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        if (request->hasParam("ssid")) 
        {   // Verifica se o parâmetro "ssid" está presente na requisição.
            String ssidToDelete = request->getParam("ssid")->value(); // Obtém o SSID que deve ser deletado.

            File file = SPIFFS.open("/wifiredes.txt", FILE_READ);
            if (!file) 
            {   // Verifica se o arquivo foi aberto com sucesso para leitura.
                request->send(500, "text/plain", "Erro ao abrir o arquivo de redes Wi-Fi"); // Responde com erro (500) se não conseguir abrir o arquivo.
                return; // Interrompe a execução da função em caso de erro.
            }

            String content = file.readString(); // Lê o conteúdo do arquivo como uma String.
            file.close(); // Fecha o arquivo após leitura.

            String newContent; // String para armazenar o novo conteúdo do arquivo após a remoção da rede.
            int start = 0;
            while (start < content.length()) 
            {   // Percorre o conteúdo do arquivo linha por linha.
                int end = content.indexOf('\n', start);
                if (end == -1) 
                {   // Se não houver mais quebras de linha, define o fim da leitura como o final do conteúdo.
                    end = content.length();
                }
                String line = content.substring(start, end); // Extrai a linha atual.
                int commaIndex = line.indexOf(','); // Localiza a vírgula que separa o SSID da senha.
                String savedSSID = line.substring(0, commaIndex); // Obtém o SSID salvo.
                if (savedSSID != ssidToDelete) 
                {   // Se o SSID não corresponde ao que deve ser deletado, mantém a linha.
                    newContent += line + "\n";
                }
                start = end + 1; // Move o ponto de início para a próxima linha.
            }

            file = SPIFFS.open("/wifiredes.txt", FILE_WRITE);
            if (!file) 
            {   // Verifica se o arquivo foi aberto com sucesso para escrita.
                request->send(500, "text/plain", "Erro ao abrir o arquivo para escrita"); // Responde com erro (500) se não conseguir abrir o arquivo para escrita.
                return; // Interrompe a execução da função em caso de erro.
            }
            file.print(newContent); // Escreve o novo conteúdo (sem o SSID deletado) no arquivo.
            file.close(); // Fecha o arquivo após a escrita.

            request->redirect("/wifigerenciamento"); // Redireciona o cliente para a página de gerenciamento de Wi-Fi após deletar a rede.
        } 
        else 
        {   // Se o parâmetro SSID estiver ausente.
            request->send(400, "text/plain", "SSID ausente."); // Responde com erro (400) indicando que o SSID está ausente na requisição.
        } });

    // -------------------------------------------------------------------------
    // Rota para obter o IP da rede conectada.
    // -------------------------------------------------------------------------
    server.on("/ipdarede", HTTP_GET, [](AsyncWebServerRequest *request)
              {
                  String ip = WiFi.localIP().toString();                // Obtém o IP local da rede conectada e o converte para uma String.
                  String jsonResponse = "{\"ip\":\"" + ip + "\"}";      // Formata a resposta como um objeto JSON contendo o IP.
                  request->send(200, "application/json", jsonResponse); // Envia a resposta JSON com status 200 (OK) e tipo de conteúdo "application/json".
              });
}
