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
    <!-- Define o tipo do documento como HTML5 -->

    <html lang="pt-br">
    <!-- Início do documento HTML e definição do idioma como português do Brasil -->

    <head>
        <!-- Cabeçalho do documento HTML, onde se define metadados, estilos e título -->
        <meta charset="UTF-8">
        <!-- Define a codificação de caracteres como UTF-8, suportando caracteres especiais -->

        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <!-- Configura a viewport para dispositivos móveis, garantindo a responsividade -->

        <title>Gerenciamento Wi-Fi</title>
        <!-- Define o título da página que aparecerá na aba do navegador -->

        <style>
            /* Estilos gerais da página */
            body {
                font-family: Arial, sans-serif;
                /* Define a fonte padrão como Arial, com fallback para sans-serif */

                background-color: #f4f4f4;
                /* Define a cor de fundo da página como um cinza claro */

                margin: 0;
                /* Remove a margem padrão do corpo da página */

                padding: 0;
                /* Remove o preenchimento padrão do corpo da página */

                display: flex;
                /* Usa Flexbox para o layout, permitindo centralizar o conteúdo */

                flex-direction: column;
                /* Define a direção do flex como coluna, empilhando elementos verticalmente */

                min-height: 100vh;
                /* Define a altura mínima da página como 100% da altura da viewport */

                color: #333;
                /* Define a cor do texto como um cinza escuro */

                justify-content: center;
                /* Centraliza o conteúdo horizontalmente */

                align-items: center;
                /* Centraliza o conteúdo verticalmente */
            }

            /* Estilos da div container que envolve todo o conteúdo principal */
            .container {
                background-color: #ffffff;
                /* Define a cor de fundo do contêiner como branco */

                padding: 20px;
                /* Adiciona 20px de preenchimento interno ao contêiner */

                border-radius: 8px;
                /* Adiciona bordas arredondadas ao contêiner */

                box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
                /* Aplica uma sombra sutil ao contêiner, criando um efeito de profundidade */

                width: 100%;
                /* Define a largura do contêiner para 100% da largura disponível */

                max-width: 600px;
                /* Define a largura máxima do contêiner como 600px */

                margin: 20px;
                /* Adiciona uma margem externa de 20px ao redor do contêiner */

                text-align: center;
                /* Centraliza o texto dentro do contêiner */

                flex-grow: 1;
                /* Permite que o contêiner cresça para ocupar o espaço disponível */
            }

            /* Estilos para o rodapé */
            .footer {
                background-color: #007bff;
                /* Define a cor de fundo do rodapé como azul */

                color: white;
                /* Define a cor do texto do rodapé como branco */

                text-align: center;
                /* Centraliza o texto dentro do rodapé */

                padding: 10px 0;
                /* Adiciona 10px de preenchimento vertical no rodapé, sem preenchimento horizontal */

                font-size: 14px;
                /* Define o tamanho da fonte do texto do rodapé como 14px */

                width: 100%;
                /* Define a largura do rodapé como 100% da largura disponível */

                margin-top: auto;
                /* Empurra o rodapé para o final da página, ocupando o espaço restante */
            }

            /* Estilos para mensagens de status */
            #message {
                margin: 10px 0;
                /* Adiciona 10px de margem vertical ao redor da mensagem de status */

                font-size: 16px;
                /* Define o tamanho da fonte da mensagem de status como 16px */
            }

            .alert {
                padding: 15px;
                /* Adiciona 15px de preenchimento interno à mensagem de alerta */

                border-radius: 4px;
                /* Adiciona bordas arredondadas à mensagem de alerta */

                margin-top: 10px;
                /* Adiciona 10px de margem superior à mensagem de alerta */

                display: inline-block;
                /* Define a mensagem de alerta como um bloco inline */
            }

            .alert-success {
                background-color: #d4edda;
                /* Define a cor de fundo da mensagem de sucesso como verde claro */

                color: #155724;
                /* Define a cor do texto da mensagem de sucesso como verde escuro */

                border: 1px solid #c3e6cb;
                /* Adiciona uma borda verde clara à mensagem de sucesso */
            }

            .alert-danger {
                background-color: #f8d7da;
                /* Define a cor de fundo da mensagem de erro como vermelho claro */

                color: #721c24;
                /* Define a cor do texto da mensagem de erro como vermelho escuro */

                border: 1px solid #f5c6cb;
                /* Adiciona uma borda vermelha clara à mensagem de erro */
            }

            /* Estilos para os campos de formulário */
            .form-group {
                margin-bottom: 15px;
                /* Adiciona 15px de margem inferior aos grupos de formulário */
            }

            .form-group label {
                display: block;
                /* Define o label como um bloco, ocupando toda a largura disponível */

                margin-bottom: 5px;
                /* Adiciona 5px de margem inferior ao label */

                font-weight: bold;
                /* Define o peso da fonte como negrito */
            }

            .form-group input {
                width: 96.5%;
                /* Define a largura do input como 96.5% da largura do seu contêiner */

                padding: 10px;
                /* Adiciona 10px de preenchimento interno ao input */

                border: 1px solid #ddd;
                /* Define a borda do input como cinza claro */

                border-radius: 4px;
                /* Adiciona bordas arredondadas ao input */
            }

            /* Estilos para botões */
            .btn {
                display: inline-block;
                /* Define o botão como um bloco inline, permitindo que seja estilizado */

                padding: 10px 20px;
                /* Adiciona 10px de preenchimento vertical e 20px horizontal ao botão */

                border: none;
                /* Remove qualquer borda padrão do botão */

                border-radius: 4px;
                /* Adiciona bordas arredondadas ao botão */

                color: white;
                /* Define a cor do texto do botão como branco */

                text-align: center;
                /* Centraliza o texto dentro do botão */

                font-size: 16px;
                /* Define o tamanho da fonte do texto do botão como 16px */

                cursor: pointer;
                /* Define o cursor como uma mãozinha ao passar o mouse sobre o botão */

                text-decoration: none;
                /* Remove a sublinhado do texto do botão */

                margin: 5px;
                /* Adiciona 5px de margem ao redor do botão */
            }

            .btn-success {
                background-color: #28a745;
                /* Define a cor de fundo do botão de sucesso como verde */
            }

            .btn-success:hover {
                background-color: #218838;
                /* Altera a cor de fundo do botão de sucesso ao passar o mouse */
            }

            .btn-blue {
                background-color: #007bff;
                /* Define a cor de fundo do botão azul */
            }

            .btn-blue:hover {
                background-color: #0056b3;
                /* Altera a cor de fundo do botão azul ao passar o mouse */
            }

            .btn-custom-danger {
                background-color: #dc3545;
                /* Define a cor de fundo do botão de perigo personalizado como vermelho */
            }

            .btn-custom-danger:hover {
                background-color: #c82333;
                /* Altera a cor de fundo do botão de perigo personalizado ao passar o mouse */
            }

            /* Estilos para o grupo de inputs */
            .input-group {
                display: flex;
                /* Define um layout flexível para o grupo de inputs */

                align-items: center;
                /* Alinha os itens do grupo de inputs ao centro verticalmente */

                justify-content: space-between;
                /* Distribui os itens do grupo de inputs com espaço entre eles */
            }

            .input-group-append {
                margin-left: -1px;
                /* Adiciona uma margem esquerda negativa para colar o botão ao input */
            }

            .input-group-append button {
                padding: 10px;
                /* Adiciona 10px de preenchimento ao botão do grupo de inputs */

                border: 1px solid #ddd;
                /* Define uma borda cinza clara ao botão do grupo de inputs */

                border-radius: 4px;
                /* Adiciona bordas arredondadas ao botão do grupo de inputs */

                background-color: #f4f4f4;
                /* Define a cor de fundo do botão do grupo de inputs como cinza claro */

                cursor: pointer;
                /* Define o cursor como uma mãozinha ao passar o mouse sobre o botão */
            }

            .input-group-append button:hover {
                background-color: #e2e2e2;
                /* Altera a cor de fundo do botão do grupo de inputs ao passar o mouse */
            }

            /* Estilos para a lista de redes salvas */
            #saved-networks {
                max-height: 300px;
                /* Define a altura máxima do contêiner de redes salvas como 300px */

                overflow-y: auto;
                /* Adiciona uma barra de rolagem vertical se o conteúdo exceder a altura máxima */

                margin-top: 20px;
                /* Adiciona 20px de margem superior ao contêiner de redes salvas */

                text-align: left;
                /* Alinha o texto do contêiner de redes salvas à esquerda */
            }
        </style>
        <!-- Fim dos estilos CSS definidos na tag <style> -->
    </head>
    <!-- Fim do cabeçalho HTML -->

    <body>
        <!-- Início do corpo do documento HTML -->
        <div class="container">
            <!-- Início do contêiner principal que envolve todo o conteúdo -->
            <h2>Gerenciamento de Redes Wi-Fi</h2>
            <!-- Título principal da página -->

            <div id="message" class="alert"></div>
            <!-- Contêiner para exibir mensagens de status ao usuário, inicialmente vazio -->

            <p id="connected-ip">Aguardando IP...</p>
            <!-- Parágrafo que exibe o IP da rede conectada, inicialmente com texto padrão -->

            <form id="save-form" action="/salvarwifi" method="post">
                <!-- Início do formulário para salvar as credenciais Wi-Fi, utilizando o método POST -->
                <div class="form-group">
                    <!-- Grupo de formulário para o campo SSID -->
                    <label for="ssid">SSID da Rede Wi-Fi:</label>
                    <!-- Rótulo para o campo SSID -->

                    <input type="text" id="ssid" name="ssid" placeholder="Digite o SSID" required>
                    <!-- Campo de input para o SSID, obrigatório para submissão -->
                </div>

                <div class="form-group">
                    <!-- Grupo de formulário para o campo de senha -->
                    <label for="password">Senha:</label>
                    <!-- Rótulo para o campo de senha -->

                    <div class="input-group">
                        <!-- Grupo de inputs que contém o campo de senha e o botão de visibilidade -->
                        <input type="password" id="password" name="password" placeholder="Digite a senha" required>
                        <!-- Campo de input para a senha, com o tipo definido como "password" para esconder o texto -->

                        <div class="input-group-append">
                            <!-- Contêiner para o botão de alternar a visibilidade da senha -->
                            <button type="button" id="toggle-password">Mostrar</button>
                            <!-- Botão para alternar a visibilidade da senha -->
                        </div>
                    </div>
                </div>

                <button type="submit" class="btn btn-success">Conectar</button>
                <!-- Botão para submeter o formulário e conectar à rede Wi-Fi -->

                <button type="button" onclick="window.history.back()" class="btn btn-blue">Voltar</button>
                <!-- Botão para voltar à página anterior, usando o histórico do navegador -->
            </form>
            <!-- Fim do formulário de salvamento de Wi-Fi -->

            <hr>
            <!-- Linha horizontal para separar seções na página -->

            <div id="saved-networks">Aguardando redes salvas...</div>
            <!-- Contêiner que exibirá as redes Wi-Fi salvas, inicialmente com texto padrão -->
        </div>
        <!-- Fim do contêiner principal -->

        <div class="footer">
            <!-- Início do rodapé -->
            <p>Aplicação desenvolvida pela Turma de Informática Para Internet Trilhas de Futuro 2024</p>
            <!-- Texto informativo sobre a aplicação -->

            <p>Instrutor: Lenon Yuri</p>
            <!-- Texto informativo sobre o instrutor -->
        </div>
        <!-- Fim do rodapé -->

        <script>
            // Início do código JavaScript

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
        </script>
        <!-- Fim do código JavaScript -->
    </body>
    <!-- Fim do corpo do documento HTML -->

    </html>
    <!-- Fim do documento HTML -->
    )rawliteral";
    // Finaliza o literal de string retornado pela função em C++
}
