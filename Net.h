#ifndef NET_H
#define NET_H

#include <WiFi.h>
#include <time.h>
#include <HTTPClient.h>

class Net {
private:
    String wifiName;
    String password;
    String IP;
    inline static const char* servidor = "https://romulothardelly.com/environment/receber.php";
    inline static const char* apiKey =    "ESP32_CHAVE_123456";

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
        wifiName=WiFi.SSID();
        IP=WiFi.localIP().toString();

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

    bool getDataHora(char* data,size_t size_data,char* hora,size_t size_hora){

         struct tm timeinfo;

        if (!getLocalTime(&timeinfo))
        {
            snprintf(data, size_data, "--/--/----");
            snprintf(hora, size_hora, "--:--:--");
            return false;
        }

        strftime(
            data,
            size_data,
            "%d/%m/%Y",
            &timeinfo
        ); 

        strftime(
        hora,
        size_hora,
        "%H:%M:%S",
        &timeinfo
        );

        return true;
    }

    bool isConnected() {
        return WiFi.status() == WL_CONNECTED;
    }

    
    const char* getName(){        
        return wifiName.c_str();
    }


    
    const char* getIP(){
        return IP.c_str();
    }

    void disconnect() {
        WiFi.disconnect();
        WiFi.mode(WIFI_OFF);
    }
    void setup(String name,String p){
        wifiName=name;
        password=p;
    }
    /*bool enviarDados(float temperatura, float umidade) {

        HTTPClient http;

        http.begin(servidor);

        http.addHeader(
            "Content-Type",
            "application/json");

        String json = "{";
        json += "\"api_key\":\"";
        json += apiKey;
        json += "\",";

        json += "\"temperature\":";
        json += String(temperatura, 2);
        json += ",";

        json += "\"humidity\":";
        json += String(umidade, 2);

        json += "}";

        Serial.println("Enviando:");
        //Serial.println(json);

        int codigoHttp = http.POST(json);

        if (codigoHttp > 0) {
            String resposta = http.getString();

            //Serial.print("Código HTTP: ");
            //Serial.println(codigoHttp);

            //Serial.print("Resposta: ");
           // Serial.println(resposta);

            http.end();

            return codigoHttp == 200 || codigoHttp == 201;
        }

        Serial.print("Erro na requisição: ");
        //Serial.println(http.errorToString(codigoHttp));

        http.end();

        return false;
    }*/

    bool enviarDados(float temperatura, float umidade)
{
    HTTPClient http;

    if (!http.begin(servidor))
    {
        Serial.println("Erro ao iniciar HTTP.");
        return false;
    }

    http.addHeader(
        "Content-Type",
        "application/json"
    );

    char json[128];

    int tamanho = snprintf(
        json,
        sizeof(json),
        "{\"api_key\":\"%s\",\"temperature\":%.2f,\"humidity\":%.2f}",
        apiKey,
        temperatura,
        umidade
    );

    if (tamanho < 0 || tamanho >= sizeof(json))
    {
        Serial.println("Buffer JSON insuficiente.");
        http.end();
        return false;
    }

    Serial.println("Enviando:");
    Serial.println(json);

    int codigoHttp = http.POST(
        reinterpret_cast<uint8_t*>(json),
        strlen(json)
    );

    bool sucesso =
        codigoHttp == HTTP_CODE_OK ||
        codigoHttp == HTTP_CODE_CREATED;

    Serial.print("Codigo HTTP: ");
    Serial.println(codigoHttp);

    http.end();

    return sucesso;
}
};

#endif