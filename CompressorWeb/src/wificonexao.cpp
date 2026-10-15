/**
 * ============================================================================
 * PROJETO: EnergizAR - Sistema de Automação e Monitoramento ESP32
 * ARQUIVO: src/wificonexao.cpp
 * DESCRIÇÃO: Conectividade Wi-Fi assíncrona, tolerante a falhas no SPIFFS.
 * ============================================================================
 */

#include "wificonexao.h"
#include <SPIFFS.h>
#include <esp_wifi.h>
#include <vector>

// -------------------------------------------------------------------------
// 1. CONFIGURAÇÃO DA SUA REDE WI-FI REAL
// -------------------------------------------------------------------------
// 👉 Coloque o SSID e Senha da sua rede local:
static const char *DEFAULT_WIFI_SSID = "NOME_DO_SEU_WIFI_AQUI";
static const char *DEFAULT_WIFI_PASS = "SENHA_DO_SEU_WIFI_AQUI";

static const char *ARQUIVO_WIFI = "/wifiredes.txt";

struct CredencialWiFi
{
    String ssid;
    String pass;
};

static std::vector<CredencialWiFi> filaTentativas;
static std::vector<CredencialWiFi> redesEmRAM;
static size_t indiceTentativaAtual = 0;
static bool spiffsMontado = false;
static bool spiffsTentado = false;

// Buffers
static char active_ssid[MAX_SSID_LEN + 1] = "";
static char active_password[MAX_PASS_LEN + 1] = "";

const char *ssid = active_ssid;
const char *password = active_password;

// Modo AP de Contingência
const char *ap_ssid = "EnergizAR";
const char *ap_password = "12345678";
const int ap_channel = 1;
const int ap_max_conn = 4;

IPAddress local_ip(192, 168, 26, 7);
IPAddress gateway(192, 168, 26, 1);
IPAddress subnet(255, 255, 255, 0);

// Variáveis de Estado
bool isAPMode = false;
bool wifiConectando = false;
bool scanning = false;
bool scanDone = false;
int scanResults = 0;

const unsigned long scanInterval = WIFI_SCAN_INTERVAL_MS;
unsigned long lastScan = 0;

static wifi_system_state_t currentState = WIFI_STATE_IDLE;
static unsigned long connectionStartTime = 0;
static uint8_t lastDisconnectReason = 0;

// -------------------------------------------------------------------------
// 2. Verificação Segura do Sistema de Arquivos (Anti-Crash)
// -------------------------------------------------------------------------
bool isSPIFFSDisponivel()
{
    if (spiffsMontado)
        return true;
    if (spiffsTentado)
        return false;

    spiffsTentado = true;
    if (SPIFFS.begin(false))
    {
        spiffsMontado = true;
        return true;
    }
    else if (SPIFFS.begin(true)) // Tenta formatar se necessário
    {
        spiffsMontado = true;
        return true;
    }

    Serial.println("[SPIFFS] Aviso: Partição SPIFFS indisponível. Operando via memória RAM transitória.");
    return false;
}

// -------------------------------------------------------------------------
// 3. Funções de Persistência (SPIFFS com Fallback em RAM)
// -------------------------------------------------------------------------
String obterListaRedesSalvas()
{
    String conteudo = "";
    if (isSPIFFSDisponivel() && SPIFFS.exists(ARQUIVO_WIFI))
    {
        File f = SPIFFS.open(ARQUIVO_WIFI, "r");
        if (f)
        {
            conteudo = f.readString();
            f.close();
            return conteudo;
        }
    }

    // Se SPIFFS não existe ou não tem arquivo, usa a RAM
    for (size_t i = 0; i < redesEmRAM.size(); i++)
    {
        conteudo += redesEmRAM[i].ssid + "," + redesEmRAM[i].pass + "\n";
    }
    return conteudo;
}

