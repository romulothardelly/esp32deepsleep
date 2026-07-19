#ifndef SISTEMA_H
#define SISTEMA_H
#include "Net.h"
#include "Tela.h"
#include "Blue.h"
#include "Botao.h"
#include <DHT.h>
#include <Preferences.h>

class Sistema {
private:
  Net net;
  Tela tela;
  Blue bluetooth;
  Botao botaoMenu;
  String bluemsg;
  DHT dht;
  Preferences preferences;
  String wifiName;
  String wifipassword;
  static const uint8_t DHT_PIN = 4;
  static const uint8_t DHT_TYPE = DHT11;  
  uint32_t ultimaTrocaTela = 0;
  uint8_t telaAtual = 0;
  static const uint32_t INTERVALO_TELA = 10000;

  uint32_t ultimoEvento = 0;
  const uint32_t TEMPO_SLEEP    = 60000;     // 1 min
  const uint32_t TEMPO_WAKE=10;//min

  

  void telaInit() {
    tela.apagar();
    tela.escreverLinha("     Bem vindo",5,5);
    tela.escreverLinha("      SISTEMA",5,16);
    tela.escreverLinha("        DE",5,27);
    tela.escreverLinha("   MONITORAMENTO",5,38);
    tela.escreverLinha("    by Thardelly",5,49);
    tela.mostrar();    
  }
  void telaInfo(){
    tela.apagar();
    tela.escreverLinha("     Status",5,5);
    tela.escreverLinha("WIFI: ",5,16);
    tela.escreverLinha(net.isConnected() ? "OK" : "NOP",41,16);
    tela.escreverLinha("Rede: ",5,27);
    tela.escreverLinha(net.getName(),5,38);
    tela.escreverLinha("IP: ",5,49);
    tela.escreverLinha(net.getIP(),30,49);
    tela.mostrar();    
  }
  void telaHora(){
    char data[11];
    char hora[9];
    
    net.getDataHora(
        data, sizeof(data),
        hora, sizeof(hora));

    tela.apagar();
    tela.escreverLinha("    Dia e hora",5,5);
    tela.escreverLinha("Dia: ",5,16);
    tela.escreverLinha(data,30,16);
    tela.escreverLinha("Hora: ",5,27);
    tela.escreverLinha(hora,38,27);
    tela.mostrar();   

  }
  void telaTempo(){
      float temperatura = dht.readTemperature();
      float umidade = dht.readHumidity();

      char textoTemperatura[16];
      char textoUmidade[16];

      tela.apagar();

      tela.escreverLinha("       Dados", 5, 5);

      if (isnan(temperatura) || isnan(umidade))
      {
          tela.escreverLinha("Erro no sensor DHT", 5, 27);
          tela.mostrar(); 
          return;
      }

      snprintf(
          textoTemperatura,
          sizeof(textoTemperatura),
          "Temp: %.1f C",
          temperatura
      );

      snprintf(
          textoUmidade,
          sizeof(textoUmidade),
          "Hum:  %.0f%%",
          umidade
      );

      tela.escreverLinha(textoTemperatura, 5, 27);
      tela.escreverLinha(textoUmidade, 5, 40);

      tela.mostrar(); 
  }

  bool senddata(){
    float temperatura = dht.readTemperature();
    float umidade = dht.readHumidity();
    return net.enviarDados(temperatura,umidade);
  }

  bool getblue() {

    if (bluetooth.available()) {
      bluemsg = bluetooth.read();

      bluemsg.trim();
      bluemsg.toUpperCase();
      return true;
    }
    return false;
  }

  void desenharTelaAtual() {
    Serial.println("Mudando tela.");
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

  void salvarWifi(String name,String p){

    preferences.begin("config", false);
    preferences.putString("ssid", name);
    preferences.putString("senha", p);
    preferences.end();

  }

  bool checkwifisetup() {

    preferences.begin("config", true);

    wifiName = preferences.getString("ssid", "");
    wifipassword = preferences.getString("senha", "");

    preferences.end();

    if (wifiName.isEmpty() || wifipassword.isEmpty()) {
      Serial.println("Wi-Fi não configurado.");
      return false;
    }
    else{
      net.setup(wifiName,wifipassword);
      return true;
    }
  }

  void connectwifi() {
    if (checkwifisetup()) {
      if (net.connect()) {
        net.configurarHora();
        //Serial.println(net.getData());
        //Serial.println(net.getHora());
      }
    }
  }

public:
  Sistema()
    : net("", ""),
      tela(),
      bluetooth("ESP32-S3-RGB"),
      botaoMenu(GPIO_NUM_6),
      dht(DHT_PIN, DHT_TYPE) {
        wifiName="";
        wifipassword="";
  }
  void init() {

    Serial.begin(115200);

    botaoMenu.begin();

    if (!tela.begin()) {
      while (true) {
        delay(100);
      }
    }

    
    if (!bluetooth.begin()) {
      while (true) {
        delay(100);
      }
    }

    if(checkwifisetup()){
        if (net.connect()) {
        net.configurarHora();
      }
    }   
    

    ultimaTrocaTela = millis();
    ultimoEvento = millis();
    desenharTelaAtual();
    Serial.println("Programa iniciado");
  }
  void run() {


    if (botaoMenu.clicou()) {     
      telaInit();
      telaAtual=0;
      ultimaTrocaTela = millis();
      ultimoEvento = millis();
      Serial.println("Botao clicado");
    }


    if (millis() - ultimaTrocaTela >= INTERVALO_TELA) {

      ultimaTrocaTela = millis();

      telaAtual++;

      if (telaAtual > 3) {  // atualmente existem 2 telas
         
         telaAtual = 0;
      }

      desenharTelaAtual();
    }

    // Deep Sleep por inatividade
    if (millis() - ultimoEvento >= TEMPO_SLEEP) {
      Serial.println("Enviando dados");
      if(senddata()){
        Serial.println("Vou dormir");
        dormir();
      }
      
    }



    if (getblue()) {
      if (bluemsg.startsWith("WIFI=")) {//WIFI=MinhaRede;MinhaSenha123
        String dados = bluemsg.substring(5);

        int p = dados.indexOf(';');

        if (p > 0) {
          String ssid = dados.substring(0, p);
          String senha = dados.substring(p + 1);

          salvarWifi(ssid, senha);

          bluetooth.enviar("WiFi salvo.");
          connectwifi();
        }
      }
      
    }
    
    

  }

  void dormir() {
    


    Serial.println("Apagando tela");
    tela.desligar();
    Serial.println("Apagando wifi");
    net.disconnect();


      /*
      * Evita entrar no sono enquanto o botão estiver pressionado.
      * Caso contrário, o nível LOW poderia provocar despertar imediato.
      */
       while (digitalRead(botaoMenu.getPino()) == LOW) {
        delay(10);
      }

      /*
      * Configura a GPIO6 para acordar o ESP32 quando ficar LOW.
      */
      Serial.println("Configurando botao para acordar.");
    esp_sleep_enable_ext0_wakeup(
      botaoMenu.getPino(),
      0);
    Serial.println("Setando o tempo para acordar.");
    esp_sleep_enable_timer_wakeup(TEMPO_WAKE * 60 * 1000000ULL);

    delay(100);

    // O código para aqui e o ESP32 entra em deep sleep.
    Serial.println("Dormindo .......");
    esp_deep_sleep_start();
  }
};
#endif