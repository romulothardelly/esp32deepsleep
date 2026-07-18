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



class Tela {

private:
  Adafruit_SSD1306 display;

  void criarBorda() {
    display.drawRect(
      0,
      0,
      SCREEN_WIDTH,
      SCREEN_HEIGHT,
      SSD1306_WHITE);
  }


public:
  Tela()
    : display(
      SCREEN_WIDTH,
      SCREEN_HEIGHT,
      &Wire,
      -1){
  }

  bool begin() {
    Wire.begin(SDA_PIN, SCL_PIN);

    delay(100);

    if (!display.begin(
          SSD1306_SWITCHCAPVCC,
          SCREEN_ADDRESS)) {
      return false;
    }
    return true;
  }
 

  void desligar() {
    display.clearDisplay();
    display.display();

    display.ssd1306_command(
      SSD1306_DISPLAYOFF);
  }


  void escreverTexto(
    const String& texto,
    int x = 0,
    int yInicial = 0,
    int espacamento = 8) {
    int inicio = 0;
    int y = yInicial;

    display.clearDisplay();
    
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setTextWrap(false);

    criarBorda();

    while (inicio < texto.length()) {

      int fim = texto.indexOf('\n', inicio);

      if (fim == -1) {
        fim = texto.length();
      }

      display.setCursor(x, y);
      display.print(texto.substring(inicio, fim));

      y += espacamento;
      inicio = fim + 1;
    }
    display.display();
  }
};

#endif