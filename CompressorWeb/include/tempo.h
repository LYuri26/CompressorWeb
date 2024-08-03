#ifndef TEMPO_H
#define TEMPO_H

#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <time.h>

// Declaração das funções
void setupTimeClient();
String getTimeClient();
String getInternTime();
void updateTime();
void setTimeFromNTP();
void printInternalTime();
void updateCurrentTime();

#endif // TEMPO_H
