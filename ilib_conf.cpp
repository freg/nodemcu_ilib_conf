#include "ilib_conf.h"


String cfgsaved ;
short nbConfs = 0;
static uint8_t oc = 0;
#define MAXCONF 10
IpCapConf Confs [MAXCONF] ;
static String oldconf[MAXCONF];
ESP8266WebServer server(80);
String debut_html = "<!DOCTYPE HTML>\r\n<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\"><meta http-equiv=\"refresh\" content=\"10\";></head><body>";
String debut_html_simple = "<!DOCTYPE HTML>\r\n<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\"></head><body>";
String menu_html = "<table border=\"1\"><tr><th><a href=\"/\">etat</a></th><th><a href=\"/infos\">Infos</a></th><th><a href=\"/setup\">Setup</a></th><th><a href=\"/setupssid\">SetSSID</a></th><th><a href=\"/permanentlock\">ForceLock</a></th><th><a href=\"/permanentunlock\">ForceUnlock</a></th></tr></table><hr/>\n" ;
String fin_html = "</body></html>";
String ssid     = "";
String password = "";

void traiteConfCapteur(String ok, int i)
{
  String modset = ok.substring(ok.indexOf("SET[") + 4, ok.indexOf("]"));
  //String maconf = getConf(cip, 1, true) ;
  //if (maconf == modset) return;
  String xstr = modset.substring(modset.indexOf("ssid:") + 3, modset.indexOf(",password:"));
  if (xstr.indexOf(":") == -1 && xstr.indexOf(",") == -1) Confs[i].ssid = xstr ;
  xstr = modset.substring(modset.indexOf(",password:") + 10, modset.indexOf(",identifiant:"));
  if (xstr.indexOf(":") == -1 && xstr.indexOf(",") == -1)Confs[i].password = xstr ;
  xstr = modset.substring(modset.indexOf(",identifiant:") + 13, modset.indexOf(",path:"));
  if (xstr.indexOf(":") == -1 && xstr.indexOf(",") == -1)Confs[i].identifiant = xstr ;
  xstr = modset.substring(modset.indexOf(",path:") + 6, modset.indexOf(",host:"));
  if (xstr.indexOf(":") == -1 && xstr.indexOf(",") == -1)Confs[i].path = xstr ;
  xstr = modset.substring(modset.indexOf(",host:") + 6, modset.indexOf(",value_debug:"));
  if (xstr.indexOf(":") == -1 && xstr.indexOf(",") == -1)Confs[i].host = xstr ;
  xstr = modset.substring(modset.indexOf(",value_debug:") + 13, modset.indexOf(",debug:"));
  if (xstr.indexOf(":") == -1 && xstr.indexOf(",") == -1)Confs[i].value_debug = xstr.toInt() ;
  xstr = modset.substring(modset.indexOf(",debug:") + 7, modset.indexOf(",sleep:"));
  if (xstr.indexOf(":") == -1 && xstr.indexOf(",") == -1)Confs[i].debug = xstr == String("true") ? true : xstr == String("1") ? true : false ;
  xstr = modset.substring(modset.indexOf(",sleep:") + 7, modset.indexOf(",deep:"));
  if (xstr.indexOf(":") == -1 && xstr.indexOf(",") == -1)Confs[i].value_debug = xstr.toInt() ;
  xstr = modset.substring(modset.indexOf(",debug:") + 7, modset.indexOf(",local:"));
  if (xstr.indexOf(":") == -1 && xstr.indexOf(",") == -1)Confs[i].debug = xstr == String("true") ? true : xstr == String("1") ? true : false ;
  xstr = modset.substring(modset.indexOf(", local: ") + 7);
  if (xstr.indexOf(":") == -1 && xstr.indexOf(",") == -1)Confs[i].mode_local = xstr.toInt();
  //save_conf();
  //delay(100);
}

