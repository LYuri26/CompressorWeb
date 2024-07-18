#include "ligadesliga.h"  // Inclui o cabeçalho para a funcionalidade de ligar/desligar

// Variável global para controlar o estado do compressor
bool compressorLigado = false;

void setupLigaDesliga(ESP8266WebServer& server) {
    // Define a rota "/toggle" para a requisição HTTP GET
    server.on("/toggle", HTTP_GET, [&server]() {
        // Lógica para ligar/desligar o compressor
        compressorLigado = !compressorLigado;  // Inverte o estado do compressor (liga/desliga)
        String message = compressorLigado ? "Compressor ligado!" : "Compressor desligado!";  // Define a mensagem de resposta com base no estado do compressor
        server.send(200, "text/plain", message);  // Envia a resposta HTTP 200 com a mensagem de estado do compressor
    });
}
