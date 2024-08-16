#include <WiFi.h>   // Biblioteca para gerenciar a conexão Wi-Fi
#include <SPIFFS.h> // Biblioteca para gerenciar o sistema de arquivos SPIFFS
#include <vector>   // Biblioteca para usar vetores

// -------------------------------------------------------------------------
// Configurações Globais
// -------------------------------------------------------------------------
const char *ssid = "";                 // SSID da rede Wi-Fi para conexão (deixe vazio se não houver rede pré-configurada)
const char *password = "";             // Senha da rede Wi-Fi para conexão (deixe vazio se não houver rede pré-configurada)
const char *ap_ssid = "CompressorWeb"; // SSID do Access Point (nome da rede gerada pelo dispositivo)
const char *ap_password = "12345678";  // Senha do Access Point (senha para se conectar à rede gerada pelo dispositivo)

IPAddress local_ip(192, 168, 26, 7); // IP local do Access Point (endereço IP do Access Point no formato (A, B, C, D))
IPAddress gateway(192, 168, 26, 1);  // Gateway do Access Point (endereço do roteador na rede do Access Point)
IPAddress subnet(255, 255, 255, 0);  // Máscara de sub-rede do Access Point (define a faixa de IPs na rede do Access Point)

const unsigned long scanInterval = 5000; // Intervalo entre escaneamentos (em milissegundos)
unsigned long lastScan = 0;              // Armazena o tempo do último escaneamento (para evitar escaneamentos excessivos)

bool scanning = false; // Indica se o escaneamento de redes está em andamento (evita múltiplos escaneamentos simultâneos)
bool isAPMode = false; // Indica se o dispositivo está no modo Access Point (para saber se a configuração é para modo AP)
bool scanDone = false; // Indica se o escaneamento foi concluído (para saber quando usar os resultados)
int scanResults = 0;   // Número de redes encontradas durante o escaneamento (para exibir ou processar redes encontradas)

// -------------------------------------------------------------------------
// Função para Configurar o Modo AP
// -------------------------------------------------------------------------
void setupAP()
{
    WiFi.mode(WIFI_AP_STA);                       // Configura o ESP32 para operar tanto em modo Access Point quanto em modo Estação (permite conectividade e criação de rede)
    WiFi.softAPConfig(local_ip, gateway, subnet); // Configura o IP, gateway e máscara de sub-rede do AP (define a configuração de rede local)
    if (WiFi.softAP(ap_ssid, ap_password))        // Inicia o Access Point com o SSID e a senha fornecidos (cria uma rede Wi-Fi que outros dispositivos podem se conectar)
    {
        Serial.print("Modo AP iniciado. Endereço IP: ");
        Serial.println(WiFi.softAPIP()); // Imprime o IP do Access Point no monitor serial (para ajudar na conexão)
        Serial.print("Nome da rede AP: ");
        Serial.println(ap_ssid); // Imprime o SSID do Access Point no monitor serial (para ajudar na conexão)
        isAPMode = true;         // Marca que o dispositivo está em modo Access Point (para controle de fluxo do código)
    }
    else
    {
        Serial.println("Falha ao iniciar o Access Point."); // Mensagem de erro se o Access Point não iniciar corretamente
    }
}

// -------------------------------------------------------------------------
// Função para Conectar ao Wi-Fi
// -------------------------------------------------------------------------
void connectToWiFi(const char *ssid, const char *password)
{
    if (!ssid || !password || strlen(ssid) == 0 || strlen(password) == 0)
    {
        Serial.println("SSID ou senha inválidos."); // Mensagem de erro se o SSID ou senha estiverem ausentes ou vazios
        return;
    }

    Serial.print("Tentando conectar ao WiFi: ");
    Serial.println(ssid);       // Imprime o SSID da rede Wi-Fi para o qual o dispositivo está tentando se conectar
    WiFi.begin(ssid, password); // Inicia a tentativa de conexão com a rede Wi-Fi fornecida

    int attempts = 0;
    const int maxAttempts = 10; // Aumenta o número máximo de tentativas para melhorar a conexão (evita falhas de conexão devido a tentativas insuficientes)

    while (WiFi.status() != WL_CONNECTED && attempts < maxAttempts)
    {
        delay(1000);       // Espera 1 segundo entre as tentativas de conexão
        Serial.print("."); // Imprime um ponto a cada tentativa para indicar progresso
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println();
        Serial.print("Conexão feita! Endereço IP: ");
        Serial.println(WiFi.localIP()); // Imprime o IP local após uma conexão bem-sucedida
    }
    else
    {
        Serial.println();
        Serial.println("Conexão falhou."); // Mensagem de erro se a conexão falhar após todas as tentativas
        // Mantenha o modo AP ativo independentemente do sucesso da conexão
    }
}

