# Compressor Web Control

## Equipe

- **Ali**
- **Felipe**
- **Paulo**
- **Pietro**
- **Yago**

## Instrutores

- **Franco**
- **Lenon**

## Objetivo

Desenvolver uma aplicação web utilizando o ESP32 para ligar e desligar um compressor localizado no SENAI Uberaba.

## Requisitos

- **Funcionamento Local**: A aplicação funcionará localmente através da rede Wi-Fi.
- **Sistema de Login**: Haverá um sistema de login com usuário e senha para acessar a aplicação e controlar o compressor.
- **Placa**: Usaremos o ESP32 CH340G, compatível com os notebooks fornecidos pelo SENAI aos instrutores.
- **Linguagens**: As linguagens utilizadas serão C++, JavaScript, HTML e CSS.
- **Automatização**: Implementaremos uma função no ESP32 que desligará o compressor automaticamente após as 22:30, como medida de segurança.
- **Proteção Física**: Utilizaremos uma impressora 3D para fabricar um suporte que protegerá o ESP32 e seus componentes contra umidade e danos físicos, facilitando a manutenção.
- **Acessibilidade**: A aplicação será acessível por múltiplos dispositivos: desktop, notebook, smartphone e tablet.
- **Ambiente Local**: O sistema será utilizado exclusivamente em ambiente local para evitar acionamentos acidentais fora do SENAI, garantindo a segurança do compressor.

## Requisitos que Não Serão Atendidos

- Cadastro individual de usuários.
- Uso de outras placas ESP32 além do modelo CH340G.
- Escalabilidade para outros dispositivos além do compressor.
- Implementação de banco de dados e registro de utilizadores.

## Escopo da Aplicação

A aplicação será responsável apenas por ligar e desligar o compressor, embora possa ser expandida para outras funções futuramente.

### Possível Escalabilidade

Embora a aplicação inicial se concentre apenas em ligar e desligar o compressor, há um potencial significativo para escalabilidade futura. Podemos integrar funcionalidades adicionais, como:

- **Monitoramento de Umidade**: Sensores para medir a umidade no ambiente do compressor, garantindo condições ideais de operação.
- **Monitoramento de Voltagem**: Análise da voltagem do compressor para detectar possíveis anomalias e evitar danos ao equipamento.
- **Monitoramento de Temperatura**: Sensores para monitorar a temperatura do compressor, prevenindo superaquecimento e garantindo um funcionamento seguro.
- **Alertas e Notificações**: Sistema de alertas para notificar os usuários sobre qualquer irregularidade detectada, como variações na voltagem ou umidade.
- **Interface de Usuário Amigável**: Melhorias na interface web para facilitar o uso e a interação com o sistema.

## Custos

- **ESP32 CH340G**
- **Notebook**
- **Transistores**
- **Visual Studio Code (VSCode)**
- **Bateria**

## Considerações Finais

Nosso objetivo é criar uma solução segura e eficiente para o controle do compressor no SENAI Uberaba, utilizando tecnologia acessível e recursos locais. O sistema será desenvolvido de forma a ser robusto, seguro e fácil de utilizar, atendendo às necessidades do projeto e respeitando as limitações estabelecidas. Com a possibilidade de futuras expansões, nosso projeto tem o potencial de evoluir para um sistema de monitoramento completo, garantindo ainda mais segurança e eficiência no uso do compressor.
