#include <ESP8266WiFi.h>  // Inclui a biblioteca para WiFi no ESP8266
#include <ESP8266WebServer.h>  // Inclui a biblioteca para criar um servidor web no ESP8266
#include <LittleFS.h>  // Inclui a biblioteca para o sistema de arquivos LittleFS
#include "index.h"  // Inclui o cabeçalho para a página index
#include "dashboard.h"  // Inclui o cabeçalho para a página dashboard
#include "ligadesliga.h"  // Inclui o cabeçalho para a funcionalidade de ligar/desligar
#include "creditos.h"  // Inclui o cabeçalho para a página de créditos

// Definições das credenciais da rede WiFi
const char* ssid = "LenonClaro_2.4G";
const char* password = "13539406670";

// Cria um objeto servidor web na porta 80
ESP8266WebServer server(80);

// Declarações antecipadas das funções
void connectToWiFi();  // Função para conectar ao WiFi
void setupServer();  // Função para configurar o servidor
void handleLogin();  // Função para lidar com o login

void setup() {
    Serial.begin(115200);  // Inicia a comunicação serial a 115200 bps
    Serial.println("Iniciando o setup...");  // Mensagem de início do setup

    connectToWiFi();  // Chama a função para conectar ao WiFi
    setupServer();  // Chama a função para configurar o servidor
}

void loop() {
    server.handleClient();  // Lida com requisições dos clientes
}

void connectToWiFi() {
    Serial.println("Tentando conectar ao WiFi...");  // Mensagem de tentativa de conexão
    Serial.print("Conectando à rede WiFi: ");  // Mensagem de conexão ao WiFi
    Serial.println(ssid);  // Imprime o SSID da rede WiFi

    unsigned long startAttemptTime = millis();  // Armazena o tempo de início da tentativa de conexão
    WiFi.begin(ssid, password);  // Inicia a conexão WiFi com o SSID e senha fornecidos

    int attempts = 0;  // Inicializa o contador de tentativas
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {  // Enquanto não conectar e tiver menos de 20 tentativas
        delay(1000);  // Aguarda 1 segundo
        Serial.print(".");  // Imprime um ponto para indicar tentativa de conexão
        attempts++;  // Incrementa o contador de tentativas
    }

    unsigned long endAttemptTime = millis();  // Armazena o tempo de término da tentativa de conexão
    unsigned long connectionTime = (endAttemptTime - startAttemptTime) / 1000;  // Calcula o tempo total de conexão em segundos

    if (WiFi.status() == WL_CONNECTED) {  // Se a conexão for bem-sucedida
        Serial.println();  // Imprime uma linha em branco no monitor serial
        Serial.print("Conectado com sucesso! Endereço IP: ");  // Imprime a mensagem de sucesso
        Serial.println(WiFi.localIP());  // Imprime o endereço IP do ESP8266
        Serial.print("Tentativas de conexão: ");  // Imprime o número de tentativas
        Serial.println(attempts);  // Imprime o número de tentativas
        Serial.print("Tempo total de conexão: ");  // Imprime o tempo total de conexão
        Serial.print(connectionTime);  // Imprime o tempo total de conexão
        Serial.println(" segundos");  // Unidade do tempo
    } else {  // Se a conexão falhar
        Serial.println();  // Imprime uma linha em branco no monitor serial
        Serial.println("Falha ao conectar-se ao WiFi");  // Imprime a mensagem de falha de conexão
        Serial.print("Status: ");  // Imprime o status da conexão
        Serial.println(WiFi.status());  // Imprime o status da conexão WiFi
        Serial.print("Tentativas de conexão: ");  // Imprime o número de tentativas
        Serial.println(attempts);  // Imprime o número de tentativas
    }
}

void setupServer() {
    Serial.println("Configurando o servidor...");  // Mensagem de configuração do servidor

    if (!LittleFS.begin()) {  // Se o sistema de arquivos LittleFS não iniciar
        Serial.println("Falha ao iniciar o sistema de arquivos LittleFS");  // Mensagem de erro
        return;  // Sai da função
    }

    setupIndexPage(server);  // Configura a página index
    setupCreditsPage(server);  // Configura a página de créditos
    setupDashboardPage(server);  // Configura a página dashboard
    setupLigaDesliga(server);  // Configura a funcionalidade de ligar/desligar

    server.on("/login", HTTP_POST, [](){  // Define a rota para a requisição POST de login
        handleLogin();  // Chama a função para lidar com o login
    });

    server.begin();  // Inicia o servidor
    Serial.println("Servidor iniciado");  // Mensagem de sucesso
}

void handleLogin() {
    String username = server.arg("username");  // Obtém o valor do campo username do formulário
    String password = server.arg("password");  // Obtém o valor do campo password do formulário

    if (username == "admin" && password == "admin123") {  // Se as credenciais forem válidas
        server.sendHeader("Location", "/dashboard");  // Redireciona para a página dashboard
        server.send(302, "text/plain", "");  // Envia a resposta de redirecionamento
    } else {  // Se as credenciais forem inválidas
        String html = "<html><body><h1>Login falhou. Credenciais inválidas.</h1></body></html>";  // Cria uma resposta HTML com a mensagem de erro
        server.send(401, "text/html", html);  // Envia a resposta de erro
    }
}
