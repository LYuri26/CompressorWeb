#include <WiFi.h>                   // Inclui a biblioteca WiFi para conectar-se a redes Wi-Fi
#include <ESPAsyncWebServer.h>      // Inclui a biblioteca para o servidor web assíncrono
#include <SPIFFS.h>                 // Inclui a biblioteca para o sistema de arquivos SPIFFS
#include "wificonexao.h"            // Inclui o arquivo de cabeçalho com funções de conexão Wi-Fi

// Função para configurar a página de gerenciamento de Wi-Fi
void setupWiFiManagementPage(AsyncWebServer &server) {
    // -------------------------------------------------------------------------
    // Inicializa o sistema de arquivos SPIFFS
    // -------------------------------------------------------------------------
    if (!SPIFFS.begin(true)) {
        Serial.println("Falha ao iniciar o sistema de arquivos SPIFFS");
        return; // Interrompe a função se SPIFFS não iniciar
    }

    // -------------------------------------------------------------------------
    // Rota para a página de gerenciamento de Wi-Fi
    // -------------------------------------------------------------------------
    server.on("/wifi-management", HTTP_GET, [](AsyncWebServerRequest *request) {
        // Define o conteúdo HTML da página de gerenciamento de Wi-Fi
        String html = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-br">
<head>
    <meta charset="UTF-8"> <!-- Define a codificação de caracteres para UTF-8 -->
    <meta name="viewport" content="width=device-width, initial-scale=1.0"> <!-- Define a largura da página e o zoom inicial -->
    <title>Gerenciamento Wi-Fi</title> <!-- Define o título da página -->
    <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css"> <!-- Inclui o CSS do Bootstrap para estilização -->
    <style>
        /* Estilos para o corpo da página */
        body {
            font-family: Arial, sans-serif; /* Define a fonte do corpo da página */
            background-color: #f8f9fa; /* Define a cor de fundo da página */
            margin: 0; /* Remove as margens padrão */
            padding: 0; /* Remove o preenchimento padrão */
            display: flex; /* Usa flexbox para o layout da página */
            flex-direction: column; /* Define a direção do layout como coluna */
            min-height: 100vh; /* Define a altura mínima da página como 100% da altura da janela de visualização */
        }

        /* Estilos para o contêiner principal */
        .container {
            background-color: #ffffff; /* Define a cor de fundo do contêiner */
            padding: 20px; /* Adiciona preenchimento interno ao contêiner */
            border-radius: 5px; /* Adiciona bordas arredondadas ao contêiner */
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); /* Adiciona uma sombra ao contêiner */
            width: 100%; /* Define a largura do contêiner como 100% */
            max-width: 600px; /* Define a largura máxima do contêiner */
            margin: 30px auto; /* Define a margem superior e inferior como 30px e centraliza horizontalmente */
            flex: 1; /* Faz o contêiner crescer para preencher o espaço disponível */
        }

        /* Estilos para o rodapé */
        .footer {
            background-color: #007bff; /* Define a cor de fundo do rodapé */
            color: white; /* Define a cor do texto no rodapé */
            text-align: center; /* Centraliza o texto no rodapé */
            padding: 10px 0; /* Adiciona preenchimento vertical ao rodapé */
            font-size: 14px; /* Define o tamanho da fonte do texto no rodapé */
            margin-top: 30px; /* Adiciona uma margem superior ao rodapé para separá-lo do conteúdo acima */
        }

        /* Estilos para a lista de redes salvas */
        #saved-networks {
            max-height: 300px; /* Define a altura máxima para a lista de redes salvas */
            overflow-y: auto; /* Adiciona rolagem vertical se o conteúdo exceder a altura máxima */
            margin-bottom: 20px; /* Adiciona espaçamento inferior */
        }

        /* Estilos para o botão de conectar */
        .btn-success {
            background-color: #28a745; /* Cor de fundo do botão de conectar */
            color: white; /* Cor do texto do botão */
        }

        .btn-success:hover {
            background-color: #218838; /* Cor de fundo do botão de conectar ao passar o mouse sobre ele */
        }

        /* Estilos para o botão de voltar */
        .btn-blue {
            background-color: #007bff; /* Cor de fundo do botão de voltar */
            color: white; /* Cor do texto do botão */
            border: none; /* Remove a borda do botão */
            padding: 10px 20px; /* Adiciona padding ao botão para espaçamento interno */
            font-size: 16px; /* Define o tamanho da fonte do botão */
            cursor: pointer; /* Muda o cursor para uma mão ao passar o mouse sobre o botão */
            border-radius: 3px; /* Adiciona bordas arredondadas ao botão */
            width: 100%; /* Define a largura como 100% */
        }

        .btn-blue:hover {
            background-color: #0056b3; /* Altera a cor de fundo ao passar o mouse sobre o botão de voltar */
        }

        /* Estilos para o botão de deletar */
        .btn-custom-danger {
            background-color: #dc3545; /* Cor de fundo do botão de deletar */
            color: white; /* Cor do texto do botão */
            border: none; /* Remove a borda do botão */
            padding: 5px 10px; /* Adiciona padding ao botão para espaçamento interno */
            font-size: 14px; /* Define o tamanho da fonte do botão */
            cursor: pointer; /* Muda o cursor para uma mão ao passar o mouse sobre o botão */
            border-radius: 3px; /* Adiciona bordas arredondadas ao botão */
            text-decoration: none; /* Remove o sublinhado do link */
        }

        .btn-custom-danger:hover {
            background-color: #c82333; /* Altera a cor de fundo ao passar o mouse sobre o botão de deletar */
        }

        /* Estilos para o grupo de entrada com botão de alternar senha */
        .input-group {
            display: flex; /* Usa o flexbox para o layout do grupo de entrada */
            align-items: center; /* Alinha os itens verticalmente ao centro */
        }

        .input-group-append {
            margin-left: -1px; /* Remove a margem esquerda do botão para que ele se encaixe com o campo de entrada */
        }
    </style>
</head>
<body>
    <div class="container">
        <h2>Gerenciamento de Redes Wi-Fi</h2>
        <div id="message" class="alert" role="alert"></div> <!-- Div para mensagens de status -->
        <form id="save-form" action="/save-wifi" method="post">
            <div class="form-group">
                <label for="ssid">SSID da Rede Wi-Fi:</label>
                <input type="text" id="ssid" name="ssid" class="form-control" placeholder="Digite o SSID" required>
            </div>
            <div class="form-group">
                <label for="password">Senha:</label>
                <div class="input-group">
                    <input type="password" id="password" name="password" class="form-control" placeholder="Digite a senha" required>
                    <div class="input-group-append">
                        <button class="btn btn-outline-secondary" type="button" id="toggle-password">Mostrar</button>
                    </div>
                </div>
            </div>
            <button type="submit" class="btn btn-success btn-block">Conectar</button>
            <button type="button" onclick="window.history.back()" class="btn-blue">Voltar</button>
        </form>
        <hr>
        <div id="saved-networks">Aguardando redes salvas...</div>
    </div>
    <div class="footer">
        <p>Aplicação desenvolvida pela Turma de Informática Para Internet Trilhas de Futuro 2024</p>
        <p>Instrutor: Lenon Yuri</p>
    </div>
    <script>
        // -------------------------------------------------------------------------
        // Função para buscar redes Wi-Fi salvas e atualizar a interface
        // -------------------------------------------------------------------------
        function fetchSavedNetworks() {
            fetch('/list-saved-wifi') // Faz uma solicitação GET para a rota '/list-saved-wifi'
                .then(response => {
                    if (!response.ok) {
                        throw new Error('Network response was not ok');
                    }
                    return response.text(); // Converte a resposta em texto
                })
                .then(data => {
                    var savedNetworks = document.getElementById('saved-networks');
                    savedNetworks.innerHTML = ''; // Limpa o conteúdo atual
                    var networks = data.trim().split('\n'); // Divide a resposta em linhas
                    if (networks.length > 0 && networks[0] !== "") {
                        // Se houver redes salvas, atualiza a lista na página
                        networks.forEach(network => {
                            var parts = network.split(','); // Divide cada linha em SSID e senha
                            savedNetworks.innerHTML += '<p><strong>SSID:</strong> ' + parts[0] + ' <a class="btn btn-custom-danger" href="/delete-wifi?ssid=' + parts[0] + '">Delete</a></p>';
                        });
                    } else {
                        savedNetworks.innerHTML = '<p>Nenhuma rede salva encontrada.</p>'; // Mensagem se nenhuma rede estiver salva
                    }
                })
                .catch(error => {
                    console.error('Erro ao buscar as redes salvas:', error); // Log de erros no console
                    var savedNetworks = document.getElementById('saved-networks');
                    savedNetworks.innerHTML = '<p>Erro ao buscar redes salvas.</p>'; // Mensagem de erro na página
                });
        }

        // -------------------------------------------------------------------------
        // Atualiza a lista de redes salvas ao carregar a página
        // -------------------------------------------------------------------------
        document.addEventListener('DOMContentLoaded', function() {
            fetchSavedNetworks(); // Chama a função para buscar as redes salvas
        });

        // -------------------------------------------------------------------------
        // Alterna a visibilidade da senha no campo de entrada
        // -------------------------------------------------------------------------
        document.getElementById('toggle-password').addEventListener('click', function() {
            var passwordField = document.getElementById('password');
            var button = this;
            if (passwordField.type === 'password') {
                passwordField.type = 'text';
                button.textContent = 'Ocultar'; // Muda o texto do botão para 'Ocultar'
            } else {
                passwordField.type = 'password';
                button.textContent = 'Mostrar'; // Muda o texto do botão para 'Mostrar'
            }
        });
    </script>
</body>
</html>
        )rawliteral";

        // -------------------------------------------------------------------------
        // Envia a resposta HTML com o código definido
        // -------------------------------------------------------------------------
        request->send(200, "text/html", html);
    });

    // -------------------------------------------------------------------------
    // Rota para listar redes Wi-Fi salvas
    // -------------------------------------------------------------------------
    server.on("/list-saved-wifi", HTTP_GET, [](AsyncWebServerRequest *request) {
        // Abre o arquivo de redes Wi-Fi para leitura
        File file = SPIFFS.open("/wifiredes.txt", FILE_READ);
        if (!file) {
            request->send(500, "text/plain", "Erro ao abrir o arquivo de redes Wi-Fi"); // Responde com erro se não conseguir abrir o arquivo
            return;
        }

        // Lê o conteúdo do arquivo
        String networks = file.readString();
        file.close(); // Fecha o arquivo após leitura
        request->send(200, "text/plain", networks); // Envia o conteúdo lido como resposta
    });

    // -------------------------------------------------------------------------
    // Rota para salvar uma nova rede Wi-Fi
    // -------------------------------------------------------------------------
    server.on("/save-wifi", HTTP_POST, [](AsyncWebServerRequest *request) {
        String ssid;
        String password;

        if (request->hasParam("ssid", true) && request->hasParam("password", true)) {
            // Obtém o SSID e a senha do formulário
            ssid = request->getParam("ssid", true)->value();
            password = request->getParam("password", true)->value();

            // Abre o arquivo de redes Wi-Fi para leitura
            File file = SPIFFS.open("/wifiredes.txt", FILE_READ);
            if (!file) {
                request->send(500, "text/plain", "Erro ao abrir o arquivo de redes Wi-Fi"); // Responde com erro se não conseguir abrir o arquivo
                return;
            }

            // Lê o conteúdo do arquivo
            String content = file.readString();
            file.close(); // Fecha o arquivo após leitura

            String newContent = "";
            bool ssidExists = false;
            if (content.length() > 0) {
                // Processa o conteúdo existente para verificar se o SSID já está presente
                int start = 0;
                while (start < content.length()) {
                    int end = content.indexOf('\n', start);
                    if (end == -1) {
                        end = content.length();
                    }
                    String line = content.substring(start, end);
                    int commaIndex = line.indexOf(',');
                    String savedSSID = line.substring(0, commaIndex);
                    if (savedSSID == ssid) {
                        newContent += ssid + "," + password + "\n"; // Atualiza a entrada existente com a nova senha
                        ssidExists = true;
                    } else {
                        newContent += line + "\n"; // Mantém a entrada existente
                    }
                    start = end + 1;
                }
            }

            if (!ssidExists) {
                newContent += ssid + "," + password + "\n"; // Adiciona nova entrada se o SSID não existir
            }

            // Abre o arquivo de redes Wi-Fi para escrita
            file = SPIFFS.open("/wifiredes.txt", FILE_WRITE);
            if (!file) {
                request->send(500, "text/plain", "Erro ao abrir o arquivo para escrita"); // Responde com erro se não conseguir abrir o arquivo
                return;
            }
            file.print(newContent); // Escreve o conteúdo atualizado no arquivo
            file.close(); // Fecha o arquivo após escrita

            // Tenta conectar à nova rede salva
            connectToWiFi(ssid.c_str(), password.c_str());

            request->redirect("/wifi-management"); // Redireciona para a página de gerenciamento de Wi-Fi
        } else {
            request->send(400, "text/plain", "Dados ausentes."); // Responde com erro se os dados estiverem ausentes
        }
    });

    // -------------------------------------------------------------------------
    // Rota para deletar uma rede Wi-Fi salva
    // -------------------------------------------------------------------------
    server.on("/delete-wifi", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (request->hasParam("ssid")) {
            // Obtém o SSID a ser excluído
            String ssidToDelete = request->getParam("ssid")->value();

            // Abre o arquivo de redes Wi-Fi para leitura
            File file = SPIFFS.open("/wifiredes.txt", FILE_READ);
            if (!file) {
                request->send(500, "text/plain", "Erro ao abrir o arquivo de redes Wi-Fi"); // Responde com erro se não conseguir abrir o arquivo
                return;
            }

            // Lê o conteúdo do arquivo
            String content = file.readString();
            file.close(); // Fecha o arquivo após leitura

            String newContent = "";
            int start = 0;
            while (start < content.length()) {
                int end = content.indexOf('\n', start);
                if (end == -1) {
                    end = content.length();
                }
                String line = content.substring(start, end);
                int commaIndex = line.indexOf(',');
                String savedSSID = line.substring(0, commaIndex);
                if (savedSSID != ssidToDelete) {
                    newContent += line + "\n"; // Mantém a entrada que não deve ser excluída
                }
                start = end + 1;
            }

            // Abre o arquivo de redes Wi-Fi para escrita
            file = SPIFFS.open("/wifiredes.txt", FILE_WRITE);
            if (!file) {
                request->send(500, "text/plain", "Erro ao abrir o arquivo para escrita"); // Responde com erro se não conseguir abrir o arquivo
                return;
            }
            file.print(newContent); // Escreve o conteúdo atualizado no arquivo
            file.close(); // Fecha o arquivo após escrita

            request->redirect("/wifi-management"); // Redireciona para a página de gerenciamento de Wi-Fi
        } else {
            request->send(400, "text/plain", "SSID ausente."); // Responde com erro se o SSID estiver ausente
        }
    });
}
