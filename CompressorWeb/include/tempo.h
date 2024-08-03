#ifndef TEMPO_H
#define TEMPO_H

#include <WiFiUdp.h>

void setupTimeClient();
String getTimeClient();
void updateTime();

#endif
