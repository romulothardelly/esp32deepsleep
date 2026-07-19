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
  static const uint16_t COR_PADRAO = SSD1306_WHITE;
  static const uint8_t TAMANHO_PADRAO = 1;

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

    display.setTextColor(COR_PADRAO);
    display.setTextSize(TAMANHO_PADRAO);
    return true;
  }
  void desligar() {
    display.clearDisplay();
    display.display();

    display.ssd1306_command(
      SSD1306_DISPLAYOFF);
  }
  void escreverLinha(const char* texto,int x,int y){
    display.setCursor(x, y);
    //display.setTextColor(cor);
   // display.setTextSize(tamanho);
    display.print(texto);
 }
  void escreverLinha(const String& texto, int x, int y){
    escreverLinha(texto.c_str(), x, y);
  }
  void apagar(){
    display.clearDisplay();
  }
  void mostrar(){
    display.display();
  }

};

#endif