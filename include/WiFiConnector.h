/**
 * @Author: Alexander Silva Barbosa
 * @Date:   2024-07-13 15:44:53
 * @Last Modified by:   Alexander Silva Barbosa
 * @Last Modified time: 2024-07-13 22:36:23
 */
#ifndef WiFiConnector_h
#define WiFiConnector_h

#include <Arduino.h>
#include <LittleFS.h>
#include <FS.h>
#include <WiFiManager.h>
#ifdef ESP32
#include <SPIFFS.h>
#endif
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <vector>

#include "WiFiParameter.h"

class WiFiConnector
{
public:
    WiFiConnector();
    WiFiConnector(int button);
    WiFiConnector(int button, int status);
    WiFiConnector(int button, int status, const char *fileName);
    virtual ~WiFiConnector();

    const char *getTitle();
    void setTitle(const char *title);
    const char *getHostName();
    void setHostName(const char *hostname);
    const char *getSettingsFile();
    void setSettingsFile(const char *fileName);
    int getButton();
    void setButton(int button);
    int getStatus();
    void setStatus(int status);
    const char *getHTMLHeader();
    void setHTMLHeader(const char *header);
    float getTimeout();
    void setTimeout(float seconds);
    float getStartTimeout();
    void setStartTimeout(float seconds);

    void setAP(const char *name, const char *password = nullptr);
    void setStream(Stream *stream);

    bool init();
    void addHTML(const char *value);
    void addParam(const char *id, const char *label, const char *defaultValue);
    void addParam(const char *id, const char *label, int defaultValue);

    int getInt(const char *id);
    const char *getStr(const char *id);
    bool get(const char *id, int *value);
    bool get(const char *id, const char **value);

    void setStatic(const char *address, const char *gateway, const char *subnet = nullptr);
    void setDHCP();

    void configureStatus(float configure, float update);

    bool connected();
    void startPortal();
    bool configure();
    bool ready();
    bool valid();
    bool getButtonValue();
    bool shouldSetup();

    void reset(bool clear = false);
    void clear();

private:
    WiFiManager wm;
    std::vector<WiFiParameter> params;
    Ticker timer;
    Stream *stream;
    const char *apName;
    const char *apPassword;
    const char *title;
    const char *hostname;
    const char *fileName;
    const char *address;
    const char *gateway;
    const char *subnet;
    const char *header;
    int button;
    bool buttonValue;
    int status;
    bool isConfiguring;
    bool isUpdating;
    unsigned long startTime;
    float timeout;
    float startTimeout;
    bool isValid;
    bool shouldStartPortal;
    bool isCheckingButton;
    float configureInterval;
    float updateInterval;
    bool justDone;
    void loadParams();
    void saveParams();
    void resetParams();
    void onStartOTA();
    void notify();
};

#endif