String getConfCapteur(int ncap)
{
  //  return "";
  String page = "" ;
  page += "ssid:";
  page += Confs[ncap].ssid;
  page += ",password:";
  page += Confs[ncap].password ;
  page += ",identifiant:";
  page += Confs[ncap].identifiant ;
  page += ",path:";
  page += Confs[ncap].path ;
  page += ",host:";
  page += Confs[ncap].host;
  page += ",value_debug:";
  page += String(Confs[ncap].value_debug);
  page += ",debug:";
  page += Confs[ncap].debug ? "true" : "false";
  page += ",local:";
  page += Confs[ncap].mode_local;
  Serial.println(String("getConfCapteur:") + String(ncap) + String(":") + page);
  return page ;
}
String page_ssidbox_setupssid(String set_ip)
{
  //String page = String("<form action=\"http://") + sip + String("/dosetupssid\" method=\"get\" >") ;
  String page = String("<form action=\"/dosetupssid\" method=\"get\" >") ;
  page += "<input type=\"submit\" name=\"write\"/>";
  page += "ssid:<br>";
  page += String("<input type=\"text\" name=\"ssid\" value=\"") + ssid + String("\"><br/>");
  page += "password:<br>";
  page += String("<input type=\"text\" name=\"password\"") + password + String("\"><br/>");
}

String page_capteur_setup(String set_ip)
{
  String page = String("<form action=\"/dosetup\" method=\"get\" >") ;
  page += "<input type=\"submit\" name=\"write\"/>";
  page += "identifiant:<br>";
  page += String("<input type=\"text\" name=\"identifiant\" value=\"") + identifiant + String("\"><br/>");
  page += "path:<br>";
  page += String("<input type=\"text\" name=\"path\" value=\"") + path + String("\"><br/>");
  page += "host:<br>";
  page += String("<input type=\"text\" name=\"host\" value=\"") + host + String("\"><br/>");
  page += "debug:<br>";
  page += String("<input type=\"text\" name=\"debug\" value=\"") + value_debug + String("\"><br/>");
  page += "local:<br>";
  page += String("<input type=\"text\" name=\"local\" value=\"") + mode_local + String("\"><br/>");
  page += "</form>" ;
  return page ;
}

void handleCapConf()
{
  String args = "" ;
  IPAddress cip = server.client().remoteIP() ;
  datacpt += 1;
  String xconf ;
  for (int i = 0; i < server.args(); i++)
    if (server.argName(i) == String("conf"))
    {
      xconf = server.arg(i) ;
      Serial.println(String("handleCapConf:") + xconf);
      if (oc == 9)
      {
        for (int ii = 0; ii < 9; ii++)
          oldconf[ii] = oldconf[ii + 1];
      }
      else
        oc++;
      oldconf[oc] = xconf;
    }
  int i = findConf(cip);
  Serial.println(String("Find return:") + String(i) + String(" for cip:") + cip);
  if (i == -1)
  {
    i = new_conf(cip, xconf) ;
  }
  else
    Serial.println(xconf + String(" Old conf:") + Confs[i].conf);
  String ret = "SET[";
  ret += getSet(cip);
  ret += "]de_cip:";
  ret += String(cip) + "_ip:";
  ret += get_sip(cip);
  ret += "cfg:";
  ret += getConf(cip, 0, true); ;
  ret += "_FIN...";
  setOldConf(server.client().remoteIP(), xconf);
  Serial.println(String("------>set conf ") + Confs[i].conf + String(" nbcap ") + String(nbConfs) + String(" IP: ") + get_sip(server.client().remoteIP()));
  server.send(200, "text/html", String("ok conf [") + ret + String("]"));

  ret = "Conf ipaddress:" ;
  ret += String(Confs[i].ip) + ", ";
  ret += String(Confs[i].recept) + ", ";
  ret += Confs[i].ssid + ", ";
  ret += Confs[i].identifiant + ", ";
  ret += Confs[i].conf + " ";
  Serial.println(ret);

}


