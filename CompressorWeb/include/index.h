#ifndef INDEX_H // Verifica se o símbolo INDEX_H ainda não foi definido
#define INDEX_H // Define o símbolo INDEX_H para evitar a inclusão múltipla do arquivo de cabeçalho

#include <ESPAsyncWebServer.h> // Inclui a biblioteca ESPAsyncWebServer
                               // Esta biblioteca fornece funcionalidades para criar e gerenciar um servidor web assíncrono.
                               // Permite a criação de rotas HTTP e o gerenciamento de solicitações e respostas de forma não bloqueante.
                               // É particularmente útil em projetos com microcontroladores como o ESP32 ou ESP8266.

// Ao incluir esta biblioteca, você pode configurar e servir páginas web para o usuário final.

// -------------------------------------------------------------------------
// Declaração de Funções
// -------------------------------------------------------------------------

/**
 * Configura a página inicial do servidor web.
 *
 * Esta função define a configuração para a página inicial que é acessada quando um usuário
 * visita a raiz do servidor web. Ela define as rotas HTTP e os manipuladores necessários
 * para servir o conteúdo da página inicial do servidor.
 *
 * @param server Referência para o objeto `AsyncWebServer` que é utilizado para configurar rotas e handlers.
 *
 * Detalhes Técnicos:
 * - **Parâmetro `server`**: É uma referência para o objeto `AsyncWebServer`, que é a instância do servidor web assíncrono.
 * - **Função `setupIndexPage(AsyncWebServer& server)`**: Esta função deve ser chamada durante a inicialização do servidor para configurar a página inicial.
 * - **Objetivo**: Define como o servidor responde a solicitações para a raiz do servidor ("/"), tipicamente servindo o conteúdo da página inicial.
 * - **Importância**: A página inicial é a primeira página que um usuário vê ao acessar o servidor e frequentemente fornece links para outras páginas ou informações principais.
 */
void setupIndexPage(AsyncWebServer &server); // Declara a função `setupIndexPage`, que configura a página inicial do servidor web.

#endif // INDEX_H            // Fim da diretiva de inclusão condicional para o símbolo INDEX_H
