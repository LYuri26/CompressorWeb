#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>
#include "index.h"
#include "dashboard.h"
#include "ligadesliga.h"
#include "creditos.h"
#include "umidade.h"
#include "oleo.h"
#include "wifi.h"
#include "autenticador.h"  // Certifique-se de que o cabeçalho esteja incluído

// Cria um objeto servidor web na porta 80
WebServer server(80);

// Declaração de funções
void setup();
void loop();
void setupServer();
void setupAcessoInvalidoPage(WebServer& server); // Declaração do protótipo
void setupNotFoundPage(WebServer& server); // Declaração do protótipo

void setup()
{
    Serial.begin(115200);
    Serial.println("Iniciando o setup...");

    connectToWiFi();
    setupServer();
}

void loop()
{
    server.handleClient();
    updateCompressorStatus();

    // Reconecta se a conexão WiFi for perdida
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("Conexão WiFi perdida. Tentando reconectar...");
        connectToWiFi();
    }
}

void setupServer()
{
    Serial.println("Configurando o servidor...");

    if (!SPIFFS.begin(true))
    {
        Serial.println("Falha ao iniciar o sistema de arquivos SPIFFS");
        return;
    }

    setupIndexPage(server);
    setupCreditosPage(server);
    setupDashboardPage(server);
    setupLigaDesliga(server);
    setupUmidadePage(server);
    setupOleoPage(server);
    handleToggleAction(server);
    setupAcessoInvalidoPage(server); // Adiciona a configuração da página de acesso inválido
    setupNotFoundPage(server); // Adiciona a configuração da página de erro 404

    server.on("/login", HTTP_POST, handleLogin);
    server.on("/logout", HTTP_GET, handleLogout); // Adiciona a rota de logout

    server.begin();
    Serial.println("Servidor iniciado");
}

void setupAcessoInvalidoPage(WebServer& server)
{
    server.on("/acesso-invalido", HTTP_GET, [&server]() {
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
)rawliteral");
    });
}

void setupNotFoundPage(WebServer& server)
{
    server.onNotFound([&server]() {
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
)rawliteral");
    });
}
