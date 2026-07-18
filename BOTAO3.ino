#include "Sistema.h"

Sistema sistema;

/*
#define PINO_BOTAO GPIO_NUM_6  //Botao usado para reiniciar
#define TEMPO_ACORDADO 10000  // 10 segundos para dormir se nao tiver acao

Botao botaoMenu(6);  // criar botao na GPIO6
Tela tela;           //Criar tela
Net net("SATURNO24G", "19AURORA11@");
Blue bluetooth("ESP32-S3-RGB");
unsigned long momentoInicial;
void dormir();//definir funcao de dormir

*/
void setup() {

  sistema.init();

/*
  Serial.begin(115200);// inicio da porta serial para debug
  delay(1000);

  neopixelWrite(RGB_BUILTIN, 0, 0, 0);

  Serial.println("Iniciando BLE UART...");



  botaoMenu.begin();// inicializacao do botao

  //teste se a tela esta ok
  if (!tela.begin()) {
    Serial.println("Falha ao inicializar o display");

    while (true) {
      delay(100);
    }
  }

//teste se o bluetooth esta ok
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


  momentoInicial = millis();

  // Mostra por que o ESP32 acordou.
  esp_sleep_wakeup_cause_t motivo =
    esp_sleep_get_wakeup_cause();

  if (motivo == ESP_SLEEP_WAKEUP_EXT0) {
    Serial.println("Acordou pelo botao");
  } else {
    Serial.println("Ligacao normal");
  }*/
}




void loop() {

  sistema.run();

/*
  if (botaoMenu.clicou()) {
    tela.proxima();
    momentoInicial = millis();
    Serial.println("Tela alterada");
  }

  if (bluetooth.available()) {
    String comando = bluetooth.read();

    comando.trim();
    comando.toUpperCase();

    Serial.print("Recebido: [");
    Serial.print(comando);
    Serial.println("]");

    if (comando == "TELA1") {
      tela.mostrar(TELA_1);
      bluetooth.enviar("Tela 1 selecionada");
      bluetooth.enviar(net.getData());

    }

    else if (comando == "TELA2") {
      tela.mostrar(TELA_2);
      bluetooth.enviar("Tela 2 selecionada");
      bluetooth.enviar(net.getHora());
    }

    else if (comando == "PROXIMA") {
      tela.proxima();
      bluetooth.enviar("Proxima tela selecionada");
    }

    else {
      bluetooth.enviar("Comando desconhecido");
    }
    momentoInicial = millis();
  }
  // Após 10 segundos sem interação, dorme.
  if (millis() - momentoInicial >= TEMPO_ACORDADO) {
    dormir();
  }*/
}
/*
void dormir() {
  Serial.println("Entrando em deep sleep...");

  // Desliga visualmente o display.
  tela.desligar();

  net.disconnect();*/

  /*
     * Evita entrar no sono enquanto o botão estiver pressionado.
     * Caso contrário, o nível LOW poderia provocar despertar imediato.
     */
 /* while (digitalRead(PINO_BOTAO) == LOW) {
    delay(10);
  }*/

  /*
     * Configura a GPIO6 para acordar o ESP32 quando ficar LOW.
     *
     * Botão ligado:
     * GPIO6 ---- botão ---- GND
     */
  /*esp_sleep_enable_ext0_wakeup(
    PINO_BOTAO,
    0);

  delay(100);

  // O código para aqui e o ESP32 entra em deep sleep.
  esp_deep_sleep_start();
}*/
