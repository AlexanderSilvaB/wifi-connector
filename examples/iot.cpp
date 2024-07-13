/**
 * @Author: Alexander Silva Barbosa
 * @Date:   2024-07-13 15:00:30
 * @Last Modified by:   Alexander Silva Barbosa
 * @Last Modified time: 2024-07-14 00:53:04
 */

#include <Arduino.h>
#include <WiFiConnector.h>
#include <SinricPro.h>
#include <SinricProContactsensor.h>
#include <ESP8266WiFi.h>

#define APP_KEY "YOUR-APP-KEY"
#define APP_SECRET "YOUR-APP-SECRET"
#define DEVICE_ID "YOUR-DEVICE-ID"

WiFiConnector conn;
WiFiClient client;

const char *device_id;
const char *app_key;
const char *app_secret;

bool lastState = false;
unsigned long lastChange = 0;

void setupIOT();
void handleDevice();

void setup()
{
    Serial.begin(115200);
    Serial.println();

    conn.setStream(&Serial);
    conn.setTitle("IoT Device");
    conn.setHostName("iot-device");
    conn.setAP("IoT Device");
    conn.setStatic("192.168.0.1", "192.168.0.1", "255.255.255.0");

    conn.addParam("device_id", "SinricPro Device ID", DEVICE_ID);
    conn.addParam("app_key", "SinricPro App Key", APP_KEY);
    conn.addParam("app_secret", "SinricPro App Secret", APP_SECRET);

    conn.setStartTimeout(5);
    conn.init();
}

void loop()
{
    if (conn.configure())
        return;

    if (conn.connected())
    {
        if (conn.shouldSetup())
            setupIOT();

        handleDevice();
        SinricPro.handle();
    }
}

void setupIOT()
{
    Serial.println("Configuring device...");

    device_id = conn.getStr("device_id");
    app_key = conn.getStr("app_key");
    app_secret = conn.getStr("app_secret");

    Serial.printf("Device ID: %s\n", device_id);
    Serial.printf("App Key: %s\n", app_key);
    Serial.printf("App Secret: %s\n", app_secret);

    SinricProContactsensor &device = SinricPro[device_id];

    SinricPro.onConnected([]()
                          { Serial.printf("Connected to SinricPro\r\n"); });
    SinricPro.onDisconnected([]()
                             { Serial.printf("Disconnected from SinricPro\r\n"); });
    SinricPro.begin(app_key, app_secret);
}

void handleDevice()
{
    unsigned long actualMillis = millis();
    if (actualMillis - lastChange < 500)
        return;

    if (conn.getButtonValue())
    {
        lastChange = actualMillis;
        lastState = !lastState;
        Serial.printf("Device is %s now\r\n", lastState ? "ON" : "OFF");

        SinricProContactsensor &device = SinricPro[device_id];
        bool success = device.sendContactEvent(lastState);
        if (!success)
        {
            Serial.printf("Something went wrong...could not send Event to server!\r\n");
        }
    }
}