void registrarNovaRede(const String &novoSSID, const String &novaSenha)
{
    // 1. Atualiza em RAM
    bool achou = false;
    for (size_t i = 0; i < redesEmRAM.size(); i++)
    {
        if (redesEmRAM[i].ssid.equalsIgnoreCase(novoSSID))
        {
            redesEmRAM[i].pass = novaSenha;
            achou = true;
            break;
        }
    }
    if (!achou)
    {
        redesEmRAM.push_back({novoSSID, novaSenha});
    }

    // 2. Atualiza no SPIFFS se disponível
    if (isSPIFFSDisponivel())
    {
        String texto = "";
        bool gravado = false;

        if (SPIFFS.exists(ARQUIVO_WIFI))
        {
            File f = SPIFFS.open(ARQUIVO_WIFI, "r");
            if (f)
            {
                while (f.available())
                {
                    String linha = f.readStringUntil('\n');
                    linha.trim();
                    if (linha.length() == 0)
                        continue;

                    int sep = linha.indexOf(',');
                    if (sep != -1)
                    {
                        String s = linha.substring(0, sep);
                        s.trim();
                        if (s.equalsIgnoreCase(novoSSID))
                        {
                            texto += novoSSID + "," + novaSenha + "\n";
                            gravado = true;
                        }
                        else
                        {
                            texto += linha + "\n";
                        }
                    }
                }
                f.close();
            }
        }

        if (!gravado)
        {
            texto += novoSSID + "," + novaSenha + "\n";
        }

        File out = SPIFFS.open(ARQUIVO_WIFI, "w");
        if (out)
        {
            out.print(texto);
            out.close();
        }
    }
}

void removerRedeSalva(const String &ssidParaRemover)
{
    // 1. Remove da RAM
    for (auto it = redesEmRAM.begin(); it != redesEmRAM.end();)
    {
        if (it->ssid.equalsIgnoreCase(ssidParaRemover))
            it = redesEmRAM.erase(it);
        else
            ++it;
    }

    // 2. Remove do SPIFFS
    if (isSPIFFSDisponivel() && SPIFFS.exists(ARQUIVO_WIFI))
    {
        File f = SPIFFS.open(ARQUIVO_WIFI, "r");
        String texto = "";
        if (f)
        {
            while (f.available())
            {
                String linha = f.readStringUntil('\n');
                linha.trim();
                if (linha.length() == 0)
                    continue;

                int sep = linha.indexOf(',');
                if (sep != -1)
                {
                    String s = linha.substring(0, sep);
                    s.trim();
                    if (!s.equalsIgnoreCase(ssidParaRemover))
                    {
                        texto += linha + "\n";
                    }
                }
            }
            f.close();
        }

        File out = SPIFFS.open(ARQUIVO_WIFI, "w");
        if (out)
        {
            out.print(texto);
            out.close();
        }
    }
}

// -------------------------------------------------------------------------
// 4. Utilitários e Diagnósticos
// -------------------------------------------------------------------------
static const char *wifiDisconnectReasonToString(uint8_t reason)
{
    switch (reason)
    {
    case WIFI_REASON_AUTH_EXPIRE:
        return "Autenticação expirada";
    case WIFI_REASON_AUTH_LEAVE:
        return "Desconectado pelo cliente";
    case WIFI_REASON_ASSOC_TOOMANY:
        return "Roteador lotado";
    case WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT:
        return "Senha incorreta";
    case WIFI_REASON_BEACON_TIMEOUT:
        return "Sinal fraco";
    case WIFI_REASON_NO_AP_FOUND:
        return "Roteador não localizado";
    case WIFI_REASON_AUTH_FAIL:
        return "Falha de autenticação WPA";
    default:
        return "Transição de rede normal";
    }
}

int getWiFiSignalQuality()
{
    if (WiFi.status() != WL_CONNECTED)
        return 0;
    int rssi = WiFi.RSSI();
    if (rssi <= -100)
        return 0;
    if (rssi >= -50)
        return 100;
    return 2 * (rssi + 100);
}

bool isWiFiConnected()
{
    return (WiFi.status() == WL_CONNECTED && !isAPMode);
}

const char *getWiFiStateString()
{
    switch (currentState)
    {
    case WIFI_STATE_CONNECTING:
        return "Conectando ao Roteador...";
    case WIFI_STATE_CONNECTED:
        return "Conectado e Operacional";
    case WIFI_STATE_DISCONNECTED:
        return "Desconectado";
    case WIFI_STATE_FAILED:
        return "Falha na Conexão";
    case WIFI_STATE_AP_MODE:
        return "Modo Ponto de Acesso (AP)";
    default:
        return "Pronto";
    }
}

