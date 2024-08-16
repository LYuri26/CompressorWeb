#ifndef TEMPO_H // Verifica se o símbolo TEMPO_H não foi definido
#define TEMPO_H // Define o símbolo TEMPO_H para evitar inclusão múltipla do arquivo

// -------------------------------------------------------------------------
// Inclusão das Bibliotecas Necessárias
// -------------------------------------------------------------------------

#include <WiFi.h>      // Inclui a biblioteca para conectividade WiFi, permitindo que o dispositivo se conecte a redes WiFi
#include <WiFiUdp.h>   // Inclui a biblioteca para comunicação UDP sobre WiFi, necessária para enviar e receber pacotes UDP
#include <NTPClient.h> // Inclui a biblioteca NTPClient para sincronização de tempo com um servidor NTP via UDP
#include <time.h>      // Inclui a biblioteca para manipulação de tempo e data, fornecendo funções para manipular e formatar tempo

// -------------------------------------------------------------------------
// Declaração de Funções
// -------------------------------------------------------------------------

/**
 * Configura o cliente NTP para sincronização de tempo.
 *
 * Esta função inicializa o cliente NTP, conecta-se à rede WiFi e configura
 * o servidor NTP para sincronização do tempo. Deve ser chamada durante a
 * configuração inicial do sistema.
 *
 * Detalhes Técnicos:
 * - **Função `setupTimeClient()`**: Inicializa o cliente NTP, que se conecta ao servidor NTP especificado para obter a hora correta.
 * - **Biblioteca `NTPClient`**: Usada para simplificar a sincronização com servidores NTP, ocultando a complexidade da comunicação UDP direta.
 * - **Biblioteca `WiFi`**: Necessária para estabelecer uma conexão com a rede WiFi antes de sincronizar com o servidor NTP.
 */
void setupTimeClient();

/**
 * Obtém a hora atual como uma string formatada.
 *
 * @return A hora atual formatada como uma string.
 *
 * Esta função consulta o cliente NTP e retorna a hora atual no formato
 * especificado. Pode ser utilizada para exibir a hora em uma interface de usuário.
 *
 * Detalhes Técnicos:
 * - **Função `getTimeClient()`**: Consulta o cliente NTP para obter a hora atual e retorna essa informação como uma string.
 * - **Formatos de Hora**: Pode ser formatada em diferentes estilos, dependendo das necessidades da aplicação, como HH:MM:SS ou DD/MM/YYYY.
 */
String getTimeClient();

/**
 * Atualiza a hora interna com base na hora recebida do servidor NTP.
 *
 * Esta função ajusta o relógio interno do dispositivo para garantir que
 * a hora interna esteja sincronizada com o tempo fornecido pelo servidor NTP.
 *
 * Detalhes Técnicos:
 * - **Função `updateTime()`**: Atualiza o relógio interno do dispositivo para refletir a hora obtida do servidor NTP.
 * - **Sincronização**: Essencial para manter a precisão do tempo no dispositivo, especialmente em aplicações que dependem de horários corretos.
 */
void updateTime();

/**
 * Configura o tempo a partir de um servidor NTP.
 *
 * Esta função solicita a hora ao servidor NTP e configura o tempo interno
 * do dispositivo para garantir que o tempo esteja sempre correto.
 *
 * Detalhes Técnicos:
 * - **Função `setTimeFromNTP()`**: Envia uma solicitação ao servidor NTP para obter a hora atual e ajusta o tempo interno do dispositivo.
 * - **Servidor NTP**: O servidor NTP deve estar acessível via UDP, e o cliente NTP deve ser configurado para se conectar a ele.
 */
void setTimeFromNTP();

/**
 * Imprime a hora interna atual no console serial.
 *
 * Esta função exibe a hora atual do dispositivo no console serial. É útil
 * para depuração e para verificar se o tempo está sendo atualizado corretamente.
 *
 * Detalhes Técnicos:
 * - **Função `printInternalTime()`**: Exibe a hora interna no console serial, que é útil para depuração e para verificar se o tempo está sincronizado.
 * - **Console Serial**: Utiliza o monitor serial para exibir informações de depuração, acessível através da IDE do Arduino ou similar.
 */
void printInternalTime();

#endif // TEMPO_H  // Fim da diretiva de inclusão condicional para o símbolo TEMPO_H