int new_conf(IPAddress cip, String conf)
{
  Serial.println(String("New conf:") + String(nbConfs) + String(":") + get_sip(cip) + String(" ") + conf);
  if (nbConfs < MAXCONF)
    ++nbConfs ;
  else
    return -1;
  Confs[nbConfs - 1].ip = cip ;
  Confs[nbConfs - 1].conf = conf ;
  traiteConfCapteur(conf, nbConfs - 1) ;
  Confs[nbConfs - 1].recept = 0 ; ;
  Confs[nbConfs - 1].old = "";
  Serial.println(conf + "\nnew conf done");
  return nbConfs - 1;
}

int findConf(IPAddress cip)
{
  for (int i = 0; i < nbConfs; i++)
    if (Confs[i].ip == cip)
      return i;
  return -1;
}

String getSet(IPAddress cip)
{
  int i = findConf(cip);
  if (i > -1)
  {
    String ret = getConfCapteur(i);
    Confs[i].conf = ret ;
    Serial.println(String("GetSet:") + ret + String(cip));
    //Confs[i].set_ = "";
    return ret;
  }
  return "ssid:relais,password:,identifiant:capteur,path:/humidity?,host:relais.loc,value_debug:702,debug:false,local:0";
}
String getConf(IPAddress cip, int recept, bool recur)
{
  int i = findConf(cip);
  if (i > -1)
  {
    Confs[i].recept += recept ;
    return Confs[i].conf;
  }
  if (recur)
  {
    Serial.println(String("Attention recursion, nbConfs:") + String(nbConfs) + String("/") + String(MAXCONF) + String(" pour ") + get_sip(cip));
    return String("ERROR") ;
  }
  //new_conf(cip,"VIDE");
  return getConf(cip, recept, true) ;
}

int setConf(IPAddress cip, String conf)
{
  int i = findConf(cip);
  Serial.println(String("SetConf:") + String(i) + String(" CIP:") + String(cip) + String(" Conf:") + conf);
  if (i > -1)
  {
    Serial.println(String("found ") + String(i));
    Confs[i].conf = conf;
    return i;
  } ;

  Serial.println("Not found...");
  new_conf(cip, conf);
  return -1;
}
int setOldConf(IPAddress cip, String conf)
{
  int i = findConf(cip);
  Serial.println(String("SetOldConf:") + String(i) + String(" CIP:") + String(cip) + String(" Conf:") + conf);
  if (i > -1)
  {
    Serial.println(String("found ") + String(i));
    Confs[i].old = conf;
    return i;
  } ;

  Serial.println("Not found...");
  new_conf(cip, conf);
  return -1;
}

void handleCapteurGetConf()
{
  server.send(200, "text/html", getConf(server.client().remoteIP(), 1, false));
  Serial.println(String("GetConf hostHeaders ") + server.hostHeader() + String(" IP: ") + get_sip(server.client().remoteIP()));
}

void save_conf()
{
  String str = "" ;
  str += String("ssid:") + ssid + String("#");
  str += "\n";
  str += String("password:") + password + String("#");
  str += "\n";
  str += String("identifiant:") + identifiant + String("#");
  str += "\n";
  //  str += String("url_humi:") + url_humi + String("#");
  str += "\n";
  str += String("seuil:") + String(seuil) + String("#");
  str += "\n";
  str += String("seuil_load:") + String(seuil_load) + String("#");
  str += "\n";
  str += String("mini:") + String(mini) + String("#");
  str += "\n";
  str += String("maxi:") + String(maxi) + String("#");
  str += "\n";
  str += String("locked:") + String(locked ? 1 : 0) + String("#");
  str += "\n";
  str += String("permanent_lock:") + String(permanent_lock ? 1 : 0) + String("#");
  str += "\n";
  str += String("plafond:") + String(plafond) + String("#");
  str += "\n";
  str += String("fenetre_plafond:") + String(fenetre_plafond) + String("#");
  if (str != cfgsaved)
  {
    Serial.println(str + cfgsaved + " Save conf");
    cfgsaved = str;
    delay(100);
    myFile = SPIFFS.open(String("/conf.txt"), "w");
    Serial.println(String(myFile) + " myfile");
    delay(10);
    if (myFile)
    {
      myFile.println(str);
      delay(100);
      myFile.close();
      Serial.println("Conf saved in conf.txt");
      delay(10);
      //read_conf();
    }
    else
      Serial.println("open /conf.txt failed");
  }
  Serial.println("Save conf end");
}

