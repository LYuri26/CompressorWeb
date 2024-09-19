// -------------------------------------------------------------------------
// Função que retorna o conteúdo HTML para a página de gerenciamento Wi-Fi
// -------------------------------------------------------------------------
const char *getWiFiManagementPage()
{
    // Retorna um literal de string em C++, que contém o HTML, CSS e JavaScript
    // A função retorna um ponteiro constante para char, que aponta para a string HTML.
    // -------------------------------------------------------------------------
    // Início do conteúdo HTML
    // -------------------------------------------------------------------------
    return R"rawliteral(
<!DOCTYPE html>
<html lang="pt-br">

    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Gerenciamento Wi-Fi</title>
        <style>
            /* Estilos gerais da página */
            body {
                font-family: Arial, sans-serif;
                background-color: #f4f4f4;
                margin: 0;
                padding: 0;
                display: flex;
                flex-direction: column;
                min-height: 100vh;
                color: #333;
                justify-content: center;
                align-items: center;
            }
    
            /* Modo noturno */
            body.night-mode {
                background-color: #121212;
                color: #c7c7c7;
            }
    
            /* Modo alto contraste */
            body.high-contrast {
                background-color: #000;
                color: #ffffff;
            }
    
            .container {
                background-color: #ffffff;
                padding: 20px;
                border-radius: 8px;
                box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
                width: 100%;
                max-width: 600px;
                margin: 20px;
                text-align: center;
                flex-grow: 1;
            }
    
            /* Estilos para o container em modo noturno */
            body.night-mode .container {
                background-color: #292929;
                color: #e0e0e0;
            }
    
            /* Estilos para o container em modo alto contraste */
            body.high-contrast .container {
                background-color: #000000;
                color: #ffffff;
            }
    
            .footer {
                background-color: #007bff;
                color: white;
                text-align: center;
                padding: 10px 0;
                font-size: 14px;
                width: 100%;
                margin-top: auto;
            }
    
            /* Estilos para o footer em modo noturno */
            body.night-mode .footer {
                background-color: #1e1e1e;
                color: #e0e0e0;
            }
    
            /* Estilos para o footer em modo alto contraste */
            body.high-contrast .footer {
                background-color: #000;
                color: #ffffff;
            }
    
            /* Mensagens de status */
            #message {
                margin: 10px 0;
                font-size: 16px;
            }
    
            .alert {
                padding: 15px;
                border-radius: 4px;
                margin-top: 10px;
                display: inline-block;
            }
    
            /* Estilos para alertas de sucesso e erro */
            .alert-success {
                background-color: #d4edda;
                color: #155724;
                border: 1px solid #c3e6cb;
            }
    
            .alert-danger {
                background-color: #f8d7da;
                color: #721c24;
                border: 1px solid #f5c6cb;
            }
    
            /* Campos de formulário */
            .form-group {
                margin-bottom: 15px;
            }
    
            .form-group label {
                display: block;
                margin-bottom: 5px;
                font-weight: bold;
            }
    
            .form-group input {
                width: 96.5%;
                padding: 10px;
                border: 1px solid #ddd;
                border-radius: 4px;
            }
    
            /* Estilos para o fundo dos inputs em modo noturno e alto contraste */
            body.night-mode .form-group input,
            body.high-contrast .form-group input {
                background-color: #333;
                color: #fff;
                border-color: #555;
            }
    
            /* Botões com cores diferenciadas */
            .btn {
                display: inline-block;
                padding: 10px 20px;
                border: none;
                border-radius: 4px;
                color: white;
                text-align: center;
                font-size: 16px;
                cursor: pointer;
                margin: 5px;
                text-decoration: none;
                transition: background-color 0.3s, transform 0.3s;
            }
    
            /* Cores exclusivas para cada botão */
            .btn-success {
                background-color: #28a745; /* Verde */
            }
    
            .btn-success:hover {
                background-color: #218838;
            }
    
            .btn-blue {
                background-color: #007bff; /* Azul */
            }
    
            .btn-blue:hover {
                background-color: #0056b3;
            }
    
            .btn-custom-danger {
                background-color: #dc3545; /* Vermelho */
            }
    
            .btn-custom-danger:hover {
                background-color: #c82333;
            }
    
            .btn-night-mode {
                background-color: #e83e8c; /* Rosa */
            }
    
            .btn-night-mode:hover {
                background-color: #d6336c;
            }
    
            .btn-contrast-mode {
                background-color: #6f42c1; /* Roxo */
            }
    
            .btn-contrast-mode:hover {
                background-color: #5a36a0;
            }
    
            /* Grupo de inputs */
            .input-group {
                display: flex;
                align-items: center;
                justify-content: space-between;
            }
    
            .input-group-append {
                margin-left: -1px;
            }
    
            .input-group-append button {
                padding: 10px;
                border: 1px solid #ddd;
                border-radius: 4px;
                background-color: #f4f4f4;
                cursor: pointer;
            }
    
            .input-group-append button:hover {
                background-color: #e2e2e2;
            }
    
            /* Redes salvas */
            #saved-networks {
                max-height: 300px;
                overflow-y: auto;
                margin-top: 20px;
                text-align: left;
            }
        </style>
    </head>

    <body>
        <div class="container" aria-label="Gerenciamento de Redes Wi-Fi">
            <h2>Gerenciamento de Redes Wi-Fi</h2>

            <div id="message" class="alert" role="alert"></div>

            <p id="connected-ip" aria-live="polite">Aguardando IP...</p>

            <form id="save-form" action="/salvarwifi" method="post"
                aria-label="Formulário de rede Wi-Fi">
                <div class="form-group">
                    <label for="ssid">SSID da Rede Wi-Fi:</label>
                    <input type="text" id="ssid" name="ssid"
                        placeholder="Digite o SSID" required
                        aria-required="true">
                </div>

                <div class="form-group">
                    <label for="password">Senha:</label>
                    <div class="input-group">
                        <input type="password" id="password" name="password"
                            placeholder="Digite a senha" required
                            aria-required="true">
                        <div class="input-group-append">
                            <button type="button" id="toggle-password"
                                aria-label="Mostrar senha">Mostrar</button>
                        </div>
                    </div>
                </div>

                <button type="submit" class="btn btn-success"
                    aria-label="Conectar">Conectar</button>
                <button type="button" onclick="window.history.back()"
                    class="btn btn-blue" aria-label="Voltar">Voltar</button>
                <!-- Botões de alternância para modo noturno e alto contraste -->
                <button id="toggle-night-mode" class="btn btn-night-mode"
                    aria-label="Alternar modo noturno">Modo Noturno</button>
                <button id="toggle-contrast-mode" class="btn btn-contrast-mode"
                    aria-label="Alternar modo alto contraste">Modo Alto
                    Contraste</button>
            </form>

            <hr>

            <div id="saved-networks" aria-label="Redes Wi-Fi Salvas">Aguardando
                redes salvas...</div>
        </div>

        <div class="footer" aria-label="Rodapé">
            <p>Aplicação desenvolvida pela Turma de Informática Para Internet
                Trilhas de Futuro 2024</p>
            <p>Instrutor: Lenon Yuri</p>
        </div>
        <script>
        // Função para buscar as redes Wi-Fi salvas e exibir na página
        function fetchSavedNetworks() {
            // Faz uma requisição para o servidor na rota /listadewifi
            fetch('/listadewifi')
                .then(response => {
                    // Verifica se a resposta é válida
                    if (!response.ok) {
                        throw new Error('Network response was not ok');
                    }
                    // Retorna o texto da resposta (lista de redes Wi-Fi salvas)
                    return response.text();
                })
                .then(data => {
                    // Obtém o contêiner para exibir as redes salvas
                    var savedNetworks = document.getElementById('saved-networks');
                    savedNetworks.innerHTML = '';
                    // Divide os dados recebidos por linhas (cada linha representa uma rede)
                    var networks = data.trim().split('\n');
                    // Verifica se há redes salvas
                    if (networks.length > 0 && networks[0] !== "") {
                        // Para cada rede salva, adiciona um elemento HTML com o SSID e um botão para excluir
                        networks.forEach(network => {
                            var parts = network.split(',');
                            savedNetworks.innerHTML += '<p><strong>SSID:</strong> ' + parts[0] + ' <a class="btn btn-custom-danger" href="/excluirwifi?ssid=' + encodeURIComponent(parts[0]) + '">Excluir</a></p>';
                        });
                    } else {
                        // Se não houver redes salvas, exibe uma mensagem apropriada
                        savedNetworks.innerHTML = '<p>Nenhuma rede salva encontrada.</p>';
                    }
                })
                .catch(error => {
                    // Lida com erros de requisição
                    console.error('Erro ao buscar as redes salvas:', error);
                    var savedNetworks = document.getElementById('saved-networks');
                    savedNetworks.innerHTML = '<p>Erro ao buscar redes salvas.</p>';
                });
        }
    
        // Função para buscar o IP da rede conectada e atualizar na página
        function fetchConnectedNetworkInfo() {
            // Faz uma requisição para o servidor na rota /ipdarede
            fetch('/ipdarede')
                .then(response => response.json())
                .then(data => {
                    // Atualiza o parágrafo com o IP da rede conectada
                    document.getElementById('connected-ip').innerText = `IP da Rede Conectada: ${data.ip}`;
                })
                .catch(error => console.error('Erro ao carregar informações da rede conectada:', error));
        }
    
        // Inicialização ao carregar a página
        document.addEventListener('DOMContentLoaded', function() {
            // Chama as funções para buscar as redes salvas e as informações da rede conectada
            fetchSavedNetworks();
            fetchConnectedNetworkInfo();
        });
    
        // Alterna a visibilidade da senha no campo de input
        document.getElementById('toggle-password').addEventListener('click', function() {
            var passwordField = document.getElementById('password');
            var button = this;
            // Verifica se a senha está oculta e alterna entre mostrar e ocultar
            if (passwordField.type === 'password') {
                passwordField.type = 'text';
                button.textContent = 'Ocultar';
            } else {
                passwordField.type = 'password';
                button.textContent = 'Mostrar';
            }
        });
    
        // Lida com a submissão do formulário para salvar as credenciais de Wi-Fi
        document.getElementById('save-form').addEventListener('submit', function(event) {
            event.preventDefault();
            var form = this;
            var formData = new FormData(form);
    
            // Envia os dados do formulário para o servidor usando a rota /salvarwifi
            fetch(form.action, {
                method: form.method,
                body: formData
            })
            .then(response => {
                // Verifica se a resposta é válida
                if (!response.ok) {
                    throw new Error('Network response was not ok');
                }
                // Retorna o texto da resposta
                return response.text();
            })
            .then(data => {
                // Exibe uma mensagem de sucesso e atualiza as redes salvas
                var message = document.getElementById('message');
                message.className = 'alert alert-success';
                message.textContent = 'Rede conectada';
                form.reset();
                fetchSavedNetworks();
                fetchConnectedNetworkInfo();
            })
            .catch(error => {
                // Exibe uma mensagem de erro em caso de falha na conexão
                console.error('Erro ao conectar na rede:', error);
                var message = document.getElementById('message');
                message.className = 'alert alert-danger';
                message.textContent = 'Erro ao conectar na rede';
            });
        });
    
        // Funções para alternar entre modo noturno e modo alto contraste
        function toggleNightMode() {
            document.body.classList.toggle('night-mode');
        }
    
        function toggleHighContrastMode() {
            document.body.classList.toggle('high-contrast');
        }
    
        // Modo noturno
        document.getElementById('toggle-night-mode').addEventListener('click', toggleNightMode);
        // Modo alto contraste
        document.getElementById('toggle-contrast-mode').addEventListener('click', toggleHighContrastMode);
    
    </script>
        <!-- Fim do código JavaScript -->
    </body>
    <!-- Fim do corpo do documento HTML -->

</html>
<!-- Fim do documento HTML -->
    )rawliteral";
    // Finaliza o literal de string retornado pela função em C++
}
