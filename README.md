# Sistema de Controle de Compressores via ESP32

Este projeto foi desenvolvido para controlar o ligar e desligar de compressores utilizando um ESP32. O sistema possui uma interface web acessível via Wi-Fi, com autenticação e gerenciamento de usuários. Ele opera tanto em modo de **Access Point (AP)** quanto em modo de conexão a redes Wi-Fi locais.

## Desenvolvedores

Este projeto foi criado pelos seguintes alunos:

- **Brenda**
- **Davi**
- **João**
- **Pedro**
- **Sofia**

## Funcionalidades

- **Autenticação de usuários:** Login com validação de credenciais para acessar o painel de controle.
- **Controle de compressores:** Interface para ligar e desligar os compressores remotamente.
- **Monitoramento:** Visualização de dados como pressão, umidade e tempo de uso.
- **Manutenção remota:** Acesso para monitoramento de condições de operação para manutenção.
- **Sistema de erros:** Páginas de erro personalizadas para lidar com situações como perda de conexão.
- **Modo Access Point (AP):** O ESP32 pode criar um ponto de acesso próprio, permitindo que dispositivos se conectem diretamente a ele.
- **Conexão via Wi-Fi:** Também pode se conectar a uma rede Wi-Fi existente, permitindo o controle via rede local.

## Setup de Rede (Access Point)

O ESP32 pode atuar como um Access Point (AP), criando uma rede local própria para controlar os compressores. O ponto de acesso gerado pelo dispositivo tem os seguintes parâmetros:

```cpp
const char *ap_ssid = "EnergizAR"; // SSID do Access Point (nome da rede gerada pelo dispositivo)
const char *ap_password = "12345678";  // Senha do Access Point (senha para se conectar à rede gerada pelo dispositivo)

IPAddress local_ip(192, 168, 26, 7); // IP local do Access Point (endereço IP do Access Point no formato (A, B, C, D))
IPAddress gateway(192, 168, 26, 1);  // Gateway do Access Point (endereço do roteador na rede do Access Point)
IPAddress subnet(255, 255, 255, 0);  // Máscara de sub-rede do Access Point (define a faixa de IPs na rede do Access Point)
```

## Estrutura de Diretórios

Abaixo está a estrutura de diretórios do projeto, com uma visão detalhada dos arquivos principais e suas funções:

```
.pio/              # Diretório gerado automaticamente pelo PlatformIO (não precisa de modificação)
.vscode/           # Arquivos de configuração do VSCode
include/           # Cabeçalhos (.h) do projeto
│   ├── autenticador.h     # Declaração de funções de autenticação de usuário
│   ├── creditos.h         # Declaração das funções da página de créditos
│   ├── dashboard.h        # Declaração das funções da página do painel de controle
│   ├── index.h            # Declaração das funções da página inicial do sistema
│   ├── ligadesliga.h      # Funções para controle de ligar/desligar compressores
│   ├── manutencao.h       # Funções de gerenciamento e monitoramento de manutenção
│   ├── paginaserro.h      # Declaração das funções das páginas de erro
│   ├── pressao.h          # Monitoramento e exibição de dados de pressão
│   ├── tempo.h            # Monitoramento de tempo de operação
│   ├── umidade.h          # Monitoramento de umidade
│   ├── wificonexao.h      # Declaração de funções de conexão Wi-Fi
│   ├── wifigerenciador.h  # Gerenciamento da configuração Wi-Fi
│   └── wifiinterface.h    # Interface de comunicação do Wi-Fi com o sistema
lib/               # Diretório para bibliotecas de terceiros (se houver)
src/               # Código-fonte (.cpp) do projeto
│   ├── autenticador.cpp   # Implementação da autenticação de usuários
│   ├── creditos.cpp       # Implementação da página de créditos
│   ├── dashboard.cpp      # Implementação da página do painel de controle
│   ├── index.cpp          # Arquivo de inicialização da interface web
│   ├── ligadesliga.cpp    # Implementação do controle de ligar/desligar compressores
│   ├── main.cpp           # Arquivo principal do projeto (inicializa o sistema)
│   ├── manutencao.cpp     # Implementação do monitoramento de manutenção
│   ├── paginaserro.cpp    # Implementação das páginas de erro
│   ├── pressao.cpp        # Implementação do monitoramento de pressão
│   ├── tempo.cpp          # Implementação do monitoramento de tempo de operação
│   ├── umidade.cpp        # Implementação do monitoramento de umidade
│   ├── wificonexao.cpp    # Implementação da conexão Wi-Fi
│   ├── wifigerenciador.cpp # Implementação do gerenciamento de redes Wi-Fi
│   └── wifiinterface.cpp  # Implementação da interface de comunicação Wi-Fi
.gitignore          # Arquivo que especifica quais arquivos/diretórios devem ser ignorados pelo Git
platformio.ini      # Arquivo de configuração do PlatformIO
```

## Instalação

1. **Clone o repositório:**
   ```bash
   git clone https://github.com/LYuri26/CompressorWeb.git
   cd controle-compressor
   ```

2. **Instale o [PlatformIO](https://platformio.org/) para compilar o código:**
   - Certifique-se de que você tem o [VSCode](https://code.visualstudio.com/) instalado e o plugin do PlatformIO habilitado.

3. **Configure o ESP32:**
   - Conecte o ESP32 ao seu computador.
   - Edite o arquivo `platformio.ini` se necessário, para ajustar a porta serial e outros parâmetros de compilação.

4. **Compile e envie o código:**
   ```bash
   pio run --target upload
   ```

5. **Conecte-se ao Access Point:**
   - Após a configuração, conecte-se à rede Wi-Fi gerada pelo ESP32 com as credenciais definidas acima (`EnergizAR` / `12345678`).

6. **Acesse o sistema:**
   - Acesse a interface web pelo IP `192.168.26.7` no navegador, onde será solicitado o login.

## Autenticação

O sistema de login foi implementado para garantir a segurança do controle remoto dos compressores. Apenas usuários autenticados podem controlar o dispositivo. A implementação do login é gerida pelo arquivo `autenticador.cpp`, que verifica as credenciais inseridas na interface.

## Principais Arquivos

### `main.cpp`

O arquivo principal do sistema, onde a configuração inicial do ESP32 e o loop principal do programa são definidos. Ele faz a configuração do Wi-Fi, o gerenciamento do Access Point e o roteamento de requisições da interface web.

### `index.cpp`

Este arquivo define o comportamento e a estrutura da página inicial do sistema, carregada assim que o usuário se conecta ao Access Point. Ele contém o código HTML e JavaScript que compõe o painel de controle dos compressores.

### `ligadesliga.cpp`

Contém a lógica que lida com os comandos para ligar ou desligar os compressores, recebendo comandos via interface web e enviando sinais correspondentes ao hardware.

### `wificonexao.cpp` e `wifigerenciador.cpp`

Responsáveis pela configuração de rede e gerenciamento das conexões Wi-Fi. Definem o comportamento tanto para o modo **Access Point** quanto para a conexão a redes Wi-Fi externas.

## Créditos

Projeto desenvolvido como parte do curso de engenharia eletrônica pelos alunos:

- **Brenda**
- **Davi**
- **João**
- **Pedro**
- **Sofia**

## Licença

Este projeto está licenciado sob a licença MIT. Veja o arquivo [LICENSE](LICENSE) para mais detalhes.
