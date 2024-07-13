/**
 * @Author: Alexander Silva Barbosa
 * @Date:   2024-07-13 15:58:53
 * @Last Modified by:   Alexander Silva Barbosa
 * @Last Modified time: 2024-07-13 20:34:56
 */

#include <Arduino.h>
#include <WiFiConnector.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

WiFiConnector conn;
WiFiClient client;
PubSubClient mqtt(client);

const char *client_id;
const char *server;
int port;
bool setup_done = false;

void setup()
{
    Serial.begin(115200);
    Serial.println();

    conn.setStream(&Serial);

    conn.setTitle("MQTT");
    conn.setHostName("mqtt");
    conn.setAP("MQTT Setup");

    conn.addParam("client_id", "MQTT Client ID", "IotClient");
    conn.addParam("server", "MQTT Server", "");
    conn.addParam("port", "MQTT Port", 1883);

    conn.init();
}

void loop()
{
    if (conn.ready())
    {
        if (!setup_done)
        {
            client_id = conn.getStr("client_id");
            server = conn.getStr("server");
            port = conn.getInt("port");

            Serial.printf("Configuring MQTT: %s:%d\r\n", server, port);
            mqtt.disconnect();
            mqtt.setServer(server, port);

            setup_done = true;
        }
        else
        {
            if (!mqtt.connected())
            {
                Serial.printf("Connecting MQTT: Client ID = %s...\r\n", client_id);
                if (mqtt.connect(client_id))
                {
                    Serial.println("Connected!");
                }
                else
                {
                    Serial.println("Failed!");
                    delay(2000);
                }
            }
        }
    }
}