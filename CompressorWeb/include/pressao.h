#ifndef PRESSAO_H // Verifica se o símbolo PRESSAO_H não foi definido
#define PRESSAO_H // Define o símbolo PRESSAO_H para evitar inclusão múltipla do arquivo

// -------------------------------------------------------------------------
// Inclusão de Bibliotecas
// -------------------------------------------------------------------------

#include <ESPAsyncWebServer.h> // Inclui a biblioteca para criar e gerenciar um servidor web assíncrono
                               // Esta biblioteca permite criar servidores web que respondem a requisições HTTP de forma eficiente
                               // e sem bloquear a execução do código principal, facilitando a criação de interfaces web interativas.

// -------------------------------------------------------------------------
// Declaração de Funções
// -------------------------------------------------------------------------

/**
 * Configura a página de nível de pressão no servidor web.
 *
 * Esta função define a rota e o conteúdo da página de nível de pressão,
 * que exibirá informações relacionadas à pressão de um sistema ou sensor.
 * A página será acessível via um servidor web assíncrono configurado pelo
 * objeto `server` passado como parâmetro.
 *
 * @param server Referência para o objeto do servidor web assíncrono que
 *               gerenciará a configuração da página.
 *
 * Detalhes Técnicos:
 * - **Função `setupPressaoPage(AsyncWebServer& server)`**: Define uma página web para mostrar dados de pressão. A configuração inclui a definição de rotas e a resposta a requisições HTTP.
 * - **Parâmetro `server`**: O parâmetro é uma referência para um objeto `AsyncWebServer`. Esse objeto é usado para definir rotas e gerenciar requisições HTTP de forma assíncrona.
 * - **Configuração da Página**: A função deve definir como a página será acessada (por exemplo, através de uma rota específica como `/pressao`) e qual o conteúdo HTML será servido ao cliente.
 * - **Importância**: Fornece uma interface web para monitorar a pressão, o que pode ser útil para aplicações de monitoramento remoto ou controle de sistemas com sensores de pressão.
 */
void setupPressaoPage(AsyncWebServer &server);

#endif // PRESSAO_H  // Fim da diretiva de inclusão condicional para o símbolo PRESSAO_H
