#include "ligadesliga.h"
#include "tempo.h"
#include <FS.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include "autenticador.h" // Inclui o cabeçalho onde a variável userLoggedIn é declarada


// Variáveis globais
const int pinoCompressor = 2;
const long intervalo = 300000; // 5 minutos
const String arquivoEstado = "/estadocompressor.txt";

bool compressorLigado = false;
bool timerAtivo = false;
unsigned long previousMillis = 0;
unsigned long lastToggleTime = 0;

void handleToggleAction(AsyncWebServer& server) {
    server.on("/toggle", HTTP_ANY, [](AsyncWebServerRequest *request) {
        if (!isAuthenticated(request)) {
            redirectToAccessDenied(request); // Certifique-se de ter essa função definida em um arquivo de cabeçalho incluído
            return;
        }
        // Se estiver autenticado, continua com a lógica para ligar/desligar o compressor
        String action = request->getParam("action")->value();
        if (action == "ligar") {
            compressorLigado = true;
            request->send(200, "text/plain", "Compressor ligado!");
        } else if (action == "desligar") {
            compressorLigado = false;
            request->send(200, "text/plain", "Compressor desligado!");
        } else {
            request->send(400, "text/plain", "Ação inválida!");
        }
    });
}

void initSPIFFS() {
    if (!SPIFFS.begin(true)) {
        Serial.println("Erro ao iniciar SPIFFS. O sistema de arquivos não pôde ser montado.");
        return;
    }
    Serial.println("SPIFFS inicializado com sucesso.");
}

bool readCompressorState() {
    Serial.println("Tentando ler o estado do compressor do arquivo.");
    File file = SPIFFS.open(arquivoEstado, "r");
    if (!file) {
        Serial.println("Arquivo de estado não encontrado, assumindo estado desligado.");
        return false;
    }

    String state = file.readStringUntil('\n');
    file.close();

    bool estado = state.toInt() == 1;
    Serial.printf("Estado lido do arquivo: %s\n", estado ? "Ligado" : "Desligado");
    return estado;
}

void saveCompressorState(bool state) {
    Serial.printf("Salvando o estado do compressor: %s\n", state ? "Ligado" : "Desligado");
    File file = SPIFFS.open(arquivoEstado, "w");
    if (!file) {
        Serial.println("Erro ao abrir o arquivo para escrita.");
        return;
    }

    file.println(state ? "1" : "0");
    file.close();
    Serial.println("Estado do compressor salvo com sucesso.");
}

// Função auxiliar para obter a hora a partir do formato de data e hora
int getHoursFromTime(const String& time) {
    return time.substring(11, 13).toInt();
}

// Função auxiliar para obter os minutos a partir do formato de data e hora
int getMinutesFromTime(const String& time) {
    return time.substring(14, 16).toInt();
}

bool isAfterClosingTime() {
    updateTime(); // Atualiza o tempo antes de verificar
    String time = getInternTime();
    int hour = getHoursFromTime(time);
    bool resultado = hour >= 22;
    Serial.printf("Verificação de horário: %s\n", resultado ? "Após o horário de fechamento" : "Antes do horário de fechamento");
    return resultado;
}

bool isBeforeOpeningTime() {
    updateTime(); // Atualiza o tempo antes de verificar
    String time = getInternTime();
    int hour = getHoursFromTime(time);
    bool resultado = hour < 7;
    Serial.printf("Verificação de horário: %s\n", resultado ? "Antes do horário de abertura" : "Após o horário de abertura");
    return resultado;
}

void setupLigaDesliga(AsyncWebServer& server) {
    Serial.println("Configurando o servidor Web para controle do compressor.");
    initSPIFFS();

    pinMode(pinoCompressor, OUTPUT);
    compressorLigado = readCompressorState();
    digitalWrite(pinoCompressor, compressorLigado ? HIGH : LOW);

    server.on("/toggle", HTTP_GET, [](AsyncWebServerRequest *request) {
        unsigned long currentMillis = millis();
        Serial.println("Requisição recebida para alternar o estado do compressor.");

        if (compressorLigado && (currentMillis - lastToggleTime < 30000)) {
            Serial.println("Comando ignorado. O compressor foi alterado recentemente. Aguarde 5 minutos entre as tentativas.");
            request->send(200, "text/plain", "Comando ignorado. Aguarde 5 minutos entre as tentativas.");
            return;
        }

        lastToggleTime = currentMillis;
        compressorLigado = !compressorLigado;
        digitalWrite(pinoCompressor, compressorLigado ? HIGH : LOW);

        String message = compressorLigado ? "Compressor ligado!" : "Compressor desligado!";
        if (compressorLigado && (isAfterClosingTime() || isBeforeOpeningTime())) {
            message += " Alerta! Já se passou das 22:30 e ainda não chegou às 07:30, atente-se para desligar o compressor após o uso.";
            Serial.println("Alerta: Compressor ligado fora do horário permitido.");
        }

        Serial.print("Estado do compressor: ");
        Serial.println(message);
        request->send(200, "text/plain", message);

        saveCompressorState(compressorLigado);

        if (compressorLigado) {
            previousMillis = millis();
            timerAtivo = true;
            Serial.println("Compressor ligado. Timer ativado.");
        } else {
            Serial.println("Compressor desligado. Timer desativado.");
        }
    });

    setupTimeClient();

    Serial.print("Estado inicial do compressor: ");
    Serial.println(compressorLigado ? "Ligado" : "Desligado");

    updateTime();
    Serial.print("Hora atual: ");
    Serial.println(getInternTime()); // Hora em formato completo

    if (isAfterClosingTime() || isBeforeOpeningTime()) {
        compressorLigado = false;
        digitalWrite(pinoCompressor, LOW);
        saveCompressorState(compressorLigado);
        Serial.println("Compressor desligado devido ao horário na inicialização.");
    } else {
        Serial.println("Compressor inicializado dentro do horário permitido.");
    }
}

void updateCompressorStatus() {
    unsigned long currentMillis = millis();
    if (compressorLigado && (currentMillis - previousMillis >= intervalo)) {
        previousMillis = currentMillis;
        Serial.println("Compressor status atualizado.");
        saveCompressorState(compressorLigado);
    }
}

void shutdown() {
    saveCompressorState(compressorLigado);
    Serial.println("Dispositivo desligado. Estado do compressor salvo.");
}
