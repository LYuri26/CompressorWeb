#ifndef MANUTENCAO_H // Verifica se o símbolo MANUTENCAO_H ainda não foi definido
#define MANUTENCAO_H // Define o símbolo MANUTENCAO_H para evitar inclusão múltipla do arquivo

#include <ESPAsyncWebServer.h> // Inclui a biblioteca ESPAsyncWebServer
                               // Esta biblioteca é usada para criar e gerenciar um servidor web assíncrono.
                               // O servidor web assíncrono permite que você crie e manipule páginas da web sem bloquear o loop principal do programa.
                               // Isso é especialmente útil para projetos baseados em microcontroladores como o ESP32 ou ESP8266.

// Ao incluir esta biblioteca, você pode definir rotas HTTP e responder a solicitações de forma eficiente.

// Por exemplo, você pode definir uma rota para uma página web que exibe o status de manutenção do sistema.

// -------------------------------------------------------------------------
// Declarações das funções e variáveis relacionadas à manutenção
// -------------------------------------------------------------------------

/**
 * Configura o botão de manutenção.
 *
 * Esta função é responsável por inicializar e configurar um botão físico ou virtual que
 * controla o modo de manutenção do sistema.
 *
 * @note A função deve ser chamada durante a configuração do sistema para garantir que
 *       o botão esteja corretamente configurado e pronto para uso.
 */
void setupManutencao(); // Declara a função `setupManutencao`, que é usada para configurar o botão de manutenção.

// Detalhes Técnicos:
// - **Função `setupManutencao()`**: Esta função deve ser implementada para configurar o botão de manutenção, que pode incluir a definição de pinos de entrada ou a configuração de interfaces de usuário.
// - **Objetivo**: Permite ao usuário iniciar ou parar a manutenção do sistema com um botão físico ou virtual.
// - **Importância**: A função deve ser chamada no início do programa para garantir que o botão esteja funcionando corretamente.

// -------------------------------------------------------------------------
// Função para atualizar o estado do botão de manutenção
// -------------------------------------------------------------------------

/**
 * Atualiza o estado do botão de manutenção.
 *
 * Esta função verifica e atualiza o status do botão de manutenção e reflete
 * as mudanças no estado do sistema.
 *
 * @note Esta função deve ser chamada periodicamente para monitorar o estado do botão
 *       e garantir que o sistema responda adequadamente às alterações.
 */
void atualizarEstadoManutencao(); // Declara a função `atualizarEstadoManutencao`, responsável por atualizar o estado do botão de manutenção.

// Detalhes Técnicos:
// - **Função `atualizarEstadoManutencao()`**: Esta função deve ser implementada para ler o estado atual do botão de manutenção e atualizar a variável global `sistemaEmManutencao` conforme necessário.
// - **Objetivo**: Garante que o estado do sistema esteja sincronizado com o estado atual do botão.
// - **Importância**: Permite que o sistema responda a mudanças no estado de manutenção em tempo real.

// -------------------------------------------------------------------------
// Variável para verificar se o sistema está em manutenção
// -------------------------------------------------------------------------

/**
 * Indica se o sistema está em manutenção.
 *
 * Esta variável global é usada para verificar se o sistema está atualmente em modo de manutenção.
 *
 * @note A variável é declarada como `extern` para indicar que a definição real ocorre em outro arquivo de código fonte.
 */
extern bool sistemaEmManutencao; // Declara a variável global `sistemaEmManutencao` como `extern`, indicando que sua definição está em outro lugar.

// Detalhes Técnicos:
// - **Variável `sistemaEmManutencao`**: Esta variável deve ser definida em um arquivo de código fonte e pode ser usada em várias partes do programa para verificar o status de manutenção.
// - **Objetivo**: Permite que diferentes partes do código verifiquem se o sistema está em modo de manutenção e ajustem seu comportamento conforme necessário.
// - **Importância**: Facilita a implementação de lógica condicional que deve ocorrer quando o sistema está ou não em manutenção.

#endif // MANUTENCAO_H          // Fim da diretiva de inclusão condicional para o símbolo MANUTENCAO_H
