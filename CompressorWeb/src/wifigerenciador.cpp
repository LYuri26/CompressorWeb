/**
 * ============================================================================
 * PROJETO: EnergizAR - Sistema de Automação e Monitoramento ESP32
 * ARQUIVO: src/wifigerenciador.cpp
 * DESCRIÇÃO: Controlador de endpoints REST para gerenciamento de Wi-Fi.
 * ============================================================================
 */

#include "wifigerenciador.h"
#include "wificonexao.h"
#include "wifiinterface.h"
#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <vector>

static const char *ARQUIVO_WIFI_PATH = "/wifiredes.txt";

// Estrutura interna para contingência em memória RAM (caso o SPIFFS falhe na Flash)
struct RedeCredencialMemoria
{
    String ssid;
    String pass;
};
static std::vector<RedeCredencialMemoria> redesEmRAM;

// -------------------------------------------------------------------------
// Verificação segura do SPIFFS local (Anti-Crash / Sem erros de escopo)
// -------------------------------------------------------------------------
static bool isSPIFFSDisponivel()
{
    static bool montado = false;
    static bool tentouMontar = false;

    if (montado)
        return true;
    if (tentouMontar)
        return false;

    tentouMontar = true;
    if (SPIFFS.begin(false) || SPIFFS.begin(true))
    {
        montado = true;
        return true;
    }

    Serial.println("[SPIFFS] Aviso: Sistema de arquivos indisponível. Operando em memória RAM temporária.");
    return false;
}

// Utilitário para limpar quebras de linha e espaços
static void sanitizarString(String &str)
{
    str.trim();
    str.replace("\r", "");
    str.replace("\n", "");
}

