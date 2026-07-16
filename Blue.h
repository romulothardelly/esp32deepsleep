#ifndef BLUE_H
#define BLUE_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

class Blue {

private:
  // UUIDs do serviço UART BLE.
  static constexpr const char* SERVICE_UUID =
    "6E400001-B5A3-F393-E0A9-E50E24DCCA9E";

  // Celular envia dados para o ESP32.
  static constexpr const char* RX_UUID =
    "6E400002-B5A3-F393-E0A9-E50E24DCCA9E";

  // ESP32 envia dados para o celular.
  static constexpr const char* TX_UUID =
    "6E400003-B5A3-F393-E0A9-E50E24DCCA9E";

  String nome;

  BLEServer* server = nullptr;
  BLEService* service = nullptr;
  BLECharacteristic* txChar = nullptr;
  BLECharacteristic* rxChar = nullptr;

  String ultimoValorRecebido;
  bool novoValorRecebido = false;
  bool dispositivoConectado = false;
  bool iniciado = false;

  /*
     * Classe interna responsável pelo evento de recebimento.
     *
     * Ela recebe um ponteiro para o objeto Blue ao qual pertence.
     * Assim, consegue atualizar ultimoValorRecebido.
     */
  class RXCallback : public BLECharacteristicCallbacks {

  private:
    Blue* blue;

  public:
    explicit RXCallback(Blue* blue)
      : blue(blue) {
    }

    void onWrite(
      BLECharacteristic* characteristic) override {
      String valor = characteristic->getValue();

      if (valor.length() > 0) {
        blue->ultimoValorRecebido = valor;
        blue->novoValorRecebido = true;
      }
    }
  };

  /*
     * Eventos de conexão e desconexão.
     */
  class ServerCallback : public BLEServerCallbacks {

  private:
    Blue* blue;

  public:
    explicit ServerCallback(Blue* blue)
      : blue(blue) {
    }

    void onConnect(BLEServer* server) override {
      blue->dispositivoConectado = true;
    }

    void onDisconnect(BLEServer* server) override {
      blue->dispositivoConectado = false;

      // Volta a anunciar para permitir nova conexão.
      server->startAdvertising();
    }
  };

  RXCallback rxCallback;
  ServerCallback serverCallback;

public:
  /*
     * Construtor.
     *
     * Exemplo:
     * Blue bluetooth("ESP32-S3-RGB");
     */
  explicit Blue(const String& nome)
    : nome(nome),
      rxCallback(this),
      serverCallback(this) {
  }

  /*
     * Inicializa o Bluetooth.
     *
     * Retorna true se a inicialização foi concluída.
     */
  bool begin() {
    if (iniciado) {
      return true;
    }

    // Inicializa o BLE e define o nome do dispositivo.
    BLEDevice::init(nome.c_str());

    // Cria o servidor BLE.
    server = BLEDevice::createServer();

    if (server == nullptr) {
      return false;
    }

    server->setCallbacks(&serverCallback);

    // Cria o serviço.
    service = server->createService(SERVICE_UUID);

    if (service == nullptr) {
      return false;
    }

    // Característica TX: ESP32 -> celular.
    txChar = service->createCharacteristic(
      TX_UUID,
      BLECharacteristic::PROPERTY_NOTIFY);

    if (txChar == nullptr) {
      return false;
    }

    // Permite que o celular habilite notificações.
    txChar->addDescriptor(new BLE2902());

    // Característica RX: celular -> ESP32.
    rxChar = service->createCharacteristic(
      RX_UUID,
      BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR);

    if (rxChar == nullptr) {
      return false;
    }

    // Associa o evento de recebimento.
    rxChar->setCallbacks(&rxCallback);

    // Inicia o serviço.
    service->start();

    // Configura e inicia o anúncio BLE.
    BLEAdvertising* advertising =
      BLEDevice::getAdvertising();

    advertising->addServiceUUID(SERVICE_UUID);
    advertising->setScanResponse(true);

    BLEAdvertisementData dadosAnuncio;
    dadosAnuncio.setName(nome.c_str());
    dadosAnuncio.setCompleteServices(
      BLEUUID(SERVICE_UUID));

    advertising->setAdvertisementData(dadosAnuncio);

    BLEAdvertisementData respostaScan;
    respostaScan.setName(nome.c_str());

    advertising->setScanResponseData(respostaScan);

    advertising->start();

    iniciado = true;

    return true;
  }

  /*
     * Informa se existe uma nova mensagem recebida.
     */
  bool available() const {
    return novoValorRecebido;
  }

  /*
     * Retorna a mensagem recebida.
     *
     * Depois da leitura, available() volta a retornar false.
     */
  String read() {
    if (!novoValorRecebido) {
      return "";
    }

    novoValorRecebido = false;

    return ultimoValorRecebido;
  }

  /*
     * Envia um texto para o celular.
     *
     * Retorna false se não houver conexão.
     */
  bool enviar(const String& texto) {
    if (
      !iniciado || !dispositivoConectado || txChar == nullptr) {
      return false;
    }

    txChar->setValue(texto.c_str());
    txChar->notify();

    return true;
  }

  /*
     * Informa se um celular está conectado.
     */
  bool conectado() const {
    return dispositivoConectado;
  }

  /*
     * Informa se o BLE foi inicializado.
     */
  bool estaIniciado() const {
    return iniciado;
  }
};

#endif