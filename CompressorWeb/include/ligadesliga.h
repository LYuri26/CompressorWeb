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
 * Configura as rotas e handlers do servidor web para controle do compressor.
 * 
 * Esta função define as rotas HTTP e os handlers associados para permitir 
 * que o servidor web possa controlar o estado do compressor, ligando-o ou 
 * desligando-o conforme necessário.
 * 
 * @param server Referência para o objeto do servidor web assíncrono que gerencia as requisições.
 */
void setupLigaDesliga(AsyncWebServer& server);

/**
 * Atualiza o status do compressor com base em condições específicas.
 * 
 * Esta função é responsável por verificar e atualizar o estado do compressor, 
 * ligando-o ou desligando-o conforme a lógica definida, como o tempo decorrido ou o horário.
 */
void updateCompressorStatus();
extern const int pinoLigaDesliga;          // Declara a variável global
void saveCompressorState(bool state); // Declara a função global

#endif // LIGADESLIGA_H
