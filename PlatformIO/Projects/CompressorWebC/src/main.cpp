#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include "index.h"
#include "dashboard.h"
#include "ligadesliga.h"

const char* ssid = "LenonClaro_2.4G";
const char* password = "13539406670";

ESP8266WebServer server(80);

// Declarações antecipadas das funções
void connectToWiFi();
void setupServer();
void handleLogin();

void setup() {
    Serial.begin(115200);
    connectToWiFi();
    setupServer();
}

void loop() {
    server.handleClient();
}

void connectToWiFi() {
    Serial.println();
    Serial.printf("Conectando à rede WiFi: %s\n", ssid);
    WiFi.begin(ssid, password);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 10) {
        delay(1000);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("");
        Serial.println("WiFi conectado");
        Serial.print("Endereço IP: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("");
        Serial.println("Falha ao conectar-se ao WiFi");
    }
}

void setupServer() {
    if (!LittleFS.begin()) {
        Serial.println("Erro ao montar LittleFS");
        return;
    }
    
    setupIndexPage(server);
    setupDashboardPage(server);
    setupLigaDesliga(server);
    
    server.on("/login", HTTP_POST, [](){
        handleLogin();
    });
    
    server.begin();
    Serial.println("Servidor HTTP iniciado");
}

void handleLogin() {
    String username = server.arg("username");
    String password = server.arg("password");
    
    if (username == "admin" && password == "admin123") {
        Serial.println("Login bem-sucedido");
        server.sendHeader("Location", "/dashboard");
        server.send(302, "text/plain", "");
    } else {
        Serial.println("Login falhou");
        String html = "<html><body><h1>Login falhou. Credenciais inválidas.</h1></body></html>";
        server.send(401, "text/html", html);
    }
}