// -------------------------------------------------------------------------
// 5. Manipulador Central de Eventos
// -------------------------------------------------------------------------
static void onWiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info)
{
    switch (event)
    {
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
        currentState = WIFI_STATE_CONNECTED;
        wifiConectando = false;
        isAPMode = false;

        Serial.println("\n==================================================");
        Serial.println("  CONEXÃO WI-FI ESTABELECIDA COM SUCESSO!        ");
        Serial.println("==================================================");
        Serial.printf(">>> ACESSE: http://%s <<<\n", WiFi.localIP().toString().c_str());
        Serial.printf("Sinal RSSI: %d dBm (%d%%)\n", WiFi.RSSI(), getWiFiSignalQuality());
        Serial.println("==================================================");
        filaTentativas.clear();
        break;

    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
        lastDisconnectReason = info.wifi_sta_disconnected.reason;
        if (currentState == WIFI_STATE_CONNECTED)
        {
            Serial.printf("[WIFI] Desconectado. Motivo: %s (Cód: %d)\n",
                          wifiDisconnectReasonToString(lastDisconnectReason), lastDisconnectReason);
            currentState = WIFI_STATE_DISCONNECTED;
        }
        break;

    case ARDUINO_EVENT_WIFI_AP_START:
        isAPMode = true;
        currentState = WIFI_STATE_AP_MODE;
        Serial.printf("[WIFI] Modo AP Ativo: %s | Acesse via: http://%s\n",
                      ap_ssid, WiFi.softAPIP().toString().c_str());
        break;

    default:
        break;
    }
}

// -------------------------------------------------------------------------
// 6. Modo AP e Conexão Não-Bloqueante
// -------------------------------------------------------------------------
void setupAP()
{
    static bool eventosRegistrados = false;
    if (!eventosRegistrados)
    {
        WiFi.onEvent(onWiFiEvent);
        eventosRegistrados = true;
    }

    WiFi.mode(WIFI_AP_STA);
    WiFi.softAPConfig(local_ip, gateway, subnet);
    WiFi.softAP(ap_ssid, ap_password, ap_channel, 0, ap_max_conn);
    isAPMode = true;
    currentState = WIFI_STATE_AP_MODE;
    wifiConectando = false;
}

void enterAPMode()
{
    setupAP();
}

void connectToWiFi(const char *target_ssid, const char *target_password)
{
    if (!target_ssid || strlen(target_ssid) == 0)
        return;

    static bool eventosRegistrados = false;
    if (!eventosRegistrados)
    {
        WiFi.onEvent(onWiFiEvent);
        eventosRegistrados = true;
    }

    memset(active_ssid, 0, sizeof(active_ssid));
    memset(active_password, 0, sizeof(active_password));

    strncpy(active_ssid, target_ssid, MAX_SSID_LEN);
    if (target_password)
    {
        strncpy(active_password, target_password, MAX_PASS_LEN);
    }

    Serial.printf("[WIFI] Conectando de forma assíncrona a: '%s'...\n", active_ssid);

    WiFi.mode(WIFI_AP_STA);
    WiFi.begin(active_ssid, active_password);

    wifiConectando = true;
    currentState = WIFI_STATE_CONNECTING;
    connectionStartTime = millis();
}

void desconectarWiFi()
{
    WiFi.disconnect(false, false);
    currentState = WIFI_STATE_DISCONNECTED;
    wifiConectando = false;
}

void processarConexaoWiFi()
{
    unsigned long agora = millis();

    if (wifiConectando && currentState == WIFI_STATE_CONNECTING)
    {
        if (agora - connectionStartTime >= WIFI_RECONNECT_TIMEOUT_MS)
        {
            Serial.printf("[WIFI] Tempo esgotado para '%s'.\n", active_ssid);
            wifiConectando = false;
            currentState = WIFI_STATE_FAILED;

            indiceTentativaAtual++;
            if (indiceTentativaAtual < filaTentativas.size())
            {
                Serial.printf("[WIFI] Tentando próxima rede prioritária: '%s'\n",
                              filaTentativas[indiceTentativaAtual].ssid.c_str());
                connectToWiFi(filaTentativas[indiceTentativaAtual].ssid.c_str(),
                              filaTentativas[indiceTentativaAtual].pass.c_str());
            }
            else
            {
                Serial.println("[WIFI] Todas as tentativas falharam. Ativando Modo AP.");
                setupAP();
            }
        }
    }
}

