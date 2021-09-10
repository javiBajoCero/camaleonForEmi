#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <ESP8266mDNS.h>

//https://tttapa.github.io/ESP8266/Chap10%20-%20Simple%20Web%20Server.html
//https://randomnerdtutorials.com/display-images-esp32-esp8266-web-server/
#include "imagesStringified.h"
/*
   This example serves a "hello world" on a WLAN and a SoftAP at the same time.
   The SoftAP allow you to configure WLAN parameters at run time. They are not setup in the sketch but saved on EEPROM.
   Connect your computer or cell phone to wifi network ESP_ap with password 12345678. A popup may appear and it allow you to go to WLAN config. If it does not then navigate to http://192.168.4.1/wifi and config it there.
   Then wait for the module to connect to your wifi and take note of the WLAN IP it got. Then you can disconnect from ESP_ap and return to your regular WLAN.
   Now the ESP8266 is in your network. You can reach it through http://192.168.x.x/ (the IP you took note of) or maybe at http://esp8266.local too.
   This is a captive portal because through the softAP it will redirect any http request to http://192.168.4.1/
*/

/* Set these to your desired softAP credentials. They are not configurable at runtime */
#ifndef APSSID
#define APSSID "CAMALEON"
#define APPSK  ""
#endif

const char *softAP_ssid = APSSID;
const char *softAP_password = APPSK;

/* hostname for mDNS. Should work at least on windows. Try http://esp8266.local */
const char *myHostname = "camaleon";
String stringImagecamaleon;/* Don't set this wifi credentials. They are configurated at runtime and stored on EEPROM */
char ssid[32] = "";
char password[32] = "";

// DNS server
const byte DNS_PORT = 53;
DNSServer dnsServer;

// Web server
ESP8266WebServer server(80);

/* Soft AP network parameters */
//IPAddress apIP(192, 168, 4, 1);
IPAddress apIP(8, 8, 8, 8);
IPAddress netMsk(255, 255, 255, 0);


/** Should I connect to WLAN asap? */
boolean connect;

/** Last time I tried to connect to WLAN */
unsigned long lastConnectTry = 0;

/** Current WLAN status */
unsigned int status = WL_IDLE_STATUS;

/** Is this an IP? */
boolean isIp(String str) {
  for (size_t i = 0; i < str.length(); i++) {
    int c = str.charAt(i);
    if (c != '.' && (c < '0' || c > '9')) {
      return false;
    }
  }
  return true;
}

/** IP to String? */
String toStringIp(IPAddress ip) {
  String res = "";
  for (int i = 0; i < 3; i++) {
    res += String((ip >> (8 * i)) & 0xFF) + ".";
  }
  res += String(((ip >> 8 * 3)) & 0xFF);
  return res;
}

/** Handle root or redirect to captive portal */
void handleRoot() {
  if (captivePortal()) { // If caprive portal redirect instead of displaying the page.
    return;
  }
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");

  String Page;
  Page += F(
            "<html><head>"
            "<style>"
".button {"
"  border: none;"
"  color: white;"
"  padding: 16px 32px;"
"  text-align: center;"
"  text-decoration: none;"
"  display: inline-block;"
"  font-size: 35px;"
"  margin: 4px 2px;"
"  transition-duration: 0.4s;"
"  cursor: pointer;"
"}"

".button1 {"
"  background-color: white; "
"  color: black; "
"  border: 2px solid #4CAF50;"
"  border-radius: 12px;"
"}"
".button2 {"
"  background-color: white; "
"  color: black; "
"  border: 2px solid #008CBA;"
"  border-radius: 12px;"
"}"

".button3 {"
"  background-color: white; "
"  color: #ba2b00; "
"  border: 2px solid #ba2b00;"
"  border-radius: 12px;"
"}"

".button1:hover {"
"  background-color: #4CAF50;"
"  color: white;"
"}"

".button2:hover {"
"  background-color: #008CBA;"
"  color: white;"
"}"

".button3:hover {"
"  background-color: #ba2b00;"
"  color: white;"
"}"

"img {"
  "width: 50%;"
  "display: block;"
  "margin-left: auto;"
  "margin-right: auto;"
  "  border-radius: 12px;"
"}"

".center {"
"  display: flex;"
"  justify-content: center;"
"  align-items: center;"
"  height: 200px;"
//"  border: 3px solid green;" 
"}"
"</style>"
"</head>"
"<body style=\"background-color:#e0e3b8;\">"
            );

   Page += F("<img "
   "src= "
   "data:image/jpeg;base64,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>"
   
   );

Page += F(   
  "<div class=\"center\">"
"  <form action=\"/IZQ\" method=\"post\">"
    "<button class=\"button button1\">OJO</button>"
"  </form>"

"  <form action=\"/DER\" method=\"post\">"
    "<button class=\"button button1\">OJO</button>"
"  </form>"
  "</div>"
  );
  Page += F(   
  "<div class=\"center\">"
  "  <form action=\"/LAZY\" method=\"post\">"
    "<button class=\"button button2\">lazy</button>"
"  </form>"
"  <form action=\"/NERVOUS\" method=\"post\">"
    "<button class=\"button button2\">nervous</button>"
"  </form>"
"  <form action=\"/LSD\" method=\"post\">"
    "<button class=\"button button2\">LSD</button>"
"  </form>"
  "</div>"
  );
    Page += F(   
  "<div class=\"center\">"
  "  <form action=\"/STOP\" method=\"post\">"
        "<button class=\"button button3\">STOP</button>"
  "  </form>"
  "</div>"
  );
  
   Page +=("</body></html>");
  server.send(200, "text/html", Page);
}

