/**
 * @Author: Alexander Silva Barbosa
 * @Date:   2024-07-13 16:35:43
 * @Last Modified by:   Alexander Silva Barbosa
 * @Last Modified time: 2024-07-14 00:18:48
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
    int max_len;
    WiFiManagerParameter *param;

    WiFiParameter(const char *text);
    WiFiParameter(const char *id, const char *label, const char *defaultValue, const int maxLength = -1);
    WiFiParameter(const char *id, const char *label, int defaultValue, const int maxLength = -1);
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