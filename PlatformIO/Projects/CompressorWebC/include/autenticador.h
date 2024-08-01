// autenticador.h

#ifndef AUTENTICADOR_H  // Se a macro AUTENTICADOR_H não está definida
#define AUTENTICADOR_H  // Define a macro AUTENTICADOR_H para evitar inclusão múltipla deste cabeçalho

#include <WebServer.h>  // Inclui a biblioteca WebServer, que fornece funcionalidades para criar e gerenciar um servidor web no ESP32

// Declaração da função handleLogin
// Esta função é responsável por processar a requisição de login, verificando as credenciais e gerenciando o estado de login do usuário
void handleLogin();

// Declaração da função handleLogout
// Esta função é responsável por processar a requisição de logout, encerrando a sessão do usuário e atualizando o estado de login
void handleLogout();

// Declaração da variável global userLoggedIn
// A variável userLoggedIn indica se um usuário está logado ou não
// É declarada como externa para que possa ser usada em outros arquivos
extern bool userLoggedIn;

// Declaração da variável global loggedInUser
// A variável loggedInUser armazena o nome do usuário que está logado
// É declarada como externa para que possa ser usada em outros arquivos
extern String loggedInUser;

// Declaração da variável global server
// A variável server é uma instância do objeto WebServer usada para gerenciar e configurar o servidor web
// É declarada como externa para que possa ser usada em outros arquivos
extern WebServer server;

#endif // AUTENTICADOR_H  // Marca o fim da diretiva de pré-processador #ifndef/#define