/** Redirect to captive portal if we got a request for another domain. Return true in that case so the page handler do not try to handle the request again. */
boolean captivePortal() {
  if (!isIp(server.hostHeader()) && server.hostHeader() != (String(myHostname) + ".local")) {
    Serial.println("Request redirected to captive portal");
    server.sendHeader("Location", String("http://") + toStringIp(server.client().localIP()), true);
    server.send(302, "text/plain", "");   // Empty content inhibits Content-length header so we have to close the socket ourselves.
    server.client().stop(); // Stop is needed because we sent no content length
    return true;
  }
  return false;
}

void handleNotFound() {
  if (captivePortal()) { // If caprive portal redirect instead of displaying the error page.
    Serial.println("captive!");
    return;
  }
  String message = F("File Not Found\n\n");
  message += F("URI: ");
  message += server.uri();
  message += F("\nMethod: ");
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += F("\nArguments: ");
  message += server.args();
  message += F("\n");

  for (uint8_t i = 0; i < server.args(); i++) {
    message += String(F(" ")) + server.argName(i) + F(": ") + server.arg(i) + F("\n");
  }
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send(404, "text/plain", message);
}

void handleIZQ() {
  Serial.println("handleIZQ");
  handleRoot();
}

void handleDER() {
    Serial.println("handleDER");
    handleRoot();
}

void handleLAZY() {
    Serial.println("handleLAZY");
    handleRoot();
}

void handleNERVOUS() {
    Serial.println("handleNERVOUS");
    handleRoot();
}

void handleLSD() {
    Serial.println("handleLSD");
    handleRoot();
}

void handleSTOP() {
    Serial.println("handleSTOP");
    handleRoot();
}

void setup() {
  delay(1000);
  Serial.begin(9600);
  Serial.println();
  Serial.println("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAPConfig(apIP, apIP, netMsk);
  WiFi.softAP(softAP_ssid, softAP_password);
  delay(500); // Without delay I've seen the IP address blank
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  /* Setup the DNS server redirecting all the domains to the apIP */
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(DNS_PORT, "*", apIP);

  /* Setup web pages: root, wifi config pages, SO captive portal detectors and not found. */
  server.on("/",              handleRoot);
  server.on("/IZQ",           handleIZQ);
  server.on("/DER",           handleDER);
  server.on("/LAZY",          handleLAZY);  
  server.on("/NERVOUS",       handleNERVOUS);  
  server.on("/LSD",           handleLSD);  
  server.on("/STOP",          handleSTOP);  
  server.onNotFound(      handleNotFound);
  server.begin(); // Web server start
  Serial.println("HTTP server started");
}


void loop() {

        // Setup MDNS responder
        if (!MDNS.begin(myHostname)) {
          Serial.println("Error setting up MDNS responder!");
        } else {
          Serial.println("mDNS responder started");
          // Add service to MDNS-SD
          MDNS.addService("http", "tcp", 80);
        }
  
  // Do work:
  //DNS
  dnsServer.processNextRequest();
  //HTTP
  server.handleClient();
}
