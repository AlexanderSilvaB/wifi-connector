/**
 * @Author: Alexander Silva Barbosa
 * @Date:   2024-07-13 15:23:21
 * @Last Modified by:   Alexander Silva Barbosa
 * @Last Modified time: 2024-07-13 20:31:33
 */

#include "WiFiConnector.h"

using namespace std;

WiFiConnector::WiFiConnector() : WiFiConnector(D3) {}
WiFiConnector::WiFiConnector(int button) : WiFiConnector(button, LED_BUILTIN) {}
WiFiConnector::WiFiConnector(int button, int status) : WiFiConnector(button, status, "/config.json") {}
WiFiConnector::WiFiConnector(int button, int status, const char *fileName)
{
    this->setTitle("WiFi Connector");
    this->setHostName("wifi-connector");
    this->setSettingsFile(fileName);
    this->setButton(button);
    this->setStatus(status);
    this->setStream(nullptr);

    this->setDHCP();
    this->setHTMLHeader(nullptr);
    this->setAP(this->getTitle());

    this->configureStatus(0.2, 0.05);

    this->setTimeout(120);
    this->isConfiguring = false;
    this->isValid = false;
    this->isUpdating = false;
    this->startTime = millis();
}

WiFiConnector::~WiFiConnector()
{
}

const char *WiFiConnector::getSettingsFile()
{
    return this->fileName;
}

void WiFiConnector::setSettingsFile(const char *fileName)
{
    this->fileName = fileName;
}

const char *WiFiConnector::getTitle()
{
    return this->title;
}

void WiFiConnector::setTitle(const char *title)
{
    this->title = title;
}

const char *WiFiConnector::getHostName()
{
    return this->hostname;
}

void WiFiConnector::setHostName(const char *hostname)
{
    this->hostname = hostname;
}

int WiFiConnector::getButton()
{
    return this->button;
}

void WiFiConnector::setButton(int button)
{
    this->button = button;
}

int WiFiConnector::getStatus()
{
    return this->status;
}

void WiFiConnector::setStatus(int status)
{
    this->status = status;
}

void WiFiConnector::setStatic(const char *address, const char *gateway, const char *subnet)
{
    this->address = address;
    this->gateway = gateway;
    if (!address || subnet)
        this->subnet = subnet;
    else
        this->subnet = "255.255.255.0";
}

void WiFiConnector::setDHCP()
{
    this->setStatic(nullptr, nullptr, nullptr);
}

const char *WiFiConnector::getHTMLHeader()
{
    return this->header;
}

void WiFiConnector::setHTMLHeader(const char *header)
{
    this->header = header;
}

float WiFiConnector::getTimeout()
{
    return this->timeout;
}

void WiFiConnector::setTimeout(float seconds)
{
    this->timeout = seconds;
}

void WiFiConnector::setAP(const char *name, const char *password)
{
    this->apName = name;
    this->apPassword = password;
}

void WiFiConnector::configureStatus(float configure, float update)
{
    this->configureInterval = configure;
    this->updateInterval = update;
}

bool WiFiConnector::init()
{
    this->isValid = false;

    if (this->stream)
    {
        this->stream->printf("[WiFi Connector] Starting up...\n");
    }

    if (!LittleFS.begin())
    {
        if (this->stream)
        {
            this->stream->printf("[WiFi Connector] FS mounting failed!\n");
        }
        return false;
    }
    else
    {
        if (this->stream)
        {
            this->stream->printf("[WiFi Connector] FS mounted!\n");
        }
    }

    this->loadParams();

    WiFi.mode(WIFI_STA);

    if (this->getStatus() >= 0)
    {
        pinMode(this->getStatus(), OUTPUT);
        if (this->stream)
        {
            this->stream->printf("[WiFi Connector] Status configured on PIN %d!\n", this->getStatus());
        }
        if (this->configureInterval > 0)
        {
            this->timer.attach(this->configureInterval, std::bind(&WiFiConnector::notify, this));
            if (this->stream)
            {
                this->stream->printf("[WiFi Connector] Status PIN  Interval = %f!\n", this->configureInterval);
            }
        }
    }
    else
    {
        if (this->stream)
        {
            this->stream->printf("[WiFi Connector] No status PIN!\n");
        }
    }

    if (this->getButton() >= 0)
    {
        pinMode(this->getButton(), INPUT_PULLUP);
        if (this->stream)
        {
            this->stream->printf("[WiFi Connector] Button configured on PIN %d!\n", this->getButton());
        }
    }
    else
    {
        if (this->stream)
        {
            this->stream->printf("[WiFi Connector] No button PIN!\n");
        }
    }

    wm.setTitle(this->getTitle());
    wm.setParamsPage(this->params.size() > 0);
    wm.setDarkMode(true);
    wm.setHostname(this->hostname);
    for (vector<WiFiParameter>::iterator it = this->params.begin(); it != this->params.end(); it++)
    {
        wm.addParameter(it->param);
    }
    wm.setSaveParamsCallback(std::bind(&WiFiConnector::saveParams, this));
    wm.setConfigResetCallback(std::bind(&WiFiConnector::resetParams, this));

    if (this->address && this->gateway && this->subnet)
    {
        IPAddress address;
        IPAddress gateway;
        IPAddress subnet;

        if (!address.fromString(this->address))
            return false;
        if (!gateway.fromString(this->gateway))
            return false;
        if (!subnet.fromString(this->subnet))
            return false;

        wm.setAPStaticIPConfig(address, gateway, subnet);
    }

    if (this->header)
        wm.setCustomHeadElement(this->header);

    wm.setPreOtaUpdateCallback(std::bind(&WiFiConnector::onStartOTA, this));
    wm.autoConnect(this->apName, this->apPassword);

    this->isValid = true;
    if (this->stream)
    {
        this->stream->printf("[WiFi Connector] Ready!\n");
    }
    return true;
}

