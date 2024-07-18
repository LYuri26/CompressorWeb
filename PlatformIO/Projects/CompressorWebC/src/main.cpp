#include <ESP8266WiFi.h>  // Inclui a biblioteca para WiFi no ESP8266
#include <ESP8266WebServer.h>  // Inclui a biblioteca para criar um servidor web no ESP8266
#include <LittleFS.h>  // Inclui a biblioteca para o sistema de arquivos LittleFS
#include "index.h"  // Inclui o cabeçalho para a página index
#include "dashboard.h"  // Inclui o cabeçalho para a página dashboard
#include "ligadesliga.h"  // Inclui o cabeçalho para a funcionalidade de ligar/desligar

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
    connectToWiFi();  // Chama a função para conectar ao WiFi
    setupServer();  // Chama a função para configurar o servidor
}

void loop() {
    server.handleClient();  // Lida com requisições dos clientes
}

void connectToWiFi() {
    Serial.println();  // Imprime uma linha em branco no monitor serial
    Serial.printf("Conectando à rede WiFi: %s\n", ssid);  // Imprime a mensagem de conexão ao WiFi
    WiFi.begin(ssid, password);  // Inicia a conexão WiFi com o SSID e senha fornecidos
    
    int attempts = 0;  // Inicializa o contador de tentativas
    while (WiFi.status() != WL_CONNECTED && attempts < 10) {  // Enquanto não conectar e tiver menos de 10 tentativas
        delay(1000);  // Aguarda 1 segundo
        Serial.print(".");  // Imprime um ponto para indicar tentativa de conexão
        attempts++;  // Incrementa o contador de tentativas
    }
    
    if (WiFi.status() == WL_CONNECTED) {  // Se a conexão for bem-sucedida
        Serial.println("");  // Imprime uma linha em branco no monitor serial
        Serial.println("WiFi conectado");  // Imprime a mensagem de sucesso de conexão
        Serial.print("Endereço IP: ");  // Imprime a mensagem de endereço IP
        Serial.println(WiFi.localIP());  // Imprime o endereço IP do ESP8266
    } else {  // Se a conexão falhar
        Serial.println("");  // Imprime uma linha em branco no monitor serial
        Serial.println("Falha ao conectar-se ao WiFi");  // Imprime a mensagem de falha de conexão
    }
}

void setupServer() {
    if (!LittleFS.begin()) {  // Se o sistema de arquivos LittleFS não iniciar
        Serial.println("Erro ao montar LittleFS");  // Imprime a mensagem de erro
        return;  // Sai da função
    }
    
    setupIndexPage(server);  // Configura a página index
    setupDashboardPage(server);  // Configura a página dashboard
    setupLigaDesliga(server);  // Configura a funcionalidade de ligar/desligar
    
    server.on("/login", HTTP_POST, [](){  // Define a rota para a requisição POST de login
        handleLogin();  // Chama a função para lidar com o login
    });
    
    server.begin();  // Inicia o servidor
    Serial.println("Servidor HTTP iniciado");  // Imprime a mensagem de servidor iniciado
}

void handleLogin() {
    String username = server.arg("username");  // Obtém o valor do campo username do formulário
    String password = server.arg("password");  // Obtém o valor do campo password do formulário
    
    if (username == "admin" && password == "admin123") {  // Se as credenciais forem válidas
        Serial.println("Login bem-sucedido");  // Imprime a mensagem de login bem-sucedido
        server.sendHeader("Location", "/dashboard");  // Redireciona para a página dashboard
        server.send(302, "text/plain", "");  // Envia a resposta de redirecionamento
    } else {  // Se as credenciais forem inválidas
        Serial.println("Login falhou");  // Imprime a mensagem de login falhado
        String html = "<html><body><h1>Login falhou. Credenciais inválidas.</h1></body></html>";  // Cria uma resposta HTML com a mensagem de erro
        server.send(401, "text/html", html);  // Envia a resposta de erro
    }
}
