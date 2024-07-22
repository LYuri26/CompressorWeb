#include "ligadesliga.h" // Inclui o cabeçalho para a funcionalidade de ligar/desligar
#include <NTPClient.h>   // Inclui a biblioteca NTPClient para obter a hora
#include <WiFiUdp.h>     // Inclui a biblioteca WiFiUDP para comunicação UDP

// Variável global para controlar o estado do compressor
bool compressorLigado = false;

// Cria um objeto para comunicação UDP
WiFiUDP ntpUDP;
// Cria um objeto NTPClient para obter a hora
NTPClient timeClient(ntpUDP, "pool.ntp.org", -3 * 3600, 60000); // Ajuste o fuso horário conforme necessário

unsigned long previousMillis = 0; // Variável para armazenar o tempo anterior
const long interval = 300000;     // Intervalo de 5 minutos em milissegundos

// Função que verifica se é após o horário de fechamento
bool isAfterClosingTime()
{
    timeClient.update(); // Atualiza a hora
    int hour = timeClient.getHours();
    int minute = timeClient.getMinutes();

    // Debug: Exibe a hora atual no Serial Monitor
    Serial.print("Hora atual: ");
    Serial.print(hour);
    Serial.print(":");
    Serial.println(minute);

    // Verifica se é após 22:30
    return (hour > 22) || (hour == 22 && minute >= 30);
}

// Função que verifica se é antes do horário de abertura
bool isBeforeOpeningTime()
{
    timeClient.update(); // Atualiza a hora
    int hour = timeClient.getHours();
    int minute = timeClient.getMinutes();

    // Debug: Exibe a hora atual no Serial Monitor
    Serial.print("Hora atual: ");
    Serial.print(hour);
    Serial.print(":");
    Serial.println(minute);

    // Verifica se é antes de 07:30
    return (hour < 7) || (hour == 7 && minute < 30);
}

// Função que imprime o estado do compressor no Serial Monitor
void printCompressorStatus()
{
    String message = compressorLigado ? "Compressor ligado!" : "Compressor desligado!";
    Serial.print("Estado do compressor: ");
    Serial.println(message);
}

// Função que atualiza o estado do compressor a cada 5 minutos
void updateCompressorStatus()
{
    unsigned long currentMillis = millis(); // Obtém o tempo atual

    // Verifica se se passaram 5 minutos
    if (currentMillis - previousMillis >= interval)
    {
        previousMillis = currentMillis; // Atualiza o tempo anterior

        // Atualiza e exibe a hora atual
        timeClient.update();
        int hour = timeClient.getHours();
        int minute = timeClient.getMinutes();
        Serial.print("Hora atual: ");
        Serial.print(hour);
        Serial.print(":");
        Serial.println(minute);

        // Verifica se é após 22:30 ou antes de 07:30
        if (isAfterClosingTime() || isBeforeOpeningTime())
        {
            if (compressorLigado)
            {
                // Desliga o compressor se estiver ligado
                compressorLigado = false;
                Serial.println("Compressor desligado devido ao horário.");
            }
            else
            {
                // Se já estiver desligado, apenas envia a mensagem de horário
                Serial.println("Compressor já desligado devido ao horário.");
            }
        }
        else
        {
            // Lógica para ligar/desligar o compressor
            compressorLigado = !compressorLigado; // Inverte o estado do compressor (liga/desliga)
            String message = compressorLigado ? "Compressor ligado!" : "Compressor desligado!";
            Serial.print("Estado do compressor: ");
            Serial.println(message);
        }

        // Imprime o estado do compressor no Serial Monitor
        printCompressorStatus();
    }
}

void setupLigaDesliga(ESP8266WebServer &server)
{
    // Define a rota "/toggle" para a requisição HTTP GET
    server.on("/toggle", HTTP_GET, [&server]()
              {
        // Atualiza e exibe a hora atual
        timeClient.update();
        int hour = timeClient.getHours();
        int minute = timeClient.getMinutes();
        Serial.print("Hora atual: ");
        Serial.print(hour);
        Serial.print(":");
        Serial.println(minute);

        // Lógica para ligar/desligar o compressor
        compressorLigado = !compressorLigado;  // Inverte o estado do compressor (liga/desliga)

        // Mensagem de resposta
        String message = compressorLigado ? "Compressor ligado!" : "Compressor desligado!";
        
        // Verifica se foi ligado fora do horário permitido
        if (compressorLigado && (isAfterClosingTime() || isBeforeOpeningTime())) {
            message += " Alerta! Já se passou das 22:30 e ainda não chegou as 07:30, atente-se para desligar o compressor após o uso.";
        }

        // Exibe o estado do compressor no terminal Serial
        Serial.print("Estado do compressor: ");
        Serial.println(message);

        // Envia a resposta HTTP 200 com a mensagem de estado do compressor
        server.send(200, "text/plain", message); });

    // Inicializa o cliente NTP
    timeClient.begin();

    // Imprime o estado do compressor no Serial Monitor
    printCompressorStatus();

    // Atualiza e exibe a hora atual na inicialização
    timeClient.update();
    int hour = timeClient.getHours();
    int minute = timeClient.getMinutes();
    Serial.print("Hora atual na inicialização: ");
    Serial.print(hour);
    Serial.print(":");
    Serial.println(minute);

    // Garante que o compressor está desligado na inicialização se for fora do horário permitido
    if (isAfterClosingTime() || isBeforeOpeningTime())
    {
        compressorLigado = false;
        Serial.println("Compressor desligado devido ao horário na inicialização.");
    }
    else
    {
        Serial.println("Compressor inicializado dentro do horário permitido.");
    }
}
