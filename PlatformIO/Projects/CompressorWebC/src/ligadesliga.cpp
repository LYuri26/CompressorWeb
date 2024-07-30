#include "ligadesliga.h"  // Inclui o cabeçalho para as funções relacionadas ao ligar/desligar o compressor
#include <NTPClient.h>    // Inclui a biblioteca NTPClient para obter a hora atual da internet
#include <WiFiUdp.h>      // Inclui a biblioteca WiFiUdp necessária para o NTPClient

// Variáveis globais
bool compressorLigado = false; // Flag global que indica se o compressor está ligado ou desligado
WiFiUDP ntpUDP;               // Objeto para gerenciar pacotes UDP necessários para NTPClient
NTPClient timeClient(ntpUDP, "pool.ntp.org", -3 * 3600, 60000); // Cria um cliente NTP para obter o tempo, ajuste o fuso horário para GMT-3 e atualiza a cada 60 segundos

unsigned long previousMillis = 0; // Armazena o tempo da última atualização para controle de intervalos
const long interval = 300000;     // Intervalo de 5 minutos (em milissegundos) para atualizar o status do compressor
unsigned long lastToggleTime = 0; // Tempo da última troca de estado do compressor para debounce
bool timerAtivo = false;          // Flag para verificar se o timer está ativo

// Função para imprimir a hora atual
void printCurrentTime()
{
    timeClient.update();                // Atualiza o tempo do NTPClient
    int hour = timeClient.getHours();  // Obtém a hora atual
    int minute = timeClient.getMinutes(); // Obtém os minutos atuais
    Serial.print("Hora atual: ");      // Imprime uma mensagem no console
    Serial.print(hour);                // Imprime a hora atual
    Serial.print(":");
    Serial.println(minute);            // Imprime os minutos atuais
}

// Função para verificar se a hora atual está após o horário de fechamento
bool isAfterClosingTime()
{
    printCurrentTime(); // Imprime a hora atual para depuração
    int hour = timeClient.getHours();  // Obtém a hora atual
    int minute = timeClient.getMinutes(); // Obtém os minutos atuais
    // Retorna verdadeiro se a hora atual for após as 22:30
    return (hour > 22) || (hour == 22 && minute >= 30);
}

// Função para verificar se a hora atual está antes do horário de abertura
bool isBeforeOpeningTime()
{
    printCurrentTime(); // Imprime a hora atual para depuração
    int hour = timeClient.getHours();  // Obtém a hora atual
    int minute = timeClient.getMinutes(); // Obtém os minutos atuais
    // Retorna verdadeiro se a hora atual for antes das 07:30
    return (hour < 7) || (hour == 7 && minute < 30);
}

// Função para imprimir o status atual do compressor
void printCompressorStatus()
{
    String message = compressorLigado ? "Compressor ligado!" : "Compressor desligado!"; // Mensagem dependendo do estado do compressor
    Serial.print("Estado do compressor: "); // Imprime uma mensagem no console
    Serial.println(message); // Imprime o status do compressor
}

// Função para atualizar o status do compressor
void updateCompressorStatus()
{
    unsigned long currentMillis = millis(); // Obtém o tempo atual em milissegundos desde o início do programa
    if (compressorLigado && (currentMillis - previousMillis >= interval))
    {
        previousMillis = currentMillis; // Atualiza o tempo da última atualização
        if (isAfterClosingTime() || isBeforeOpeningTime())
        {
            compressorLigado = false; // Desliga o compressor se estiver fora do horário permitido
            Serial.println("Compressor desligado devido ao horário."); // Imprime uma mensagem no console
        }
        else
        {
            compressorLigado = !compressorLigado; // Alterna o estado do compressor
            printCompressorStatus(); // Imprime o status atualizado do compressor
        }
    }
}

// Função para configurar o servidor Web para lidar com as solicitações de ligar/desligar o compressor
void setupLigaDesliga(WebServer &server)
{
    // Configura a rota "/toggle" para lidar com requisições GET
    server.on("/toggle", HTTP_GET, [&server]()
              {
        unsigned long currentMillis = millis(); // Obtém o tempo atual em milissegundos

        // Verifica se o compressor foi ligado/desligado recentemente e impede a troca até que o intervalo de 5 minutos tenha passado
        if (compressorLigado && (currentMillis - lastToggleTime < interval))
        {
            server.send(200, "text/plain", "Comando ignorado. Aguarde 5 minutos entre as tentativas."); // Responde com uma mensagem informando que o comando foi ignorado
            return; // Sai da função
        }

        lastToggleTime = currentMillis; // Atualiza o tempo da última troca de estado do compressor
        compressorLigado = !compressorLigado; // Alterna o estado do compressor

        String message = compressorLigado ? "Compressor ligado!" : "Compressor desligado!"; // Mensagem dependendo do novo estado do compressor
        if (compressorLigado && (isAfterClosingTime() || isBeforeOpeningTime()))
        {
            message += " Alerta! Já se passou das 22:30 e ainda não chegou às 07:30, atente-se para desligar o compressor após o uso."; // Adiciona um alerta se estiver fora do horário permitido
        }

        Serial.print("Estado do compressor: "); // Imprime uma mensagem no console
        Serial.println(message); // Imprime o status atualizado do compressor

        server.send(200, "text/plain", message); // Responde com o status do compressor

        if (compressorLigado)
        {
            previousMillis = millis(); // Atualiza o tempo da última atualização para controle do intervalo
            timerAtivo = true; // Marca o timer como ativo
        }
    });

    timeClient.begin(); // Inicia o cliente NTP para obter o tempo
    printCompressorStatus(); // Imprime o status inicial do compressor
    printCurrentTime(); // Imprime a hora atual

    // Verifica o horário e ajusta o estado inicial do compressor
    if (isAfterClosingTime() || isBeforeOpeningTime())
    {
        compressorLigado = false; // Desliga o compressor se estiver fora do horário permitido
        Serial.println("Compressor desligado devido ao horário na inicialização."); // Imprime uma mensagem no console
    }
    else
    {
        Serial.println("Compressor inicializado dentro do horário permitido."); // Imprime uma mensagem no console
    }
}
