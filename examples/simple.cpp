/**
 * @Author: Alexander Silva Barbosa
 * @Date:   2024-07-13 15:58:53
 * @Last Modified by:   Alexander Silva Barbosa
 * @Last Modified time: 2024-07-13 20:34:48
 */

#include <Arduino.h>
#include <WiFiConnector.h>

WiFiConnector conn;

void setup()
{
    Serial.begin(115200);
    Serial.println();

    conn.setStream(&Serial);
    conn.init();
}

void loop()
{
    if (conn.ready())
    {
        Serial.println("Ready!");
        delay(200);
    }
    else
    {
        Serial.println("Configuring...");
        delay(100);
    }
}