bool WiFiConnector::valid()
{
    return this->isValid;
}

bool WiFiConnector::configure()
{
    if (!this->isValid)
        return false;

    if (this->isConfiguring)
    {
        wm.process();
        this->isConfiguring = wm.getConfigPortalActive();
        if ((millis() - this->startTime) > (unsigned int)(this->timeout * 1000))
        {
            if (this->stream)
            {
                this->stream->printf("[WiFi Connector] Setup portal timeout!\n");
            }
            this->isConfiguring = false;
        }
        if (!this->isConfiguring)
            wm.stopConfigPortal();
    }

    if (this->getButton() >= 0 && digitalRead(this->button) == LOW && !this->isConfiguring)
    {
        if (this->stream)
        {
            this->stream->printf("[WiFi Connector] Starting setup portal!\n");
        }

        wm.setConfigPortalBlocking(false);
        wm.startConfigPortal(this->apName, this->apPassword);
        this->isConfiguring = true;
        this->startTime = millis();
    }

    return this->isConfiguring;
}

bool WiFiConnector::ready()
{
    if (!this->isValid)
        return false;

    return !this->configure();
}

void WiFiConnector::addHTML(const char *text)
{
    this->params.push_back(WiFiParameter(text));
}

void WiFiConnector::addParam(const char *id, const char *label, const char *defaultValue)
{
    this->params.push_back(WiFiParameter(id, label, defaultValue));
}

void WiFiConnector::addParam(const char *id, const char *label, int defaultValue)
{
    this->params.push_back(WiFiParameter(id, label, defaultValue));
}

int WiFiConnector::getInt(const char *id)
{
    int value = 0;
    this->get(id, &value);
    return value;
}

const char *WiFiConnector::getStr(const char *id)
{
    const char *value = nullptr;
    this->get(id, &value);
    return value;
}

bool WiFiConnector::get(const char *id, int *value)
{
    for (vector<WiFiParameter>::iterator it = this->params.begin(); it != this->params.end(); it++)
    {
        if (!it->isParam() || !it->is_int)
            continue;
        if (strcmp(it->id(), id) == 0)
        {
            *value = it->getInt();
            return true;
        }
    }
    return false;
}

bool WiFiConnector::get(const char *id, const char **value)
{
    for (vector<WiFiParameter>::iterator it = this->params.begin(); it != this->params.end(); it++)
    {
        if (!it->isParam() || !it->is_string)
            continue;
        if (strcmp(it->id(), id) == 0)
        {
            *value = it->getStr();
            return true;
        }
    }
    return false;
}

void WiFiConnector::loadParams()
{
    if (this->stream)
    {
        this->stream->printf("[WiFi Connector] Reading parameters file: %s\n", this->getSettingsFile());
    }
    File file = LittleFS.open(this->getSettingsFile(), "r");
    if (!file)
    {
        if (this->stream)
        {
            this->stream->print("[WiFi Connector] Parameters file not found!\n");
        }
        return;
    }

    JsonDocument doc;
    deserializeJson(doc, file);
    file.close();

    for (vector<WiFiParameter>::iterator it = this->params.begin(); it != this->params.end(); it++)
    {
        if (!it->isParam())
            continue;

        if (doc.containsKey(it->id()))
        {
            if (it->is_string)
            {
                it->set((const char *)doc[it->id()]);
            }
            else if (it->is_int)
            {
                it->set((int)doc[it->id()]);
            }
        }
    }

    if (this->stream)
    {
        this->stream->printf("[WiFi Connector] Parameters loaded!\n");
        serializeJson(doc, *this->stream);
        this->stream->printf("\n");
    }
}

void WiFiConnector::saveParams()
{
    JsonDocument doc;
    for (vector<WiFiParameter>::iterator it = this->params.begin(); it != this->params.end(); it++)
    {
        if (!it->isParam())
            continue;

        if (it->is_string)
        {
            doc[it->id()] = it->getStr();
        }
        else if (it->is_int)
        {
            doc[it->id()] = it->getInt();
        }
    }

    if (this->stream)
    {
        this->stream->printf("[WiFi Connector] Writing parameters to file: %s\n", this->getSettingsFile());
    }

    File file = LittleFS.open(this->getSettingsFile(), "w");
    serializeJson(doc, file);
    file.close();

    if (this->stream)
    {
        this->stream->printf("[WiFi Connector] Parameters saved!\n");
        serializeJson(doc, *this->stream);
        this->stream->printf("\n");
    }
}

void WiFiConnector::resetParams()
{
    for (vector<WiFiParameter>::iterator it = this->params.begin(); it != this->params.end(); it++)
    {
        if (!it->isParam())
            continue;

        if (it->is_string)
        {
            it->set(it->default_str);
        }
        else if (it->is_int)
        {
            it->set(it->default_int);
        }
    }
    this->saveParams();
}

void WiFiConnector::onStartOTA()
{
    if (this->stream)
    {
        this->stream->printf("[WiFi Connector] Starting OTA Update...\n");
    }
    this->timer.detach();
    this->isUpdating = true;
    if (this->updateInterval > 0)
        this->timer.attach(this->updateInterval, std::bind(&WiFiConnector::notify, this));
}

void WiFiConnector::notify()
{
    if (this->getStatus() < 0)
        return;

    if (this->isUpdating)
    {
        digitalWrite(this->status, !digitalRead(this->status));
    }
    else if (this->isConfiguring)
    {
        digitalWrite(this->status, !HIGH);
    }
    else
    {
        digitalWrite(this->status, !LOW);
    }
}