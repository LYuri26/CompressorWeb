#ifndef TEMPO_H
#define TEMPO_H

// -------------------------------------------------------------------------
// Inclusão das Bibliotecas Necessárias
// -------------------------------------------------------------------------

#include <WiFi.h>         // Biblioteca para conectividade WiFi
#include <WiFiUdp.h>     // Biblioteca para comunicação UDP sobre WiFi
#include <NTPClient.h>   // Biblioteca para sincronização de tempo com NTP
#include <time.h>        // Biblioteca para manipulação de tempo e data

// -------------------------------------------------------------------------
// Declaração de Funções
// -------------------------------------------------------------------------

/**
 * Configura o cliente NTP para sincronização de tempo.
 * 
 * Esta função inicializa o cliente NTP, conecta-se à rede WiFi e configura
 * o servidor NTP para sincronização do tempo. Deve ser chamada durante a
 * configuração inicial do sistema.
 */
void setupTimeClient();

/**
 * Obtém a hora atual como uma string formatada.
 * 
 * @return A hora atual formatada como uma string.
 * 
 * Esta função consulta o cliente NTP e retorna a hora atual no formato
 * especificado. Pode ser utilizada para exibir a hora em uma interface de usuário.
 */
String getTimeClient();

/**
 * Atualiza a hora interna com base na hora recebida do servidor NTP.
 * 
 * Esta função ajusta o relógio interno do dispositivo para garantir que
 * a hora interna esteja sincronizada com o tempo fornecido pelo servidor NTP.
 */
void updateTime();

/**
 * Configura o tempo a partir de um servidor NTP.
 * 
 * Esta função solicita a hora ao servidor NTP e configura o tempo interno
 * do dispositivo para garantir que o tempo esteja sempre correto.
 */
void setTimeFromNTP();

/**
 * Imprime a hora interna atual no console serial.
 * 
 * Esta função exibe a hora atual do dispositivo no console serial. É útil
 * para depuração e para verificar se o tempo está sendo atualizado corretamente.
 */
void printInternalTime();

#endif // TEMPO_H
#ifndef TEMPO_H
#define TEMPO_H

// -------------------------------------------------------------------------
// Inclusão das Bibliotecas Necessárias
// -------------------------------------------------------------------------

#include <WiFi.h>         // Biblioteca para conectividade WiFi
#include <WiFiUdp.h>     // Biblioteca para comunicação UDP sobre WiFi
#include <NTPClient.h>   // Biblioteca para sincronização de tempo com NTP
#include <time.h>        // Biblioteca para manipulação de tempo e data

// -------------------------------------------------------------------------
// Declaração de Funções
// -------------------------------------------------------------------------

/**
 * Configura o cliente NTP para sincronização de tempo.
 * 
 * Esta função inicializa o cliente NTP, conecta-se à rede WiFi e configura
 * o servidor NTP para sincronização do tempo. Deve ser chamada durante a
 * configuração inicial do sistema.
 */
void setupTimeClient();

/**
 * Obtém a hora atual como uma string formatada.
 * 
 * @return A hora atual formatada como uma string.
 * 
 * Esta função consulta o cliente NTP e retorna a hora atual no formato
 * especificado. Pode ser utilizada para exibir a hora em uma interface de usuário.
 */
String getTimeClient();

/**
 * Atualiza a hora interna com base na hora recebida do servidor NTP.
 * 
 * Esta função ajusta o relógio interno do dispositivo para garantir que
 * a hora interna esteja sincronizada com o tempo fornecido pelo servidor NTP.
 */
void updateTime();

/**
 * Configura o tempo a partir de um servidor NTP.
 * 
 * Esta função solicita a hora ao servidor NTP e configura o tempo interno
 * do dispositivo para garantir que o tempo esteja sempre correto.
 */
void setTimeFromNTP();

/**
 * Imprime a hora interna atual no console serial.
 * 
 * Esta função exibe a hora atual do dispositivo no console serial. É útil
 * para depuração e para verificar se o tempo está sendo atualizado corretamente.
 */
void printInternalTime();

#endif // TEMPO_H
