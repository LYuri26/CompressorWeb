#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>
#include "index.h"
#include "dashboard.h"
#include "ligadesliga.h"
#include "creditos.h"
#include "umidade.h"
#include "oleo.h"
#include "wificonexao.h"
#include "serverSetup.h" // Inclua o cabeçalho com a declaração da função setupServer
#include "autenticador.h"
#include "paginaserro.h"

WebServer server(80);

void setup()
{
    Serial.begin(115200);
    Serial.println("Iniciando o setup...");

    connectToWiFi();
    setupServer();
}

void loop()
{
    server.handleClient();
    updateCompressorStatus();

    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("Conexão WiFi perdida. Tentando reconectar...");
        connectToWiFi();
    }
}
