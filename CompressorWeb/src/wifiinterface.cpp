const char *getWiFiManagementPage()
{
    return R"rawliteral(
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
        <form id="save-form" action="/salvarwifi" method="post">
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
            fetch('/listadewifi') // Faz uma solicitação GET para a rota '/listadewifi'
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
                            savedNetworks.innerHTML += '<p><strong>SSID:</strong> ' + parts[0] + ' <a class="btn btn-custom-danger" href="/excluirwifi?ssid=' + parts[0] + '">Delete</a></p>';
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

        // -------------------------------------------------------------------------
        // Intercepta o envio do formulário para exibir a mensagem "Rede conectada"
        // -------------------------------------------------------------------------
        document.getElementById('save-form').addEventListener('submit', function(event) {
            event.preventDefault(); // Impede o envio padrão do formulário
            var form = this;
            var formData = new FormData(form);

            fetch(form.action, {
                method: form.method,
                body: formData
            })
            .then(response => {
                if (!response.ok) {
                    throw new Error('Network response was not ok');
                }
                return response.text(); // Converte a resposta em texto
            })
            .then(data => {
                var message = document.getElementById('message');
                message.className = 'alert alert-success'; // Define a classe CSS para o alerta de sucesso
                message.textContent = 'Rede conectada'; // Define a mensagem de sucesso
                form.reset(); // Limpa os campos do formulário
                fetchSavedNetworks(); // Atualiza a lista de redes salvas
            })
            .catch(error => {
                console.error('Erro ao conectar na  rede:', error); // Log de erros no console
                var message = document.getElementById('message');
                message.className = 'alert alert-danger'; // Define a classe CSS para o alerta de erro
                message.textContent = 'Erro ao conectar na  rede'; // Define a mensagem de erro
            });
        });
    </script>
</body>
</html>
    )rawliteral";
}
