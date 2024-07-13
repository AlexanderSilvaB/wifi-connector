/**
 * @Author: Alexander Silva Barbosa
 * @Date:   2024-07-13 16:36:47
 * @Last Modified by:   Alexander Silva Barbosa
 * @Last Modified time: 2024-07-14 00:49:57
 */

#include "WiFiParameter.h"

WiFiParameter::WiFiParameter(const char *text)
{
    this->default_int = 0;
    this->default_str = nullptr;
    this->is_int = false;
    this->is_string = false;
    this->str_value = String();
    this->int_value = this->default_int;
    this->max_len = 0;
    this->param = new WiFiManagerParameter(text);
}

WiFiParameter::WiFiParameter(const char *id, const char *label, const char *defaultValue, const int maxLength)
{
    this->max_len = maxLength;
    if (this->max_len <= 0)
        this->max_len = 128;
    this->default_int = 0;
    this->default_str = defaultValue;
    this->is_int = false;
    this->is_string = true;
    this->str_value = String(this->default_str);
    this->int_value = this->default_int;
    this->param = new WiFiManagerParameter(id, label, this->str_value.c_str(), this->max_len);
}

WiFiParameter::WiFiParameter(const char *id, const char *label, int defaultValue, const int maxLength)
{
    this->max_len = maxLength;
    if (this->max_len <= 0)
        this->max_len = 11;
    this->default_int = defaultValue;
    this->default_str = nullptr;
    this->is_int = true;
    this->is_string = false;
    this->str_value = String(defaultValue);
    this->int_value = defaultValue;
    this->param = new WiFiManagerParameter(id, label, this->str_value.c_str(), this->max_len);
}

WiFiParameter::~WiFiParameter()
{
    delete this->param;
}

bool WiFiParameter::isParam()
{
    return this->is_int || this->is_string;
}

const char *WiFiParameter::id()
{
    return this->param->getID();
}

void WiFiParameter::update()
{
    this->str_value = String(this->param->getValue());
    if (this->is_int)
        this->int_value = this->str_value.toInt();
}

const char *WiFiParameter::getStr()
{
    this->update();
    return this->str_value.c_str();
}

int WiFiParameter::getInt()
{
    this->update();
    return this->int_value;
}

void WiFiParameter::set(const char *value)
{
    this->is_int = false;
    this->is_string = true;
    this->str_value = String(value);
    this->int_value = 0;
    this->param->setValue(this->str_value.c_str(), this->max_len);
}

void WiFiParameter::set(int value)
{
    this->is_int = true;
    this->is_string = false;
    this->str_value = String(value);
    this->int_value = value;
    this->param->setValue(this->str_value.c_str(), this->max_len);
}