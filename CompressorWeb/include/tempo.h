#ifndef TEMPO_H
#define TEMPO_H

#include <Arduino.h>
#include <time.h>
#include <sys/time.h>

// -------------------------------------------------------------------------
// 1. Constantes Globais de Fuso Horário e Servidores NTP
// -------------------------------------------------------------------------
#define DEFAULT_GMT_OFFSET_SEC (-10800) // UTC-3 (Horário Oficial de Brasília: -3h = -10800s)
#define DEFAULT_DAYLIGHT_OFFSET (0)     // Horário de Verão desativado (0s)

// -------------------------------------------------------------------------
// 2. Declaração das Funções Principais de Tempo
// -------------------------------------------------------------------------

/**
 * Inicializa o subsistema SNTP nativo do ESP32 e configura os servidores de rede.
 */
void setupTimeClient();

/**
 * Retorna a hora atual como string no formato padrão "YYYY-MM-DDTHH:MM".
 * Utilizado pelos módulos de controle (ligadesliga.cpp) para verificação de turnos.
 */
String getTimeClient();

/**
 * Atualiza o relógio interno a partir da estrutura tm do RTC do ESP-IDF.
 */
void updateTime();

/**
 * Força uma nova solicitação de sincronização de relógio aos servidores NTP.
 */
void setTimeFromNTP();

/**
 * Imprime a data e hora detalhada do relógio interno no console serial.
 */
void printInternalTime();

// -------------------------------------------------------------------------
// 3. Funções Avançadas de Telemetria e Diagnóstico Temporal
// -------------------------------------------------------------------------

bool isTimeSynchronized();
unsigned long getEpochTime();
String getFormattedTimeBR();
String getISODateTime();
String getDateTimeJSON();
unsigned long getUptimeSeconds();
String getUptimeFormatted();

bool setManualDateTime(int ano, int mes, int dia, int hora, int minuto, int segundo);
int getHoraAtual();
int getMinutoAtual();
int getSegundoAtual();
int getDiaSemana();

#endif // TEMPO_H