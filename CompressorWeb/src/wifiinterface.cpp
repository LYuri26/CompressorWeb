// -------------------------------------------------------------------------
// Função que retorna o código HTML da página de gerenciamento Wi-Fi
// -------------------------------------------------------------------------
const char *getWiFiManagementPage()
{
    // -------------------------------------------------------------------------
    // Utilização de R"rawliteral" para facilitar a inclusão de texto multiline
    // contendo HTML, CSS e JavaScript dentro do código C++
    // -------------------------------------------------------------------------
    return R"rawliteral(
    <!DOCTYPE html> <!-- Declaração do tipo de documento HTML5 -->
    <html lang="pt-br"> <!-- Início do documento HTML, idioma definido como português do Brasil -->
    <head> <!-- Cabeçalho do documento contendo meta informações e links para recursos externos -->
        <meta charset="UTF-8"> <!-- Define a codificação de caracteres como UTF-8 -->
        <meta name="viewport" content="width=device-width, initial-scale=1.0"> <!-- Define o viewport para responsividade em dispositivos móveis -->
        <title>Gerenciamento Wi-Fi</title> <!-- Título da página exibido na aba do navegador -->
        <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css"> <!-- Link para o CSS do Bootstrap para estilização rápida -->
        <style> <!-- Início da seção de estilos personalizados CSS -->
            /* Estilos gerais da página */
            body {
                font-family: Arial, sans-serif; /* Define a fonte padrão como Arial */
                background-color: #f8f9fa; /* Cor de fundo da página */
                margin: 0; /* Remove margens padrão */
                padding: 0; /* Remove preenchimentos padrão */
                display: flex; /* Define a exibição do body como flexbox */
                flex-direction: column; /* Define a direção dos itens flexíveis como coluna */
                min-height: 100vh; /* Define a altura mínima como 100% da altura da janela de visualização */
            }
            /* Estilos da div container que envolve todo o conteúdo principal */
            .container {
                background-color: #ffffff; /* Define a cor de fundo como branco */
                padding: 20px; /* Adiciona preenchimento interno de 20px */
                border-radius: 5px; /* Define bordas arredondadas com raio de 5px */
                box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); /* Adiciona sombra ao redor da caixa */
                width: 100%; /* Define a largura como 100% do container pai */
                max-width: 600px; /* Define a largura máxima como 600px */
                margin: 30px auto; /* Centraliza horizontalmente e adiciona margem superior de 30px */
                flex: 1; /* Permite que o container cresça para ocupar o espaço disponível */
            }
            /* Estilos para o rodapé */
            .footer {
                background-color: #007bff; /* Define a cor de fundo como azul */
                color: white; /* Define a cor do texto como branca */
                text-align: center; /* Centraliza o texto */
                padding: 10px 0; /* Adiciona preenchimento superior e inferior de 10px */
                font-size: 14px; /* Define o tamanho da fonte como 14px */
                margin-top: 30px; /* Adiciona margem superior de 30px */
            }
            /* Estilos para a seção que lista as redes Wi-Fi salvas */
            #saved-networks {
                max-height: 300px; /* Define a altura máxima como 300px */
                overflow-y: auto; /* Adiciona barra de rolagem vertical se necessário */
                margin-bottom: 20px; /* Adiciona margem inferior de 20px */
            }
            /* Estilos para o botão de conexão Wi-Fi */
            .btn-success {
                background-color: #28a745; /* Define a cor de fundo como verde */
                color: white; /* Define a cor do texto como branca */
            }
            .btn-success:hover {
                background-color: #218838; /* Define a cor de fundo ao passar o mouse */
            }
            /* Estilos para o botão de voltar */
            .btn-blue {
                background-color: #007bff; /* Define a cor de fundo como azul */
                color: white; /* Define a cor do texto como branca */
                border: none; /* Remove a borda padrão */
                padding: 10px 20px; /* Adiciona preenchimento interno de 10px nas laterais e 20px superior/inferior */
                font-size: 16px; /* Define o tamanho da fonte como 16px */
                cursor: pointer; /* Muda o cursor para pointer ao passar sobre o botão */
                border-radius: 3px; /* Define bordas arredondadas com raio de 3px */
                width: 100%; /* Define a largura como 100% do container pai */
            }
            .btn-blue:hover {
                background-color: #0056b3; /* Define a cor de fundo ao passar o mouse */
            }
            /* Estilos para o botão de exclusão personalizado */
            .btn-custom-danger {
                background-color: #dc3545; /* Define a cor de fundo como vermelho */
                color: white; /* Define a cor do texto como branca */
                border: none; /* Remove a borda padrão */
                padding: 5px 10px; /* Adiciona preenchimento interno de 5px nas laterais e 10px superior/inferior */
                font-size: 14px; /* Define o tamanho da fonte como 14px */
                cursor: pointer; /* Muda o cursor para pointer ao passar sobre o botão */
                border-radius: 3px; /* Define bordas arredondadas com raio de 3px */
                text-decoration: none; /* Remove a decoração padrão (sublinhado) do link */
            }
            .btn-custom-danger:hover {
                background-color: #c82333; /* Define a cor de fundo ao passar o mouse */
            }
            /* Estilos para o grupo de inputs */
            .input-group {
                display: flex; /* Define o grupo de inputs como flexbox */
                align-items: center; /* Alinha os itens no centro verticalmente */
            }
            /* Estilos para o botão que aparece ao lado do campo de senha */
            .input-group-append {
                margin-left: -1px; /* Remove a margem à esquerda para alinhar o botão com o campo de input */
            }
        </style> <!-- Fim da seção de estilos personalizados -->
    </head>
    <body> <!-- Início do corpo do documento HTML -->
        <div class="container"> <!-- Início do container principal -->
            <h2>Gerenciamento de Redes Wi-Fi</h2> <!-- Título principal da página -->
            <div id="message" class="alert" role="alert"></div> <!-- Div para exibir mensagens de sucesso ou erro -->
            <p id="connected-ip">Aguardando IP...</p> <!-- Parágrafo para exibir o IP da rede conectada -->
            <form id="save-form" action="/salvarwifi" method="post"> <!-- Formulário para salvar redes Wi-Fi -->
                <div class="form-group"> <!-- Grupo do campo SSID -->
                    <label for="ssid">SSID da Rede Wi-Fi:</label> <!-- Rótulo para o campo SSID -->
                    <input type="text" id="ssid" name="ssid" class="form-control" placeholder="Digite o SSID" required> <!-- Campo de input para SSID -->
                </div>
                <div class="form-group"> <!-- Grupo do campo Senha -->
                    <label for="password">Senha:</label> <!-- Rótulo para o campo Senha -->
                    <div class="input-group"> <!-- Grupo de input que contém o campo de senha e o botão de alternância -->
                        <input type="password" id="password" name="password" class="form-control" placeholder="Digite a senha" required> <!-- Campo de input para a senha -->
                        <div class="input-group-append"> <!-- Div que agrupa o botão de alternância de visibilidade da senha -->
                            <button class="btn btn-outline-secondary" type="button" id="toggle-password">Mostrar</button> <!-- Botão para alternar visibilidade da senha -->
                        </div>
                    </div>
                </div>
                <button type="submit" class="btn btn-success btn-block">Conectar</button> <!-- Botão para enviar o formulário -->
                <button type="button" onclick="window.history.back()" class="btn-blue">Voltar</button> <!-- Botão para voltar à página anterior -->
            </form>
            <hr> <!-- Linha horizontal para separar seções da página -->
            <div id="saved-networks">Aguardando redes salvas...</div> <!-- Div que conterá a lista de redes Wi-Fi salvas -->
        </div>
        <div class="footer"> <!-- Início do rodapé da página -->
            <p>Aplicação desenvolvida pela Turma de Informática Para Internet Trilhas de Futuro 2024</p> <!-- Texto no rodapé -->
            <p>Instrutor: Lenon Yuri</p> <!-- Texto no rodapé -->
        </div>
        <script> <!-- Início da seção de scripts JavaScript -->
            // -------------------------------------------------------------------------
            // Funções JavaScript para manipulação de dados da página
            // -------------------------------------------------------------------------

            // Função para buscar as redes Wi-Fi salvas e exibir na página
            function fetchSavedNetworks() {
                fetch('/listadewifi') // Faz uma requisição GET ao endpoint /listadewifi
                    .then(response => {
                        if (!response.ok) { // Verifica se a resposta da requisição foi bem-sucedida
                            throw new Error('Network response was not ok'); // Lança um erro se a resposta não foi OK
                        }
                        return response.text(); // Retorna a resposta como texto
                    })
                    .then(data => {
                        var savedNetworks = document.getElementById('saved-networks'); // Seleciona o elemento onde as redes salvas serão exibidas
                        savedNetworks.innerHTML = ''; // Limpa o conteúdo atual da div
                        var networks = data.trim().split('\n'); // Divide os dados recebidos em uma lista de redes, cada linha representa uma rede
                        if (networks.length > 0 && networks[0] !== "") { // Verifica se há redes salvas e se a primeira linha não está vazia
                            networks.forEach(network => { // Itera sobre cada rede salva
                                var parts = network.split(','); // Divide cada linha em partes separadas por vírgula (ex: SSID, outra informação)
                                savedNetworks.innerHTML += '<p><strong>SSID:</strong> ' + parts[0] + ' <a class="btn btn-custom-danger" href="/excluirwifi?ssid=' + parts[0] + '">Delete</a></p>'; // Adiciona o SSID da rede e um link para excluir a rede
                            });
                        } else {
                            savedNetworks.innerHTML = '<p>Nenhuma rede salva encontrada.</p>'; // Exibe mensagem caso não haja redes salvas
                        }
                    })
                    .catch(error => { // Lida com erros na requisição ou no processamento dos dados
                        console.error('Erro ao buscar as redes salvas:', error); // Exibe o erro no console
                        var savedNetworks = document.getElementById('saved-networks'); // Seleciona novamente o elemento de redes salvas
                        savedNetworks.innerHTML = '<p>Erro ao buscar redes salvas.</p>'; // Exibe mensagem de erro na página
                    });
            }

            // Função para buscar o IP da rede conectada e atualizar na página
            function fetchConnectedNetworkInfo() {
                fetch('/ipdarede') // Faz uma requisição GET ao endpoint /ipdarede
                    .then(response => response.json()) // Converte a resposta em JSON
                    .then(data => {
                        document.getElementById('connected-ip').innerText = `IP da Rede Conectada: ${data.ip}`; // Atualiza o elemento na página com o IP retornado
                    })
                    .catch(error => console.error('Erro ao carregar informações da rede conectada:', error)); // Lida com erros na requisição ou no processamento dos dados
            }

            // -------------------------------------------------------------------------
            // Inicialização ao carregar a página
            // -------------------------------------------------------------------------
            document.addEventListener('DOMContentLoaded', function() {
                fetchSavedNetworks(); // Carrega as redes salvas quando a página é carregada
                fetchConnectedNetworkInfo(); // Carrega informações da rede conectada quando a página é carregada
            });

            // Alterna a visibilidade da senha no campo de input
            document.getElementById('toggle-password').addEventListener('click', function() {
                var passwordField = document.getElementById('password'); // Seleciona o campo de senha
                var button = this; // Armazena a referência ao botão
                if (passwordField.type === 'password') { // Verifica se o campo de senha está do tipo 'password'
                    passwordField.type = 'text'; // Altera o campo para tipo 'text' para exibir a senha
                    button.textContent = 'Ocultar'; // Altera o texto do botão para 'Ocultar'
                } else {
                    passwordField.type = 'password'; // Altera o campo para tipo 'password' para esconder a senha
                    button.textContent = 'Mostrar'; // Altera o texto do botão para 'Mostrar'
                }
            });

            // Lida com a submissão do formulário para salvar as credenciais de Wi-Fi
            document.getElementById('save-form').addEventListener('submit', function(event) {
                event.preventDefault(); // Evita que o formulário seja enviado da maneira padrão
                var form = this; // Armazena a referência ao formulário
                var formData = new FormData(form); // Cria um objeto FormData com os dados do formulário

                fetch(form.action, {
                    method: form.method, // Utiliza o método definido no formulário (POST)
                    body: formData // Envia os dados do formulário
                })
                .then(response => {
                    if (!response.ok) { // Verifica se a resposta da requisição foi bem-sucedida
                        throw new Error('Network response was not ok'); // Lança um erro se a resposta não foi OK
                    }
                    return response.text(); // Retorna a resposta como texto
                })
                .then(data => {
                    var message = document.getElementById('message'); // Seleciona o elemento de mensagem
                    message.className = 'alert alert-success'; // Define a classe do elemento para exibir uma mensagem de sucesso
                    message.textContent = 'Rede conectada'; // Define o conteúdo da mensagem
                    form.reset(); // Reseta o formulário após submissão bem-sucedida
                    fetchSavedNetworks(); // Atualiza a lista de redes salvas
                    fetchConnectedNetworkInfo(); // Atualiza o elemento IP com as informações da nova rede conectada
                })
                .catch(error => {
                    console.error('Erro ao conectar na rede:', error); // Exibe o erro no console
                    var message = document.getElementById('message'); // Seleciona o elemento de mensagem
                    message.className = 'alert alert-danger'; // Define a classe do elemento para exibir uma mensagem de erro
                    message.textContent = 'Erro ao conectar na rede'; // Define o conteúdo da mensagem de erro
                });
            });
        </script> <!-- Fim da seção de scripts JavaScript -->
    </body> <!-- Fim do corpo do documento HTML -->
    </html> <!-- Fim do documento HTML -->
    )rawliteral"; // Fim do retorno do conteúdo HTML dentro do R"rawliteral"
}