// -------------------------------------------------------------------------
// Função para Iniciar o Escaneamento de Redes Wi-Fi
// -------------------------------------------------------------------------
void startScanWiFiNetworks()
{
    if (!scanning && millis() - lastScan > scanInterval)
    {
        scanning = true;
        WiFi.scanNetworks(true); // Inicia o escaneamento de redes Wi-Fi (modo ativo) para encontrar redes disponíveis
        lastScan = millis();     // Atualiza o tempo do último escaneamento para controlar o intervalo
    }
}

// -------------------------------------------------------------------------
// Função para Obter os Resultados do Escaneamento
// -------------------------------------------------------------------------
String getScanResults()
{
    int n = WiFi.scanComplete(); // Obtém o número de redes encontradas ou o estado do escaneamento
    if (n == -2 || n == -1)      // Se o escaneamento não foi iniciado (-1) ou está em andamento (-2)
    {
        return "[]"; // Retorna um JSON vazio indicando que ainda não há resultados
    }
    else if (n == 0) // Se nenhuma rede foi encontrada
    {
        return "[]"; // Retorna um JSON vazio indicando que não há redes disponíveis
    }
    else
    {
        String json = "[";
        for (int i = 0; i < n; i++)
        {
            json += "{\"ssid\":\"" + WiFi.SSID(i) + "\"}"; // Adiciona cada SSID encontrado ao JSON
            if (i < n - 1)
                json += ","; // Adiciona uma vírgula entre os itens do JSON
        }
        json += "]";
        scanning = false; // Marca que o escaneamento foi concluído
        return json;      // Retorna o JSON com os resultados do escaneamento
    }
}

// -------------------------------------------------------------------------
// Função para Verificar se o Escaneamento foi Concluído
// -------------------------------------------------------------------------
void checkScanResults()
{
    if (WiFi.scanComplete() != WIFI_SCAN_RUNNING) // Verifica se o escaneamento ainda está em andamento
    {
        scanDone = true;                   // Marca que o escaneamento foi concluído
        scanResults = WiFi.scanComplete(); // Obtém o número de redes encontradas
        scanning = false;                  // Marca que o escaneamento foi concluído
    }
}

// -------------------------------------------------------------------------
// Função para Carregar Redes Wi-Fi Salvas e Tentar Conexão
// -------------------------------------------------------------------------
void loadSavedWiFiNetworks()
{
    File file = SPIFFS.open("/wifiredes.txt", FILE_READ); // Abre o arquivo contendo redes Wi-Fi salvas para leitura
    if (!file)
    {
        Serial.println("Erro ao abrir o arquivo de redes Wi-Fi."); // Mensagem de erro se o arquivo não for aberto
        return;
    }

    String content = file.readString(); // Lê todo o conteúdo do arquivo como uma String
    file.close();                       // Fecha o arquivo após leitura

    String ssid;
    String password;
    std::vector<String> networks; // Vetor para armazenar as redes Wi-Fi salvas

    int start = 0;
    while (start < content.length())
    {
        int end = content.indexOf('\n', start); // Encontra o final da linha atual
        if (end == -1)
        {
            end = content.length(); // Se não houver mais quebras de linha, define o fim como o final do conteúdo
        }
        String line = content.substring(start, end); // Extrai a linha atual
        networks.push_back(line);                    // Adiciona a linha ao vetor de redes
        start = end + 1;                             // Move o ponto de início para a próxima linha
    }

    for (int i = networks.size() - 1; i >= 0; i--) // Percorre o vetor de redes de trás para frente
    {
        String line = networks[i];
        int commaIndex = line.indexOf(','); // Localiza a vírgula que separa o SSID da senha
        if (commaIndex != -1)
        {
            ssid = line.substring(0, commaIndex);          // Extrai o SSID da linha
            password = line.substring(commaIndex + 1);     // Extrai a senha da linha
            connectToWiFi(ssid.c_str(), password.c_str()); // Tenta se conectar à rede Wi-Fi com o SSID e senha extraídos
            if (WiFi.status() == WL_CONNECTED)
            {
                return; // Se conectado, sai da função
            }
        }
    }
    // Caso não consiga conectar a nenhuma rede salva, o dispositivo continuará no modo AP
}
