#ifndef BOTAO_H
#define BOTAO_H

#include <Arduino.h>

class Botao {
private:
    uint8_t pino;

    bool leituraAnterior;
    bool estadoEstavel;

    unsigned long momentoUltimaMudanca;
    unsigned long tempoDebounce;

public:
    Botao(uint8_t gpio, unsigned long debounce = 50) {
        pino = gpio;

        leituraAnterior = HIGH;
        estadoEstavel = HIGH;

        momentoUltimaMudanca = 0;
        tempoDebounce = debounce;
    }

    void begin() {
        pinMode(pino, INPUT_PULLUP);

        leituraAnterior = digitalRead(pino);
        estadoEstavel = leituraAnterior;
    }

    bool clicou() {
        bool leituraAtual = digitalRead(pino);

        // Detectou alguma mudança elétrica no botão.
        if (leituraAtual != leituraAnterior) {
            momentoUltimaMudanca = millis();
        }

        // Verifica se a leitura permaneceu estável.
        if (millis() - momentoUltimaMudanca >= tempoDebounce) {

            // O estado estável realmente mudou.
            if (leituraAtual != estadoEstavel) {
                estadoEstavel = leituraAtual;

                // INPUT_PULLUP: LOW significa pressionado.
                if (estadoEstavel == LOW) {
                    leituraAnterior = leituraAtual;
                    return true;
                }
            }
        }

        leituraAnterior = leituraAtual;
        return false;
    }
};

#endif