#ifndef LIGADESLIGA_H
#define LIGADESLIGA_H

// -------------------------------------------------------------------------
// Inclusão de Bibliotecas
// -------------------------------------------------------------------------

#include <ESPAsyncWebServer.h> // Biblioteca para criar e gerenciar um servidor web assíncrono

// -------------------------------------------------------------------------
// Declaração de Variáveis Globais
// -------------------------------------------------------------------------

/**
 * Variável global para armazenar o estado do compressor.
 * 
 * Esta variável indica se o compressor está ligado ou desligado. 
 * Ela pode ser usada para controlar e verificar o estado do compressor
 * em diferentes partes do código.
 */
extern bool compressorLigado;

// -------------------------------------------------------------------------
// Declaração de Funções
// -------------------------------------------------------------------------

/**
 * Configura as rotas e handlers do servidor web relacionados à funcionalidade de ligar e desligar.
 * 
 * Esta função define as rotas HTTP e os handlers associados para permitir 
 * que o servidor web possa controlar o estado do compressor, ligando-o ou 
 * desligando-o conforme necessário.
 * 
 * @param server Referência para o objeto do servidor web assíncrono que gerencia as requisições.
 */
void setupLigaDesliga(AsyncWebServer& server);

/**
 * Atualiza o status do compressor.
 * 
 * Esta função pode ser usada para ligar ou desligar o compressor com base em
 * alguma lógica ou condição específica. Ela altera o estado do compressor 
 * e pode acionar ações correspondentes.
 */
void updateCompressorStatus();

#endif // LIGADESLIGA_H
