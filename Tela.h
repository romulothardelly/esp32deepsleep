#ifndef TELA_H
#define TELA_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>



#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_ADDRESS 0x3C

#define SDA_PIN 8
#define SCL_PIN 9

enum TipoTela {
  TELA_1,
  TELA_2
};

class Tela {

private:
  Adafruit_SSD1306 display;
  TipoTela telaAtual;

  void desenharTela1() {
    display.clearDisplay();

    display.drawRect(
      0,
      0,
      SCREEN_WIDTH,
      SCREEN_HEIGHT,
      SSD1306_WHITE
    );

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(5, 5);
    display.println("TELA 1");

    display.display();
  }

  void desenharTela2() {
    display.clearDisplay();

    display.drawRect(
      0,
      0,
      SCREEN_WIDTH,
      SCREEN_HEIGHT,
      SSD1306_WHITE
    );

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(5, 5);
    display.println("TELA 2");

    display.display();
  }



public:
  Tela()
    : display(
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        &Wire,
        -1
      ),
      telaAtual(TELA_1) {
  }

  bool begin() {
    Wire.begin(SDA_PIN, SCL_PIN);

    delay(100);

    if (!display.begin(
          SSD1306_SWITCHCAPVCC,
          SCREEN_ADDRESS
        )) {
      return false;
    }

    atualizar();
    return true;
  }

  void atualizar() {
    switch (telaAtual) {

      case TELA_1:
        desenharTela1();
        break;

      case TELA_2:
        desenharTela2();
        break;

    }
  }

  void mostrar(TipoTela novaTela) {
    telaAtual = novaTela;
    atualizar();
  }

  void proxima() {
    switch (telaAtual) {

      case TELA_1:
        telaAtual = TELA_2;
        break;

      case TELA_2:
        telaAtual = TELA_1;
        break;

    }

    atualizar();
  }
  void desligar() {
    display.clearDisplay();
    display.display();

    display.ssd1306_command(
        SSD1306_DISPLAYOFF
    );
}

  TipoTela atual() {
    return telaAtual;
  }
};

#endif