// -------------------------------------------------------------------------
// Configuração das Rotas Web do Gerenciador de Wi-Fi
// -------------------------------------------------------------------------
void setupWiFiGerenciadorPage(AsyncWebServer &server)
{
    // Inicializa a checagem do SPIFFS sem abortar as rotas caso falhe
    isSPIFFSDisponivel();

    // 1. Rota da Página HTML Principal de Configuração Wi-Fi
    server.on("/wifigerenciamento", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, "text/html", getWiFiManagementPage()); });

    // 2. Rota que lista redes salvas em texto simples (SSID,Senha)
    server.on("/listadewifi", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        String networks = "";

        if (isSPIFFSDisponivel() && SPIFFS.exists(ARQUIVO_WIFI_PATH)) {
            File file = SPIFFS.open(ARQUIVO_WIFI_PATH, "r");
            if (file) {
                networks = file.readString();
                file.close();
            }
        } else {
            // Contingência: lê da RAM temporária
            for (size_t i = 0; i < redesEmRAM.size(); i++) {
                networks += redesEmRAM[i].ssid + "," + redesEmRAM[i].pass + "\n";
            }
        }

        request->send(200, "text/plain", networks); });

    // 3. Rota de Varredura de Redes Wi-Fi Disponíveis no Ar (JSON)
    server.on("/scanwifi", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        int n = WiFi.scanComplete();
        if (n == -2) {
            WiFi.scanNetworks(true, true);
            request->send(200, "application/json", "{\"status\":\"scanning\"}");
        } else if (n == -1) {
            request->send(200, "application/json", "{\"status\":\"scanning\"}");
        } else {
            String json = "[";
            for (int i = 0; i < n; i++) {
                json += "{\"ssid\":\"" + WiFi.SSID(i) + "\",\"rssi\":" + String(WiFi.RSSI(i)) + "}";
                if (i < n - 1) json += ",";
            }
            json += "]";
            WiFi.scanDelete();
            request->send(200, "application/json", json);
        } });

    // 4. Rota para Salvar e Conectar a uma Nova Rede (POST)
    server.on("/salvarwifi", HTTP_POST, [](AsyncWebServerRequest *request)
              {
        if (!request->hasParam("ssid", true) || !request->hasParam("password", true)) {
            request->send(400, "text/plain", "Dados ausentes.");
            return;
        }

        String ssidNovo = request->getParam("ssid", true)->value();
        String passNova = request->getParam("password", true)->value();
        sanitizarString(ssidNovo);
        sanitizarString(passNova);

        if (ssidNovo.length() == 0) {
            request->send(400, "text/plain", "SSID não pode ser vazio.");
            return;
        }

        // Atualiza na memória RAM
        bool jaExisteNaRAM = false;
        for (size_t i = 0; i < redesEmRAM.size(); i++) {
            if (redesEmRAM[i].ssid.equalsIgnoreCase(ssidNovo)) {
                redesEmRAM[i].pass = passNova;
                jaExisteNaRAM = true;
                break;
            }
        }
        if (!jaExisteNaRAM) {
            redesEmRAM.push_back({ssidNovo, passNova});
        }

        // Salva persistentemente no SPIFFS caso disponível
        if (isSPIFFSDisponivel()) {
            String novoConteudo = "";
            bool jaExiste = false;

            if (SPIFFS.exists(ARQUIVO_WIFI_PATH)) {
                File readFile = SPIFFS.open(ARQUIVO_WIFI_PATH, "r");
                if (readFile) {
                    while (readFile.available()) {
                        String linha = readFile.readStringUntil('\n');
                        linha.trim();
                        if (linha.length() == 0) continue;

                        int sep = linha.indexOf(',');
                        if (sep != -1) {
                            String s = linha.substring(0, sep);
                            s.trim();
                            if (s.equalsIgnoreCase(ssidNovo)) {
                                novoConteudo += ssidNovo + "," + passNova + "\n";
                                jaExiste = true;
                            } else {
                                novoConteudo += linha + "\n";
                            }
                        }
                    }
                    readFile.close();
                }
            }

            if (!jaExiste) {
                novoConteudo += ssidNovo + "," + passNova + "\n";
            }

            File writeFile = SPIFFS.open(ARQUIVO_WIFI_PATH, "w");
            if (writeFile) {
                writeFile.print(novoConteudo);
                writeFile.close();
            }
        }

        // Conecta assincronamente sem travar a CPU nem estourar o Watchdog
        connectToWiFi(ssidNovo.c_str(), passNova.c_str());

        request->send(200, "application/json", "{\"status\":\"ok\",\"message\":\"Conectando a rede...\"}"); });

    // 5. Rota para Excluir Rede Salva
    server.on("/excluirwifi", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        if (!request->hasParam("ssid")) {
            request->send(400, "text/plain", "SSID ausente.");
            return;
        }

        String ssidDel = request->getParam("ssid")->value();
        sanitizarString(ssidDel);

        // Remove da RAM
        for (auto it = redesEmRAM.begin(); it != redesEmRAM.end();) {
            if (it->ssid.equalsIgnoreCase(ssidDel)) {
                it = redesEmRAM.erase(it);
            } else {
                ++it;
            }
        }

        // Remove do arquivo no SPIFFS
        if (isSPIFFSDisponivel() && SPIFFS.exists(ARQUIVO_WIFI_PATH)) {
            File readFile = SPIFFS.open(ARQUIVO_WIFI_PATH, "r");
            String novoConteudo = "";
            if (readFile) {
                while (readFile.available()) {
                    String linha = readFile.readStringUntil('\n');
                    linha.trim();
                    if (linha.length() == 0) continue;

                    int sep = linha.indexOf(',');
                    if (sep != -1) {
                        String s = linha.substring(0, sep);
                        s.trim();
                        if (!s.equalsIgnoreCase(ssidDel)) {
                            novoConteudo += linha + "\n";
                        }
                    }
                }
                readFile.close();
            }

            File writeFile = SPIFFS.open(ARQUIVO_WIFI_PATH, "w");
            if (writeFile) {
                writeFile.print(novoConteudo);
                writeFile.close();
            }
        }

        request->redirect("/wifigerenciamento"); });

    // 6. Rota que retorna o Status e IP atual da Rede
    server.on("/ipdarede", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, "application/json", getWiFiStatusJSON()); });
}