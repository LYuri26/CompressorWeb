#ifndef PAGINASERRO_H // Verifica se o símbolo PAGINASERRO_H não foi definido
#define PAGINASERRO_H // Define o símbolo PAGINASERRO_H para evitar inclusão múltipla do arquivo

// -------------------------------------------------------------------------
// Inclusão da Biblioteca para Servidor Web Assíncrono
// -------------------------------------------------------------------------

#include <ESPAsyncWebServer.h> // Inclui a biblioteca ESPAsyncWebServer
                               // Esta biblioteca é necessária para criar e gerenciar um servidor web assíncrono em plataformas como o ESP32.
                               // O uso desta biblioteca permite a criação de servidores web que podem lidar com múltiplas requisições simultaneamente,
                               // sem bloquear a execução do código principal do programa.

// -------------------------------------------------------------------------
// Declaração de Funções
// -------------------------------------------------------------------------

/**
 * Configura a página de acesso inválido no servidor web.
 *
 * Esta função define a rota e o conteúdo para uma página de acesso inválido,
 * que será exibida quando um usuário tentar acessar uma área restrita sem
 * as permissões necessárias.
 *
 * @param server Referência para o objeto do servidor web assíncrono que gerenciará a página.
 *
 * Detalhes Técnicos:
 * - **Função `setupAcessoInvalidoPage(AsyncWebServer& server)`**: Esta função deve ser implementada para configurar a página que será exibida quando um acesso inválido for detectado.
 * - **Parâmetro `server`**: É uma referência para um objeto `AsyncWebServer`, que é usado para definir rotas e respostas para requisições HTTP. O objeto deve ser configurado para servir a página de erro apropriada.
 * - **Configuração da Página**: A página pode incluir uma mensagem informando ao usuário que ele não tem permissão para acessar a página solicitada.
 * - **Importância**: Fornece uma interface de erro amigável para usuários que tentam acessar páginas protegidas sem a devida autenticação.
 */
void setupAcessoInvalidoPage(AsyncWebServer &server);

/**
 * Configura a página de erro 404 (não encontrado) no servidor web.
 *
 * Esta função define a rota e o conteúdo para uma página de erro 404, que será
 * exibida quando o usuário tentar acessar uma URL que não existe no servidor.
 *
 * @param server Referência para o objeto do servidor web assíncrono que gerenciará a página.
 *
 * Detalhes Técnicos:
 * - **Função `setupNotFoundPage(AsyncWebServer& server)`**: Esta função deve ser implementada para configurar a página que será exibida quando uma URL não for encontrada.
 * - **Parâmetro `server`**: É uma referência para um objeto `AsyncWebServer`, usado para definir a resposta para uma requisição HTTP que resulta em um erro 404.
 * - **Configuração da Página**: A página pode exibir uma mensagem de erro indicando que o recurso solicitado não foi encontrado e, possivelmente, fornecer links para a página inicial ou outras seções do site.
 * - **Importância**: Melhora a experiência do usuário ao fornecer uma página amigável em vez de uma mensagem de erro padrão ou um código de status HTTP cru.
 */
void setupNotFoundPage(AsyncWebServer &server);

/**
 * Configura a página para usuários já logados no servidor web.
 *
 * Esta função define a rota e o conteúdo para uma página que informa ao usuário
 * que ele já está logado e não precisa fazer login novamente.
 *
 * @param server Referência para o objeto do servidor web assíncrono que gerenciará a página.
 *
 * Detalhes Técnicos:
 * - **Função `setupUsuarioJaLogadoPage(AsyncWebServer& server)`**: Esta função deve ser implementada para configurar a página que será exibida quando um usuário já estiver autenticado e tentar acessar uma página de login novamente.
 * - **Parâmetro `server`**: É uma referência para um objeto `AsyncWebServer`, usado para definir a resposta para uma requisição HTTP quando o usuário já está logado.
 * - **Configuração da Página**: A página pode incluir uma mensagem informando que o login já foi realizado e fornecer links para áreas do site onde o usuário pode navegar.
 * - **Importância**: Evita a frustração do usuário ao tentar fazer login novamente quando já está autenticado, e melhora a fluidez da navegação.
 */
void setupUsuarioJaLogadoPage(AsyncWebServer &server);

/**
 * Configura a página de credenciais inválidas no servidor web.
 *
 * Esta função define a rota e o conteúdo para uma página que informa ao usuário
 * que as credenciais fornecidas durante o login são inválidas.
 *
 * @param server Referência para o objeto do servidor web assíncrono que gerenciará a página.
 *
 * Detalhes Técnicos:
 * - **Função `setupCredenciaisInvalidasPage(AsyncWebServer& server)`**: Esta função deve ser implementada para configurar a página exibida quando um usuário tenta fazer login com credenciais inválidas.
 * - **Parâmetro `server`**: É uma referência para um objeto `AsyncWebServer`, usado para definir a resposta para uma requisição HTTP com credenciais inválidas.
 * - **Configuração da Página**: A página pode exibir uma mensagem de erro indicando que as credenciais são inválidas e oferecer um link para tentar o login novamente.
 * - **Importância**: Melhora a experiência do usuário fornecendo feedback claro e ações corretivas quando o login falha devido a credenciais incorretas.
 */
void setupCredenciaisInvalidasPage(AsyncWebServer &server);

#endif // PAGINASERRO_H  // Fim da diretiva de inclusão condicional para o símbolo PAGINASERRO_H
