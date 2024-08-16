#ifndef CREDITOS_H // Verifica se o símbolo CREDITOS_H ainda não foi definido
#define CREDITOS_H // Define o símbolo CREDITOS_H para evitar a inclusão múltipla do arquivo de cabeçalho

// -------------------------------------------------------------------------
// Inclusão de Bibliotecas
// -------------------------------------------------------------------------

#include <ESPAsyncWebServer.h> // Inclui a biblioteca ESPAsyncWebServer
                               // Esta biblioteca permite criar e gerenciar um servidor web assíncrono em microcontroladores como o ESP32 ou ESP8266.
                               // A funcionalidade assíncrona ajuda a lidar com múltiplas conexões simultaneamente sem bloquear o loop principal do programa.
                               // É essencial para criar interfaces web interativas e responsivas.

// -------------------------------------------------------------------------
// Declaração de Funções
// -------------------------------------------------------------------------

/**
 * Configura a página de créditos no servidor web.
 *
 * Esta função define a configuração para a página de créditos que será servida pelo servidor web.
 * Ela configura as rotas HTTP e manipuladores (handlers) necessários para fornecer o conteúdo da página.
 * A página de créditos normalmente inclui informações sobre os criadores do projeto, agradecimentos e outras
 * informações relevantes.
 *
 * @param server Referência para o objeto `AsyncWebServer` utilizado para adicionar e configurar rotas e handlers.
 *
 * Detalhes Técnicos:
 * - **Parâmetro `server`**: Um objeto do tipo `AsyncWebServer` que é usado para adicionar rotas e configurar manipuladores para a página de créditos.
 * - **Objetivo da Função**: Configurar a página de créditos, incluindo a definição de rotas para responder a solicitações HTTP e fornecer o conteúdo da página.
 * - **Importância**: A página de créditos oferece uma oportunidade para reconhecer os contribuintes e fornecer informações adicionais sobre o projeto. É uma boa prática incluir informações de crédito e agradecimento em projetos colaborativos.
 */
void setupCreditosPage(AsyncWebServer &server); // Declara a função `setupCreditosPage`, que configura a página de créditos no servidor web.

#endif // CREDITOS_H        // Fim da diretiva de inclusão condicional para o símbolo CREDITOS_H
