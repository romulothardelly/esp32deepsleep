#ifndef NET_H
#define NET_H

#include <WiFi.h>
#include <time.h>

class Net {
private:
    String wifiName;
    String password;

public:
    Net(const String& wifiName, const String& password)
        : wifiName(wifiName),
          password(password) {
    }

    bool connect(uint32_t timeout = 10000) {
        WiFi.mode(WIFI_STA);
        WiFi.begin(wifiName.c_str(), password.c_str());

        uint32_t inicio = millis();

        while (WiFi.status() != WL_CONNECTED) {
            if (millis() - inicio >= timeout) {
                Serial.println("\nTempo limite da conexao Wi-Fi.");
                return false;
            }

            delay(500);
            Serial.print(".");
        }

        Serial.println("\nWi-Fi conectado!");
        Serial.print("IP: ");
        Serial.println(WiFi.localIP());

        return true;
    }

    void configurarHora() {
        const long fusoHorario = -3 * 3600;
        const int horarioVerao = 0;

        configTime(
            fusoHorario,
            horarioVerao,
            "pool.ntp.org",
            "time.google.com"
        );
    }

    String getHora() {
        struct tm timeinfo;

        if (!getLocalTime(&timeinfo)) {
            return "--:--:--";
        }

        char buffer[9];
        strftime(
            buffer,
            sizeof(buffer),
            "%H:%M:%S",
            &timeinfo
        );

        return String(buffer);
    }

    String getData() {
        struct tm timeinfo;

        if (!getLocalTime(&timeinfo)) {
            return "--/--/----";
        }

        char buffer[11];
        strftime(
            buffer,
            sizeof(buffer),
            "%d/%m/%Y",
            &timeinfo
        );

        return String(buffer);
    }

    bool isConnected() {
        return WiFi.status() == WL_CONNECTED;
    }

    String getName(){
        return WiFi.SSID();
    }
    String getIP(){
        return WiFi.localIP().toString();
    }

    void disconnect() {
        WiFi.disconnect();
        WiFi.mode(WIFI_OFF);
    }
};

#endif