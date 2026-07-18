#ifndef SISTEMA_H
#define SISTEMA_H
#include "Net.h"
#include "Tela.h"
#include "Blue.h"
#include "Botao.h"
#include <DHT.h>

#define DHTPIN 4       // GPIO onde o DHT11 está ligado
#define DHTTYPE DHT11  // Tipo do sensor
class Sistema {
private:
  Net net;
  Tela tela;
  Blue bluetooth;
  Botao botaoMenu;
  String bluemsg;
  DHT dht;
  static const uint8_t DHT_PIN = 4;
  static const uint8_t DHT_TYPE = DHT11;
  
  uint32_t ultimaTrocaTela = 0;
  uint8_t telaAtual = 0;
  static const uint32_t INTERVALO_TELA = 10000;

 void telaInit(){
   Serial.println("Tela inicial");
      String status =
        "     Bem vindo\n"
        "      SISTEMA\n "
        "        DE\n "
        "   MONITORAMENTO\n"
        "    by Thardelly";
      tela.escreverTexto(status, 5, 5, 11);
      bluetooth.enviar("Tela inicial");
      bluetooth.enviar(net.getHora());
 }
  void telaInfo(){
     Serial.println("Executando telaInfo");
      String status =
        "       Status\n"
        "WiFi: "
        + String(net.isConnected() ? "OK" : "NOP") + "\n"
                                                     "Rede: \n"
                                                     ""
        + net.getName() + "\n"
                          "IP: "
        + net.getIP() + "";
      tela.escreverTexto(status, 5, 5, 11);
      bluetooth.enviar("Tela info selecionada");
      bluetooth.enviar(net.getHora());
    
  }

  void telaHora(){
    String status =
        "    Dia e Hora\n"
        "Dia: "+net.getData()+"\n"
        "Hora: "+net.getHora()+"\n";
      tela.escreverTexto(status, 5, 5, 11);
      bluetooth.enviar("Tela dia e hora selecionada");
      bluetooth.enviar(net.getHora());

  }

  void telaTempo(){
    float temperatura = dht.readTemperature();
    float umidade = dht.readHumidity();
    
    String status =
        "       Dados\n"
        "\n"
        "Temp: "+String(temperatura,1)+" oC\n"
        "Hum:  "+String(umidade,0)+"%\n";
      tela.escreverTexto(status, 5, 5, 11);
      bluetooth.enviar("Tela dia e hora selecionada");
      bluetooth.enviar(net.getHora());

  }
  bool getblue() {

    if (bluetooth.available()) {
      bluemsg = bluetooth.read();

      bluemsg.trim();
      bluemsg.toUpperCase();

      Serial.print("Recebido: [");
      Serial.print(bluemsg);
      Serial.println("]");
      return true;
    }
    return false;
  }

  void writetela() {
    if (bluemsg == "TELAINFO") {
      telaInfo();
      Serial.println("Executando telaInfo");
     
    }else if (bluemsg == "TELATIME") {
      Serial.println("Executando telaTIME");
      telaHora();
    }
    else {
      bluetooth.enviar("Comando desconhecido");
    }
  }

  void desenharTelaAtual() {

    switch (telaAtual) {

        case 0:
            telaInit();
            break;

        case 1:
            telaHora();
            break;
        case 2:
            telaInfo();
            break;
        case 3:
            telaTempo();
            break;
    }
}




public:
  Sistema()
    : net("SATURNO24G", "19AURORA11@"),
      tela(),
      bluetooth("ESP32-S3-RGB"),
      botaoMenu(6),
      dht(DHT_PIN, DHT_TYPE) {
  }

  void init() {

    Serial.begin(115200);

    botaoMenu.begin();

    if (!tela.begin()) {
      Serial.println("Falha ao inicializar o display");

      while (true) {
        delay(100);
      }
    }


    

    if (!bluetooth.begin()) {
      Serial.println("Erro ao iniciar Bluetooth");

      while (true) {
        delay(100);
      }
    }

    if (net.connect()) {
      net.configurarHora();
      Serial.println(net.getData());
      Serial.println(net.getHora());
    }
    

    ultimaTrocaTela = millis();
    desenharTelaAtual();
  }
  void run() {



    if (botaoMenu.clicou()) {
     
      Serial.println("Tela alterada");
    }


if (millis() - ultimaTrocaTela >= INTERVALO_TELA) {

        ultimaTrocaTela = millis();

        telaAtual++;

        if (telaAtual > 3) {   // atualmente existem 2 telas
            telaAtual = 0;
        }

        desenharTelaAtual();
    }



    if (getblue()) {
      writetela();
    }
    
    

  }

  

  


};
#endif