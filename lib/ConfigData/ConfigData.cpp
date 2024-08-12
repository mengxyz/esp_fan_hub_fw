#include <ConfigData.h>

#define ST(A) #A
#define STR(A) ST(A)

#ifdef WIFI_SSID
const char *env_wifi_ssid = STR(WIFI_SSID);
#endif

#ifdef WIFI_PASSWORD
const char *env_wifi_password = STR(WIFI_PASSWORD);
#endif

#ifdef AUTH_USER
const char *env_auth_user = STR(AUTH_USER);
#endif

#ifdef AUTH_PASSWORD
const char *env_auth_password = STR(AUTH_PASSWORD);
#endif

#ifdef LOCAL_IP
const char *env_local_ip = STR(LOCAL_IP);
#endif

#ifdef GATEWAY
const char *env_gateway = STR(GATEWAY);
#endif

#ifdef SUBNET
const char *env_subnet = STR(SUBNET);
#endif

#ifdef DNS1
const char *env_dns1 = STR(DNS1);
#endif

#ifdef DNS2
const char *env_dns2 = STR(DNS2);
#endif
