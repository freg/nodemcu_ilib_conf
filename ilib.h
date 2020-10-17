typedef struct
{
  IPAddress ip ;
  String conf, old ;
  uint32_t recept ;
  String ssid, password, identifiant, host, path;
  int value_debug, mode_local ;
  bool debug;
} IpCapConf ;

short nbConfs = 0;

void traiteConfCapteur(String ok, int i);
String getConfCapteur(int ncap);
String page_ssidbox_setupssid(String set_ip) ;
String page_capteur_setup(String set_ip);


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
