#ifndef TEMPO_H
#define TEMPO_H


#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <time.h>

void setupTimeClient();
String getTimeClient();
void updateTime();
void setTimeFromNTP();
void printInternalTime();

#endif
