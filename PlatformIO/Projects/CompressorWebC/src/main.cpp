#include <WiFi.h>        // Inclui a biblioteca para WiFi no ESP32
#include <WebServer.h>   // Inclui a biblioteca para criar um servidor web no ESP32
#include <SPIFFS.h>      // Inclui a biblioteca para o sistema de arquivos SPIFFS
#include "index.h"       // Inclui o cabeçalho para a página index
#include "dashboard.h"   // Inclui o cabeçalho para a página dashboard
#include "ligadesliga.h" // Inclui o cabeçalho para a funcionalidade de ligar/desligar
#include "creditos.h"    // Inclui o cabeçalho para a página de créditos
#include "umidade.h"
#include "oleo.h"

// Definições das credenciais das redes WiFi
const char *ssid1 = "CFPFR_WIFI";       // Nome da primeira rede WiFi
const char *password1 = "#CFP-Ur@107!"; // Senha da primeira rede WiFi
const char *ssid2 = "LenonClaro_2.4G";  // Nome da segunda rede WiFi (backup)
const char *password2 = "13539406670";  // Senha da segunda rede WiFi (backup)

// Cria um objeto servidor web na porta 80
WebServer server(80); // Instancia um objeto do servidor web que escuta na porta 80

// Declarações das funções
void connectToWiFi();                                          // Função para conectar ao WiFi
bool tryConnectToWiFi(const char *ssid, const char *password); // Declaração da função de tentativa de conexão
void setupServer();                                            // Função para configurar o servidor
void handleLogin();                                            // Função para lidar com o login

void setup()
{
    Serial.begin(115200);                   // Inicia a comunicação serial a 115200 bps
    Serial.println("Iniciando o setup..."); // Mensagem indicando o início do setup

    connectToWiFi(); // Chama a função para conectar ao WiFi
    setupServer();   // Chama a função para configurar o servidor
}

void loop()
{
    server.handleClient();    // Lida com requisições dos clientes conectados ao servidor
    updateCompressorStatus(); // Atualiza o status do compressor a cada loop
}

void connectToWiFi()
{
    Serial.println("Tentando conectar ao WiFi..."); // Mensagem indicando que a tentativa de conexão está começando

    // Tenta se conectar à primeira rede WiFi
    bool connected = tryConnectToWiFi(ssid1, password1);

    if (!connected)
    {
        // Se a conexão à primeira rede falhar, tenta se conectar à segunda rede WiFi
        Serial.println("Tentando conectar à rede WiFi de backup...");
        connected = tryConnectToWiFi(ssid2, password2);
    }

    if (connected)
    {
        Serial.println("Conectado com sucesso!"); // Mensagem de sucesso na conexão
    }
    else
    {
        Serial.println("Falha ao conectar-se ao WiFi."); // Mensagem de falha na conexão
    }
}

bool tryConnectToWiFi(const char *ssid, const char *password)
{
    Serial.print("Conectando à rede WiFi: ");
    Serial.println(ssid); // Imprime o nome da rede WiFi à qual está tentando se conectar

    unsigned long startAttemptTime = millis(); // Armazena o tempo de início da tentativa de conexão
    WiFi.begin(ssid, password);                // Inicia a conexão WiFi com o SSID e senha fornecidos

    int attempts = 0; // Inicializa o contador de tentativas
    while (WiFi.status() != WL_CONNECTED && attempts < 20)
    {                      // Enquanto não conectar e tiver menos de 20 tentativas
        delay(1000);       // Aguarda 1 segundo
        Serial.print("."); // Imprime um ponto para indicar tentativa de conexão
        attempts++;        // Incrementa o contador de tentativas
    }

    unsigned long endAttemptTime = millis();                                   // Armazena o tempo de término da tentativa de conexão
    unsigned long connectionTime = (endAttemptTime - startAttemptTime) / 1000; // Calcula o tempo total de conexão em segundos

    if (WiFi.status() == WL_CONNECTED)
    {                                                         // Se a conexão for bem-sucedida
        Serial.println();                                     // Imprime uma linha em branco no monitor serial
        Serial.print("Conectado com sucesso! Endereço IP: "); // Mensagem de sucesso na conexão
        Serial.println(WiFi.localIP());                       // Imprime o endereço IP do ESP32
        Serial.print("Tentativas de conexão: ");              // Mensagem com o número de tentativas
        Serial.println(attempts);                             // Imprime o número de tentativas
        Serial.print("Tempo total de conexão: ");             // Mensagem com o tempo total de conexão
        Serial.print(connectionTime);                         // Imprime o tempo total de conexão em segundos
        Serial.println(" segundos");                          // Unidade do tempo
        return true;                                          // Retorna verdadeiro para indicar sucesso na conexão
    }
    else
    {                                                   // Se a conexão falhar
        Serial.println();                               // Imprime uma linha em branco no monitor serial
        Serial.println("Falha ao conectar-se ao WiFi"); // Mensagem de falha na conexão
        Serial.print("Status: ");                       // Mensagem com o status da conexão
        Serial.println(WiFi.status());                  // Imprime o status da conexão WiFi
        Serial.print("Tentativas de conexão: ");        // Mensagem com o número de tentativas
        Serial.println(attempts);                       // Imprime o número de tentativas
        return false;                                   // Retorna falso para indicar falha na conexão
    }
}

void setupServer()
{
    Serial.println("Configurando o servidor..."); // Mensagem indicando que o servidor está sendo configurado

    if (!SPIFFS.begin(true))
    {                                                                    // Se o sistema de arquivos SPIFFS não iniciar
        Serial.println("Falha ao iniciar o sistema de arquivos SPIFFS"); // Mensagem de erro ao iniciar o SPIFFS
        return;                                                          // Sai da função
    }

    // Configura as páginas e funcionalidades do servidor
    setupIndexPage(server);     // Configura a página index
    setupCreditosPage(server);  // Configura a página de créditos
    setupDashboardPage(server); // Configura a página dashboard
    setupLigaDesliga(server);   // Configura a funcionalidade de ligar/desligar
    setupUmidadePage(server);   // Configura a página de umidade
    setupOleoPage(server);      // Configura a página de nível de óleo
    handleToggleAction(server); // Configura a manipulação de ações de ligar/desligar o compressor

    server.on("/login", HTTP_POST, []() { // Define a rota para a requisição POST de login
        handleLogin();                    // Chama a função para lidar com o login
    });

    server.begin();                      // Inicia o servidor
    Serial.println("Servidor iniciado"); // Mensagem indicando que o servidor foi iniciado com sucesso
}

void handleLogin()
{
    // Obtém os valores dos campos 'username' e 'password' do formulário de login
    String username = server.arg("username"); // Obtém o valor do campo username
    String password = server.arg("password"); // Obtém o valor do campo password

    if (username == "admin" && password == "admin123")
    {                                                // Se as credenciais forem válidas
        server.sendHeader("Location", "/dashboard"); // Redireciona para a página dashboard
        server.send(302, "text/plain", "");          // Envia a resposta de redirecionamento HTTP 302
    }
    else
    {                                                                                            // Se as credenciais forem inválidas
        String html = "<html><body><h1>Login falhou. Credenciais inválidas.</h1></body></html>"; // Cria uma resposta HTML com a mensagem de erro
        server.send(401, "text/html", html);                                                     // Envia a resposta de erro HTTP 401
    }
}
