#include <WiFi.h> // Inclui a biblioteca WiFi para conectar o ESP32 a redes WiFi
#include <WebServer.h> // Inclui a biblioteca WebServer para criar um servidor web
#include <SPIFFS.h> // Inclui a biblioteca SPIFFS para acessar o sistema de arquivos SPIFFS
#include "index.h" // Inclui o cabeçalho que contém a configuração da página index
#include "dashboard.h" // Inclui o cabeçalho que contém a configuração da página dashboard
#include "ligadesliga.h" // Inclui o cabeçalho que contém a configuração da página liga/desliga
#include "creditos.h" // Inclui o cabeçalho que contém a configuração da página de créditos
#include "umidade.h" // Inclui o cabeçalho que contém a configuração da página de umidade
#include "oleo.h" // Inclui o cabeçalho que contém a configuração da página de nível de óleo
#include "wifi.h" // Inclui o cabeçalho que contém a configuração e conexão WiFi
#include "autenticador.h" // Inclui o cabeçalho que contém funções de autenticação

// Cria um objeto servidor web na porta 80
WebServer server(80); // Instancia um objeto WebServer na porta 80

// Declaração de funções
void setup(); // Declara a função setup
void loop(); // Declara a função loop
void setupServer(); // Declara a função setupServer para configurar o servidor
void setupAcessoInvalidoPage(WebServer& server); // Declara o protótipo da função para configurar a página de acesso inválido
void setupNotFoundPage(WebServer& server); // Declara o protótipo da função para configurar a página de erro 404

void setup()
{
    Serial.begin(115200); // Inicia a comunicação serial com velocidade de 115200 bps
    Serial.println("Iniciando o setup..."); // Imprime uma mensagem na serial

    connectToWiFi(); // Chama a função para conectar ao WiFi
    setupServer(); // Chama a função para configurar o servidor
}

void loop()
{
    server.handleClient(); // Processa as requisições dos clientes
    updateCompressorStatus(); // Chama uma função para atualizar o status do compressor

    // Reconecta se a conexão WiFi for perdida
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("Conexão WiFi perdida. Tentando reconectar..."); // Imprime uma mensagem na serial
        connectToWiFi(); // Tenta reconectar ao WiFi
    }
}

void setupServer()
{
    Serial.println("Configurando o servidor..."); // Imprime uma mensagem na serial

    if (!SPIFFS.begin(true))
    {
        Serial.println("Falha ao iniciar o sistema de arquivos SPIFFS"); // Imprime uma mensagem de erro na serial
        return; // Sai da função se falhar ao iniciar o SPIFFS
    }

    setupIndexPage(server); // Configura a página index
    setupCreditosPage(server); // Configura a página de créditos
    setupDashboardPage(server); // Configura a página dashboard
    setupLigaDesliga(server); // Configura a página liga/desliga
    setupUmidadePage(server); // Configura a página de umidade
    setupOleoPage(server); // Configura a página de nível de óleo
    handleToggleAction(server); // Configura a ação de alternância liga/desliga
    setupAcessoInvalidoPage(server); // Configura a página de acesso inválido
    setupNotFoundPage(server); // Configura a página de erro 404

    server.on("/login", HTTP_POST, handleLogin); // Adiciona a rota de login para requisições POST
    server.on("/logout", HTTP_GET, handleLogout); // Adiciona a rota de logout para requisições GET

    server.begin(); // Inicia o servidor web
    Serial.println("Servidor iniciado"); // Imprime uma mensagem na serial
}

void setupAcessoInvalidoPage(WebServer& server)
{
    // Configura a rota para a página de acesso inválido
    server.on("/acesso-invalido", HTTP_GET, [&server]() {
        // Envia a resposta HTTP com o cabeçalho "Content-Type" e o código 403 (Forbidden)
        server.sendHeader("Content-Type", "text/html");
        server.send(403, "text/html", R"rawliteral(
<!DOCTYPE html>
<html lang="pt-br">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Acesso Inválido</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background-color: #f4f4f4;
        }
        .container {
            text-align: center;
            background-color: #fff;
            padding: 20px;
            border-radius: 8px;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
        }
        h1 {
            color: #e74c3c;
        }
        p {
            color: #555;
        }
        a {
            text-decoration: none;
            color: #3498db;
            font-weight: bold;
            padding: 10px 20px;
            border: 2px solid #3498db;
            border-radius: 5px;
            display: inline-block;
        }
        a:hover {
            text-decoration: underline;
            background-color: #f0f0f0;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Acesso Inválido</h1>
        <p>Você não tem permissão para acessar esta página. Por favor, faça login para continuar.</p>
        <a href="/">Voltar à Página Inicial</a>
    </div>
</body>
</html>
)rawliteral"); // Envia a página HTML de acesso inválido
    });
}

void setupNotFoundPage(WebServer& server)
{
    // Configura a rota padrão para páginas não encontradas (404)
    server.onNotFound([&server]() {
        // Envia a resposta HTTP com o cabeçalho "Content-Type" e o código 404 (Not Found)
        server.sendHeader("Content-Type", "text/html");
        server.send(404, "text/html", R"rawliteral(
<!DOCTYPE html>
<html lang="pt-br">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Página Não Encontrada</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background-color: #f4f4f4;
        }
        .container {
            text-align: center;
            background-color: #fff;
            padding: 20px;
            border-radius: 8px;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
        }
        h1 {
            color: #e74c3c;
        }
        p {
            color: #555;
        }
        a {
            text-decoration: none;
            color: #3498db;
            font-weight: bold;
            padding: 10px 20px;
            border: 2px solid #3498db;
            border-radius: 5px;
            display: inline-block;
        }
        a:hover {
            text-decoration: underline;
            background-color: #f0f0f0;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Página Não Encontrada</h1>
        <p>Essa página não existe. Verifique o URL ou volte à página inicial.</p>
        <a href="/">Voltar à Página Inicial</a>
    </div>
</body>
</html>
)rawliteral"); // Envia a página HTML de erro 404
    });
}
