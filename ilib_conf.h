#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#include <ESP8266WebServer.h>
#include <TimeLib.h>

typedef struct
{
  IPAddress ip ;
  String conf, old ;
  uint32_t recept ;
  String ssid, password, identifiant, host, path;
  int value_debug, mode_local ;
  bool debug;
} IpCapConf ;
extern String ssid  ;
extern String password ;
extern String debut_html ;
extern String debut_html_simple ;
extern String menu_html ;
extern String fin_html ;
extern short nbConfs ;
extern String cfgsaved ;
extern uint8_t oc ;
extern String oldconf[];
extern IpCapConf Confs [];
extern ESP8266WebServer server;
void traiteConfCapteur(String ok, int i);
String getConfCapteur(int ncap);
String page_ssidbox_setupssid(String set_ip) ;
String page_capteur_setup(String set_ip);
String page_setupssid();

void handleCapConf();
int new_conf(IPAddress cip, String conf);
int findConf(IPAddress cip);
String getSet(IPAddress cip);
String getConf(IPAddress cip, int recept, bool recur);
int setConf(IPAddress cip, String conf);
int setOldConf(IPAddress cip, String conf);
void handleCapteurGetConf();

String read_conf();
void save_conf();

void handleDoSetupSSID () ;
void handleDoSetup () ;
void handleSetup () ;
void handleSetupSSID () ;

