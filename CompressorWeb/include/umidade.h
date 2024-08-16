#ifndef UMIDADE_H // Verifica se o símbolo UMIDADE_H não foi definido
#define UMIDADE_H // Define o símbolo UMIDADE_H para evitar inclusão múltipla do arquivo

// -------------------------------------------------------------------------
// Inclusão de Bibliotecas
// -------------------------------------------------------------------------

#include <ESPAsyncWebServer.h> // Inclui a biblioteca ESPAsyncWebServer para criar um servidor web assíncrono

// -------------------------------------------------------------------------
// Declaração de Funções
// -------------------------------------------------------------------------

/**
 * Configura a página de umidade no servidor web.
 *
 * @param server Instância do servidor web assíncrono para adicionar rotas e manipular requisições.
 *
 * @note Esta função define a configuração da página da umidade que será servida pelo servidor web.
 * Ela deve ser chamada durante a configuração do servidor para garantir que a página de umidade esteja disponível.
 *
 * Detalhes Técnicos:
 * - **`AsyncWebServer& server`**: O parâmetro é uma referência para uma instância do `ESPAsyncWebServer`. Passar o servidor como referência permite que a função adicione rotas e manipule requisições sem criar uma cópia do objeto.
 * - **Função**: `setupUmidadePage` é responsável por configurar as rotas e handlers associados à página de umidade. O servidor web assíncrono permite que você defina páginas web e gerencie requisições HTTP de forma eficiente e não bloqueante.
 * - **Importância**: Configurar a página de umidade é essencial para que o servidor possa fornecer informações sobre a umidade, possivelmente exibindo dados de sensores ou informações de status.
 */
void setupUmidadePage(AsyncWebServer &server);

#endif // UMIDADE_H  // Fim da diretiva de inclusão condicional para o símbolo UMIDADE_H
