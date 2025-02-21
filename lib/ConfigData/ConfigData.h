#include <Arduino.h>

#if !defined(CONFIG_DATA_H)
#define CONFIG_DATA_H

#define ST(A) #A
#define STR(A) ST(A)

#ifdef WIFI_SSID
extern const char *env_wifi_ssid;
#endif

#ifdef WIFI_PASSWORD
extern const char *env_wifi_password;
#endif

#ifdef AUTH_USER
extern const char *env_auth_user;
#endif

#ifdef AUTH_PASSWORD
extern const char *env_auth_password;
#endif

#ifdef LOCAL_IP
extern const char *env_local_ip;
#endif

#ifdef GATEWAY
extern const char *env_gateway;
#endif

#ifdef SUBNET
extern const char *env_subnet;
#endif

#ifdef DNS1
extern const char *env_dns1;
#endif

#ifdef DNS2
extern const char *env_dns2;
#endif

#ifdef DNS2
extern const char *env_dns2;
#endif

#ifdef MDNS_NAME
extern const char *env_mdns;
#endif

struct ArgbConfig {
  uint8_t mode;
  uint8_t brightness;
  uint16_t speed;
  uint8_t source;
  uint32_t color;
};

struct ModbusConfig {
  uint8_t unit_id;
  uint16_t baud_rate_index;
};

struct ConfigData {
  char ssid[32];
  char password[64];
  char local_ip[32];
  char gateway[32];
  char subnet[32];
  char dns1[32];
  char dns2[32];
  char mdns[32];

  char auth_user[64];
  char auth_password[64];
  uint8_t fanSource[5];
  uint8_t fanDuty[5];
  ArgbConfig argb;
  ModbusConfig modbus;
};

#endif // CONFIG_DATA_H
