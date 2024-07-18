#include "ligadesliga.h"

// Variável global para controlar o estado do compressor
bool compressorLigado = false;

void setupLigaDesliga(ESP8266WebServer& server) {
    server.on("/toggle", HTTP_GET, [&server]() {
        // Lógica para ligar/desligar o compressor
        compressorLigado = !compressorLigado;
        String message = compressorLigado ? "Compressor ligado!" : "Compressor desligado!";
        server.send(200, "text/plain", message);
    });
}
