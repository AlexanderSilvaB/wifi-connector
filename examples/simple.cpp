/**
 * @Author: Alexander Silva Barbosa
 * @Date:   2024-07-13 15:58:53
 * @Last Modified by:   Alexander Silva Barbosa
 * @Last Modified time: 2024-07-13 19:55:17
 */

#include <Arduino.h>
#include <WiFiConnector.h>

WiFiConnector conn;

void setup()
{
    Serial.begin(115200);
    Serial.println();
    Serial.println("Starting up...");

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