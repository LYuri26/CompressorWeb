#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <RTClib.h>
#include "wificonexao.h"  // Inclui o cabeçalho que contém a configuração e conexão WiFi


void handleRoot() {
    server.send(200, "text/html", "<html><body><h1>Sincronização Automática de Data e Hora</h1></body></html>");
}

void handleSync() {
    if (server.hasArg("time")) {
        String timeString = server.arg("time");
        DateTime dt = DateTime(timeString.c_str());
        rtc.adjust(dt);
        server.send(200, "text/plain", "Hora sincronizada");
    } else {
        server.send(400, "text/plain", "Argumento de tempo ausente");
    }
}