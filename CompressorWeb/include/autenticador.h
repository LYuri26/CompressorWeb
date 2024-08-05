#ifndef AUTENTICADOR_H  // Verifica se a macro AUTENTICADOR_H não está definida
#define AUTENTICADOR_H  // Define a macro AUTENTICADOR_H para evitar inclusão múltipla deste cabeçalho

// -------------------------------------------------------------------------
// Inclusão de Bibliotecas
// -------------------------------------------------------------------------

#include <ESPAsyncWebServer.h>  // Inclui a biblioteca para o servidor web assíncrono

// -------------------------------------------------------------------------
// Declaração de Variáveis Globais
// -------------------------------------------------------------------------

/**
 * ID da sessão atual.
 * 
 * Armazena o identificador da sessão do usuário atual. Utilizado para manter o estado da sessão e autenticar
 * o usuário durante a navegação.
 */
extern String sessionId;

/**
 * Indica se um usuário está logado.
 * 
 * Esta variável booleana é usada para verificar se o usuário está autenticado e logado no sistema.
 */
extern bool userLoggedIn;

/**
 * Nome do usuário logado.
 * 
 * Armazena o nome do usuário atualmente autenticado. Pode ser utilizado para personalizar a interface ou
 * mostrar informações específicas do usuário.
 */
extern String loggedInUser;

// -------------------------------------------------------------------------
// Declaração de Funções
// -------------------------------------------------------------------------

/**
 * Lida com o processo de login do usuário.
 * 
 * Esta função é responsável por processar a solicitação de login do usuário, verificando as credenciais e
 * iniciando a sessão.
 * 
 * @param request Ponteiro para o objeto AsyncWebServerRequest que contém a solicitação do login.
 */
void handleLogin(AsyncWebServerRequest *request);

/**
 * Lida com o processo de logout do usuário.
 * 
 * Esta função é responsável por encerrar a sessão do usuário e redirecioná-lo para a página inicial ou de login.
 * 
 * @param request Ponteiro para o objeto AsyncWebServerRequest que contém a solicitação do logout.
 */
void handleLogout(AsyncWebServerRequest *request);

/**
 * Verifica se a solicitação é autenticada.
 * 
 * Esta função checa se a solicitação recebida tem um status de autenticação válido. Usada para garantir que
 * apenas usuários autenticados acessem certas páginas ou recursos.
 * 
 * @param request Ponteiro para o objeto AsyncWebServerRequest que contém a solicitação a ser verificada.
 * @return Retorna verdadeiro se a solicitação for autenticada, caso contrário, retorna falso.
 */
bool isAuthenticated(AsyncWebServerRequest *request);

/**
 * Lida com o redirecionamento quando o usuário não está autenticado.
 * 
 * Esta função redireciona o usuário para uma página de login ou erro caso a solicitação não seja autenticada.
 * 
 * @param request Ponteiro para o objeto AsyncWebServerRequest que contém a solicitação.
 */
void notAuthenticated(AsyncWebServerRequest *request);

/**
 * Redireciona para a página de acesso negado.
 * 
 * Esta função é chamada quando um usuário tenta acessar uma página ou recurso para o qual não tem permissão.
 * A página de acesso negado deve informar o usuário de que ele não tem acesso ao recurso solicitado.
 * 
 * @param request Ponteiro para o objeto AsyncWebServerRequest que contém a solicitação.
 */
void redirectToAccessDenied(AsyncWebServerRequest *request);

#endif // AUTENTICADOR_H  // Finaliza a diretiva de pré-processador #ifndef/#define
