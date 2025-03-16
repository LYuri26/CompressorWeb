#include "pressostato.h"
#include "ligadesliga.h"

const int pinoPressostato = 17; // Pino do pressostato (G17)

// Verifica o estado do pressostato e toma ações com base na lógica descrita
void verificarPressostato()
{
    bool estadoPressostato = digitalRead(pinoPressostato); // Lê o estado do pressostato (G17)
    bool estadoCompressor = digitalRead(pinosStatus[0]);   // Lê o estado do compressor (G14)

    // Lógica booleana conforme descrito
    if (estadoPressostato == HIGH && estadoCompressor == HIGH)
    {
        // Ambos estão ligados: algo está errado, desligue o compressor
        saveMotorState(arquivosMotores[0], false); // Desliga o compressor
        saveStatusState(pinosStatus[0], false);    // Atualiza o status do compressor para desligado
        Serial.println("Pressostato e compressor ligados simultaneamente. Desligando compressor.");
    }
    else if (estadoPressostato == LOW && estadoCompressor == LOW)
    {
        // Ambos estão desligados: algo está errado, desligue o compressor
        saveMotorState(arquivosMotores[0], false); // Desliga o compressor
        saveStatusState(pinosStatus[0], false);    // Atualiza o status do compressor para desligado
        Serial.println("Pressostato e compressor desligados simultaneamente. Desligando compressor.");
    }
    else
    {
        // Casos normais: mantém o compressor ligado
        saveMotorState(arquivosMotores[0], true); // Mantém o compressor ligado
        // Não atualiza o status1.txt, pois ele só deve ser alterado para desligar
        Serial.println("Comportamento esperado. Compressor ligado.");
    }
}