#ifndef AUTENTICADOR_H // Verifica se a macro AUTENTICADOR_H não foi definida. Previne inclusão múltipla deste cabeçalho.
#define AUTENTICADOR_H // Define a macro AUTENTICADOR_H para garantir que este cabeçalho seja incluído apenas uma vez.

#include <ESPAsyncWebServer.h> // Inclui a biblioteca ESPAsyncWebServer
                               // Esta biblioteca permite criar e gerenciar um servidor web assíncrono em dispositivos como o ESP32 ou ESP8266.
                               // O uso de um servidor web assíncrono é crucial para lidar com várias requisições simultaneamente sem bloquear o loop principal.

// -------------------------------------------------------------------------
// Declaração de Variáveis Globais
// -------------------------------------------------------------------------

/**
 * ID da sessão atual.
 *
 * Armazena o identificador da sessão do usuário atual. É usado para manter o estado da sessão e autenticar
 * o usuário durante a navegação nas páginas do servidor. O `sessionId` ajuda a identificar e associar
 * o estado de um usuário específico entre múltiplas requisições.
 *
 * **Tipo**: `String` - Uma string que representa o identificador da sessão.
 * **Uso**: Mantém o estado de login do usuário e facilita a autenticação contínua em diferentes páginas.
 */
extern String sessionId; // Declara a variável global `sessionId`. `extern` indica que a variável é definida em outro arquivo de código fonte.

/**
 * Indica se um usuário está logado.
 *
 * Esta variável booleana é usada para verificar se o usuário está autenticado e logado no sistema.
 * É fundamental para controlar o acesso a páginas restritas e recursos protegidos.
 *
 * **Tipo**: `bool` - Variável booleana que pode ser `true` (verdadeira) ou `false` (falsa).
 * **Uso**: Permite verificar o estado de login do usuário e tomar decisões de acesso baseadas nesse estado.
 */
extern bool userLoggedIn; // Declara a variável global `userLoggedIn`. `extern` indica que a variável é definida em outro arquivo de código fonte.

/**
 * Nome do usuário logado.
 *
 * Armazena o nome do usuário atualmente autenticado. Pode ser utilizado para personalizar a interface ou
 * exibir informações específicas do usuário. O nome pode ser exibido em saudações ou em áreas personalizadas da interface.
 *
 * **Tipo**: `String` - Uma string que representa o nome do usuário logado.
 * **Uso**: Personaliza a experiência do usuário mostrando seu nome ou outras informações relevantes.
 */
extern String loggedInUser; // Declara a variável global `loggedInUser`. `extern` indica que a variável é definida em outro arquivo de código fonte.

// -------------------------------------------------------------------------
// Declaração de Funções
// -------------------------------------------------------------------------

/**
 * Lida com o processo de login do usuário.
 *
 * Esta função é responsável por processar a solicitação de login do usuário, verificando as credenciais fornecidas e
 * iniciando a sessão se as credenciais forem válidas. Ela é chamada quando um usuário envia uma solicitação de login.
 *
 * **Parâmetro**: `AsyncWebServerRequest *request` - Ponteiro para o objeto `AsyncWebServerRequest` que contém a solicitação do login.
 *
 * **Uso**: Verifica as credenciais do usuário, cria ou atualiza a sessão e redireciona o usuário para a página apropriada.
 */
void handleLogin(AsyncWebServerRequest *request); // Declara a função `handleLogin`, que processa as solicitações de login.

/**
 * Lida com o processo de logout do usuário.
 *
 * Esta função é responsável por encerrar a sessão do usuário e redirecioná-lo para a página inicial ou de login.
 * Ela é chamada quando um usuário envia uma solicitação para sair do sistema.
 *
 * **Parâmetro**: `AsyncWebServerRequest *request` - Ponteiro para o objeto `AsyncWebServerRequest` que contém a solicitação de logout.
 *
 * **Uso**: Termina a sessão do usuário, limpa as informações de autenticação e redireciona para uma página de login ou inicial.
 */
void handleLogout(AsyncWebServerRequest *request); // Declara a função `handleLogout`, que processa as solicitações de logout.

/**
 * Verifica se a solicitação é autenticada.
 *
 * Esta função checa se a solicitação recebida possui um status de autenticação válido. É utilizada para garantir
 * que apenas usuários autenticados possam acessar determinadas páginas ou recursos.
 *
 * **Parâmetro**: `AsyncWebServerRequest *request` - Ponteiro para o objeto `AsyncWebServerRequest` que contém a solicitação a ser verificada.
 * **Retorno**: `bool` - Retorna `true` se a solicitação for autenticada e `false` caso contrário.
 *
 * **Uso**: Verifica a autenticidade da solicitação para garantir o acesso controlado a recursos protegidos.
 */
bool isAuthenticated(AsyncWebServerRequest *request); // Declara a função `isAuthenticated`, que verifica a autenticidade da solicitação.

/**
 * Lida com o redirecionamento quando o usuário não está autenticado.
 *
 * Esta função redireciona o usuário para uma página de login ou de erro quando a solicitação não é autenticada.
 * Serve como uma medida de segurança para garantir que usuários não autenticados não acessem recursos restritos.
 *
 * **Parâmetro**: `AsyncWebServerRequest *request` - Ponteiro para o objeto `AsyncWebServerRequest` que contém a solicitação.
 *
 * **Uso**: Redireciona o usuário para uma página que informa que o acesso foi negado ou que é necessário fazer login.
 */
void notAuthenticated(AsyncWebServerRequest *request); // Declara a função `notAuthenticated`, que lida com solicitações não autenticadas.

/**
 * Redireciona para a página de acesso negado.
 *
 * Esta função é chamada quando um usuário tenta acessar uma página ou recurso para o qual não tem permissão.
 * Ela exibe uma página informando ao usuário que ele não tem acesso ao recurso solicitado.
 *
 * **Parâmetro**: `AsyncWebServerRequest *request` - Ponteiro para o objeto `AsyncWebServerRequest` que contém a solicitação.
 *
 * **Uso**: Notifica o usuário de que ele não tem permissão para acessar o recurso e fornece uma página de erro ou de acesso negado.
 */
void redirectToAccessDenied(AsyncWebServerRequest *request); // Declara a função `redirectToAccessDenied`, que redireciona para a página de acesso negado.

#endif // AUTENTICADOR_H  // Fim da diretiva de pré-processador para evitar inclusão múltipla do cabeçalho.
