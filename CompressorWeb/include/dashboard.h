#ifndef DASHBOARD_H // Verifica se o símbolo DASHBOARD_H ainda não foi definido
#define DASHBOARD_H // Define o símbolo DASHBOARD_H para evitar a inclusão múltipla do arquivo de cabeçalho

// -------------------------------------------------------------------------
// Inclusão de Bibliotecas
// -------------------------------------------------------------------------

#include <ESPAsyncWebServer.h> // Inclui a biblioteca ESPAsyncWebServer
                               // Esta biblioteca fornece funcionalidades para criar e gerenciar um servidor web assíncrono.
                               // Permite a criação de rotas HTTP e o gerenciamento de solicitações e respostas de forma não bloqueante.
                               // É utilizada para construir e servir páginas web em projetos baseados em microcontroladores como o ESP32 ou ESP8266.

// -------------------------------------------------------------------------
// Declaração de Funções
// -------------------------------------------------------------------------

/**
 * Configura a página do dashboard no servidor web.
 *
 * Esta função define a configuração para a página do dashboard, que é acessada através do servidor web.
 * A função é responsável por configurar as rotas HTTP e os manipuladores (handlers) necessários para servir
 * o conteúdo da página do dashboard. A página do dashboard pode incluir informações de status do sistema e
 * controles de operação.
 *
 * @param server Referência para o objeto `AsyncWebServer` que é utilizado para adicionar e configurar rotas e handlers.
 *
 * Detalhes Técnicos:
 * - **Parâmetro `server`**: Um objeto do tipo `AsyncWebServer` que gerencia a configuração e operação do servidor web assíncrono.
 * - **Objetivo da Função**: Configurar a página do dashboard, definindo rotas para responder a solicitações HTTP específicas e fornecer o conteúdo da página.
 * - **Importância**: A página do dashboard é essencial para a interface de usuário, permitindo a visualização de informações de status e a execução de controles para o sistema.
 */
void setupDashboardPage(AsyncWebServer &server); // Declara a função `setupDashboardPage`, que configura a página do dashboard no servidor web.

/**
 * Configura as ações de ligar/desligar do compressor no servidor web.
 *
 * Esta função define as rotas e manipuladores (handlers) necessários para controlar o estado do compressor,
 * permitindo que o usuário possa ligar ou desligar o compressor através do servidor web.
 *
 * @param server Referência para o objeto `AsyncWebServer` que é usado para definir rotas e handlers relacionados ao controle do compressor.
 *
 * Detalhes Técnicos:
 * - **Parâmetro `server`**: Um objeto do tipo `AsyncWebServer` que é utilizado para definir e gerenciar as rotas relacionadas ao controle do compressor.
 * - **Objetivo da Função**: Adicionar rotas que respondem a solicitações HTTP para alterar o estado do compressor e implementar a lógica para executar essas ações.
 * - **Importância**: Permite a integração de controles de hardware (como o compressor) diretamente na interface web, proporcionando ao usuário uma maneira conveniente de controlar o sistema.
 */
void handleToggleAction(AsyncWebServer &server); // Declara a função `handleToggleAction`, que configura as ações de ligar/desligar do compressor no servidor web.

#endif // DASHBOARD_H        // Fim da diretiva de inclusão condicional para o símbolo DASHBOARD_H
