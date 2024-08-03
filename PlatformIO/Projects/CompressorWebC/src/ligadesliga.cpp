#include "ligadesliga.h" // Biblioteca para funções de ligar e desligar o compressor
#include <NTPClient.h>   // Biblioteca para obter o tempo da Internet usando o protocolo NTP
#include <WiFiUdp.h>     // Biblioteca para comunicação UDP necessária para NTPClient
#include <FS.h>          // Biblioteca para manipulação do sistema de arquivos
#include <SPIFFS.h>      // Biblioteca para usar o sistema de arquivos SPIFFS

// Variáveis globais
const int pinoCompressor = 2;                         // Pino conectado ao compressor
const long intervalo = 300000;                        // Intervalo de 5 minutos (em milissegundos) para atualizar o status do compressor
const String arquivoEstado = "/estadocompressor.txt"; // Nome do arquivo para armazenar o estado do compressor

bool compressorLigado = false;                                  // Flag global que indica se o compressor está ligado ou desligado
bool timerAtivo = false;                                        // Flag para verificar se o timer está ativo
unsigned long previousMillis = 0;                               // Armazena o tempo da última atualização para controle de intervalos
unsigned long lastToggleTime = 0;                               // Tempo da última troca de estado do compressor para debounce
WiFiUDP ntpUDP;                                                 // Objeto para gerenciar pacotes UDP necessários para o cliente NTP
NTPClient timeClient(ntpUDP, "pool.ntp.org", -3 * 3600, 60000); // Cliente NTP para obter o tempo, ajuste o fuso horário para GMT-3 e atualiza a cada 60 segundos

// Função para inicializar o sistema de arquivos SPIFFS
void initSPIFFS()
{
    if (!SPIFFS.begin(true))
    {
        Serial.println("Erro ao iniciar SPIFFS. O sistema de arquivos não pôde ser montado.");
        return;
    }
    Serial.println("SPIFFS inicializado com sucesso.");
}

// Função para ler o estado do compressor do arquivo
bool readCompressorState()
{
    Serial.println("Tentando ler o estado do compressor do arquivo.");
    File file = SPIFFS.open(arquivoEstado, "r");
    if (!file)
    {
        Serial.println("Arquivo de estado não encontrado, assumindo estado desligado.");
        return false;
    }

    String state = file.readStringUntil('\n');
    file.close();

    bool estado = state.toInt() == 1;
    Serial.printf("Estado lido do arquivo: %s\n", estado ? "Ligado" : "Desligado");
    return estado;
}

// Função para salvar o estado do compressor no arquivo
void saveCompressorState(bool state)
{
    Serial.printf("Salvando o estado do compressor: %s\n", state ? "Ligado" : "Desligado");
    File file = SPIFFS.open(arquivoEstado, "w");
    if (!file)
    {
        Serial.println("Erro ao abrir o arquivo para escrita.");
        return;
    }

    file.println(state ? "1" : "0");
    file.close();
    Serial.println("Estado do compressor salvo com sucesso.");
}

// Função para verificar se a hora atual está após o horário de fechamento
bool isAfterClosingTime()
{
    timeClient.update();
    int hour = timeClient.getHours();
    bool resultado = hour >= 22;
    Serial.printf("Verificação de horário: %s\n", resultado ? "Após o horário de fechamento" : "Antes do horário de fechamento");
    return resultado;
}

// Função para verificar se a hora atual está antes do horário de abertura
bool isBeforeOpeningTime()
{
    timeClient.update();
    int hour = timeClient.getHours();
    bool resultado = hour < 7;
    Serial.printf("Verificação de horário: %s\n", resultado ? "Antes do horário de abertura" : "Após o horário de abertura");
    return resultado;
}

// Função para configurar o servidor Web para lidar com as solicitações de ligar/desligar o compressor
void setupLigaDesliga(WebServer &server)
{
    Serial.println("Configurando o servidor Web para controle do compressor.");
    initSPIFFS();

    pinMode(pinoCompressor, OUTPUT);
    compressorLigado = readCompressorState();
    digitalWrite(pinoCompressor, compressorLigado ? HIGH : LOW);

    server.on("/toggle", HTTP_GET, [&server]()
              {
        unsigned long currentMillis = millis();
        Serial.println("Requisição recebida para alternar o estado do compressor.");

        if (compressorLigado && (currentMillis - lastToggleTime < 30000))
        {
            Serial.println("Comando ignorado. O compressor foi alterado recentemente. Aguarde 5 minutos entre as tentativas.");
            server.send(200, "text/plain", "Comando ignorado. Aguarde 5 minutos entre as tentativas.");
            return;
        }

        lastToggleTime = currentMillis;
        compressorLigado = !compressorLigado;
        digitalWrite(pinoCompressor, compressorLigado ? HIGH : LOW);

        String message = compressorLigado ? "Compressor ligado!" : "Compressor desligado!";
        if (compressorLigado && (isAfterClosingTime() || isBeforeOpeningTime()))
        {
            message += " Alerta! Já se passou das 22:30 e ainda não chegou às 07:30, atente-se para desligar o compressor após o uso.";
            Serial.println("Alerta: Compressor ligado fora do horário permitido.");
        }

        Serial.print("Estado do compressor: ");
        Serial.println(message);
        server.send(200, "text/plain", message);

        saveCompressorState(compressorLigado);

        if (compressorLigado)
        {
            previousMillis = millis();
            timerAtivo = true;
            Serial.println("Compressor ligado. Timer ativado.");
        }
        else
        {
            Serial.println("Compressor desligado. Timer desativado.");
        } });

    timeClient.begin();

    Serial.print("Estado inicial do compressor: ");
    Serial.println(compressorLigado ? "Ligado" : "Desligado");

    timeClient.update();
    Serial.print("Hora atual: ");
    Serial.println(timeClient.getFormattedTime());

    if (isAfterClosingTime() || isBeforeOpeningTime())
    {
        compressorLigado = false;
        digitalWrite(pinoCompressor, LOW);
        saveCompressorState(compressorLigado);
        Serial.println("Compressor desligado devido ao horário na inicialização.");
    }
    else
    {
        Serial.println("Compressor inicializado dentro do horário permitido.");
    }
}

// Função para atualizar o status do compressor
void updateCompressorStatus()
{
    unsigned long currentMillis = millis();
    if (compressorLigado && (currentMillis - previousMillis >= intervalo))
    {
        previousMillis = currentMillis;
        Serial.println("Compressor status atualizado.");
        saveCompressorState(compressorLigado);
    }
}

// Função para desligar o dispositivo
void shutdown()
{
    saveCompressorState(compressorLigado);
    Serial.println("Dispositivo desligado. Estado do compressor salvo.");
}