String read_conf()
{
  Serial.println("Read conf");
  String conf = "";
  myFile = SPIFFS.open(String("/conf.txt"), "r");
  while (myFile.available())
  {
    char c[2];
    c[0] = (char)myFile.read();
    c[1] = 0;
    conf += c;
  }
  myFile.close();
  Serial.println("Conf read in conf.txt");
  Serial.println(conf);
  return conf;
}
void handleDoSetupSSID () {
  Serial.println("dosetupssid");
  String args = "" ;
  for (int i = 0; i < server.args(); i++)
    args += server.argName(i) + String(":") + server.arg(i) + String(" ");
  String nssid = server.arg("ssid");
  String passwd = server.arg("password");
  String debut_html_sp = "<!DOCTYPE HTML>\r\n<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">";
  String out = debut_html_sp + menu_html + String("<ul><li>") + ssid + ":" + password + String("</li></ul>") ;
  out += "DO SETUP<br/>\n";
  out += args ;
  WiFi.mode(WIFI_AP_STA);

  if (nssid.length() > 0 && passwd.length() > 0 )
  {
    out += "Vous avez change la conf wifi.....";
    out += fin_html;
    ssid = nssid;
    password = passwd;
    server.send(200, "text/html", out);
    delay(100);
    if (ssid.length() > 0)
    {
      WiFi.begin(ssid.c_str(), password.c_str());
      int cpt = 100 ;
      while (WiFi.status() != WL_CONNECTED && cpt--) {
        delay(100);
        Serial.print("nok.");
      }
      if (cpt < 1)
      {
        Serial.println("\nnot connected... spot....");
        delay(100);
        return resetup();
      }
      Serial.println("Yes new conf wifi ok ");
      Serial.println(String("conf:[") + ssid + String(":") + password + String("]"));
      ip = WiFi.localIP();
      suite_wifi();
    }
    save_conf();
    set_event(SETUPOK);//RIEN, DIRECT, FILTRE, SETUPOK, RESET, INIT
    Serial.println("doSetup done");
  }
  out += etat();
  out += fin_html;
  server.send(200, "text/html", out);
  send_conf();
}
void handleDoSetup () {
  Serial.println("dosetup");
  String args = "" ;
  for (int i = 0; i < server.args(); i++)
    args += server.argName(i) + String(":") + server.arg(i) + String(" ");
  String identi = server.arg("identifiant");
  String curl_humi = server.arg("url_humi");
  String cseuil  = server.arg("seuil");
  String cseuil_load  = server.arg("seuil_load");
  String cmini  = server.arg("mini");
  String cmaxi  = server.arg("maxi");
  String clocked  = server.arg("locked");
  String cplocked  = server.arg("permanent_lock");
  String chumidite_plafond  = server.arg("plafond");
  String cfenetre_plafond  = server.arg("fenetre_plafond");

  String debut_html_sp = "<!DOCTYPE HTML>\r\n<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">";
  String out = debut_html_sp + menu_html + String("<ul><li>") + ssid + ":" + password + "</li><li>" + identifiant + String("</li><li>") + host + String(":") + path + String("</li><li>millis:") + String(millis()) + String("</li></ul>") ;
  out += "DO SETUP<br/>\n";
  out += args ;

  //url_humi = curl_humi ;
  seuil = cseuil.toInt() ;
  seuil_load = cseuil_load.toInt() ;
  mini = cmini.toInt() ;
  maxi = cmaxi.toInt() ;
  locked = clocked.toInt() == 1 ? true : false ;
  permanent_lock = cplocked.toInt() == 1 ? true : false ;
  plafond = chumidite_plafond.toInt() ;
  fenetre_plafond = cfenetre_plafond.toInt() ;
  suite_wifi();
  save_conf();
  set_event(SETUPOK);//RIEN, DIRECT, FILTRE, SETUPOK, RESET, INIT
  Serial.println("doSetup done");
  out += etat();
  out += fin_html;
  server.send(200, "text/html", out);
  send_conf();
}
String page_setup()
{
  String page = String("<h1>SETUP</h1>\n<form action=\"http://") + sip + String("/dosetup\" method=\"get\" >") ;
  page += "<input type=\"submit\" name=\"write\"/>\n";
  page += "identifiant:<br>";
  page += "<input type=\"text\" name=\"identifiant\" value=\"";
  page += String(identifiant) + "\"><br/>\n";
  page += "url humidimetre:<br>";
  page += "<input type=\"text\" name=\"url_humi\" value=\"";
  //  page += String(url_humi) + "\"><br/>\n";
  page += "locked:<br>";
  page += "<input type=\"number\" name=\"locked\" value=\"";
  page += String(locked ? 1 : 0) + "\"><br/>\n";
  page += "permanent lock:<br>";
  page += "<input type=\"number\" name=\"permanent_lock\" value=\"";
  page += String(permanent_lock ? 1 : 0) + "\"><br/>\n";
  page += "seuil:<br>";
  page += "<input type=\"number\" name=\"seuil\" value=\"";
  page += String(seuil) + "\"><br/>\n";
  page += "seuil_load:<br>";
  page += "<input type=\"number\" name=\"seuil_load\" value=\"";
  page += String(seuil_load) + "\"><br/>\n";
  page += "mini:<br>";
  page += "<input type=\"number\" name=\"mini\" value=\"";
  page += String(mini) + "\"><br/>\n";
  page += "maxi:<br>";
  page += "<input type=\"number\" name=\"maxi\" value=\"";
  page += String(maxi) + "\"><br/>\n";
  page += "humidimetre plafond:<br>";
  page += "<input type=\"number\" name=\"plafond\" value=\"";
  page += String(plafond) + "\"><br/>\n";
  page += "fenetre plafond: en minutes<br>";
  page += "<input type=\"number\" name=\"fenetre_plafond\" value=\"";
  page += String(fenetre_plafond) + "\"><br/>\n";
  page += "</form>\n<hr/>\n" ;
  return page ;
}
void handleSetup () {
  Serial.println("setup");
  String debut_html_sp = "<!DOCTYPE HTML>\r\n<html><head><title>setup " ;
  debut_html_sp += identifiant + "</title><meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\"></head><body>";
  String out = debut_html_sp + menu_html + String("<ul><li>") + ssid + ":" + password + "</li><li>" + identifiant + "</li><li>" + host + ":" + path + String("</li><li>millis:") + String(millis()) +  String("</li>");
  //out += etat();
  out += page_setup();
  out += fin_html;
  server.send(200, "text/html", out);
}
void handleSetupSSID () {
  Serial.println("setup SSID");
  String debut_html_sp = "<!DOCTYPE HTML>\r\n<html><head><title>setup " ;
  debut_html_sp += identifiant + "</title><meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\"></head><body>";
  String out = debut_html_sp + menu_html + String("<ul><li>") + ssid + ":" + password + String("</li><li>millis:") + String(millis()) + String("</li>");
  //out += etat();
  out += page_setupssid();
  out += fin_html;
  server.send(200, "text/html", out);
}