// -------------------------------------------------------------------------
// 7. Varredura de Redes
// -------------------------------------------------------------------------
void startScanWiFiNetworks()
{
    unsigned long agora = millis();
    if (!scanning && (agora - lastScan >= scanInterval))
    {
        scanning = true;
        scanDone = false;
        lastScan = agora;
        WiFi.scanNetworks(true, true);
    }
}

void checkScanResults()
{
    int status = WiFi.scanComplete();
    if (status >= 0)
    {
        scanResults = status;
        scanning = false;
        scanDone = true;
    }
    else if (status == WIFI_SCAN_FAILED)
    {
        scanning = false;
        scanDone = false;
    }
}

String getScanResults()
{
    int n = WiFi.scanComplete();
    if (n < 0)
        return "[]";

    String json = "[";
    for (int i = 0; i < n; i++)
    {
        json += "{\"ssid\":\"" + WiFi.SSID(i) + "\",\"rssi\":" + String(WiFi.RSSI(i)) + "}";
        if (i < n - 1)
            json += ",";
    }
    json += "]";

    WiFi.scanDelete();
    scanning = false;
    return json;
}

// -------------------------------------------------------------------------
// 8. Fila de Conexão Inteligente
// -------------------------------------------------------------------------
void loadSavedWiFiNetworks()
{
    filaTentativas.clear();
    indiceTentativaAtual = 0;

    // 1. Redes salvas pelo usuário
    String salvasStr = obterListaRedesSalvas();
    if (salvasStr.length() > 0)
    {
        std::vector<CredencialWiFi> salvas;
        int inicio = 0;
        while (inicio < salvasStr.length())
        {
            int fim = salvasStr.indexOf('\n', inicio);
            if (fim == -1)
                fim = salvasStr.length();

            String linha = salvasStr.substring(inicio, fim);
            linha.trim();
            int sep = linha.indexOf(',');
            if (sep != -1)
            {
                CredencialWiFi c;
                c.ssid = linha.substring(0, sep);
                c.pass = linha.substring(sep + 1);
                c.ssid.trim();
                c.pass.trim();
                if (c.ssid.length() > 0)
                    salvas.push_back(c);
            }
            inicio = fim + 1;
        }

        for (int i = salvas.size() - 1; i >= 0; i--)
        {
            filaTentativas.push_back(salvas[i]);
        }
    }

    // 2. Wi-Fi Real padrão configurado no código
    if (strlen(DEFAULT_WIFI_SSID) > 0 && strcmp(DEFAULT_WIFI_SSID, "NOME_DO_SEU_WIFI_AQUI") != 0)
    {
        filaTentativas.push_back({DEFAULT_WIFI_SSID, DEFAULT_WIFI_PASS});
    }

    // 3. Fallback para simulação no Wokwi
    filaTentativas.push_back({"Wokwi-GUEST", ""});

    if (!filaTentativas.empty())
    {
        Serial.printf("[WIFI] Conectando à primeira rede: '%s'\n", filaTentativas[0].ssid.c_str());
        connectToWiFi(filaTentativas[0].ssid.c_str(), filaTentativas[0].pass.c_str());
    }
    else
    {
        setupAP();
    }
}

String getWiFiStatusJSON()
{
    String json = "{";
    json += "\"connected\":" + String(isWiFiConnected() ? "true" : "false") + ",";
    json += "\"apMode\":" + String(isAPMode ? "true" : "false") + ",";
    json += "\"state\":\"" + String(getWiFiStateString()) + "\",";
    json += "\"ssid\":\"" + String(WiFi.status() == WL_CONNECTED ? WiFi.SSID() : "") + "\",";
    json += "\"ip\":\"" + (WiFi.status() == WL_CONNECTED ? WiFi.localIP().toString() : WiFi.softAPIP().toString()) + "\",";
    json += "\"rssi\":" + String(WiFi.status() == WL_CONNECTED ? WiFi.RSSI() : 0) + ",";
    json += "\"quality\":" + String(getWiFiSignalQuality()) + ",";
    json += "\"mac\":\"" + WiFi.macAddress() + "\"";
    json += "}";
    return json;
}