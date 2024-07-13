/**
 * @Author: Alexander Silva Barbosa
 * @Date:   2024-07-13 16:35:43
 * @Last Modified by:   Alexander Silva Barbosa
 * @Last Modified time: 2024-07-13 19:33:57
 */

#ifndef WiFiParameter_h
#define WiFiParameter_h

#include <Arduino.h>
#include <WiFiManager.h>

class WiFiParameter
{
public:
    bool is_string;
    bool is_int;
    String str_value;
    int int_value;
    const char *default_str;
    int default_int;
    WiFiManagerParameter *param;

    WiFiParameter(const WiFiParameter &param);
    WiFiParameter(const char *text);
    WiFiParameter(const char *id, const char *label, const char *defaultValue);
    WiFiParameter(const char *id, const char *label, int defaultValue);
    virtual ~WiFiParameter();

    bool isParam();
    const char *id();

    void set(const char *value);
    void set(int value);

    void update();
    const char *getStr();
    int getInt();
};

#endif