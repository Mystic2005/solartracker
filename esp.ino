// Acesta este codul pentru placuta ESP8266 NodeMCU E12
// primeste datele de la Arduino pe D5 prin SoftwareSerial si le afiseaza pe o pagina web
// ESP ul creeaza propriul wifi (ap) la care te conectezi si accesezi 192.168.4.1 in browser
// pagina se reincarca automat la 2s ca sa arate mereu datele noi

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SoftwareSerial.h>

SoftwareSerial unoSerial(D5, -1); // am ales D5 pentru ca nu mergea pe placa mea sa folosesc RX

ESP8266WebServer server(80);

// registru pentru fiecare data
String unghi = "--";
String lumina = "--";
String temp = "--";
String bestUnghi = "--";
String bestLumina = "--";
bool pornit = false;

void parseDate(String linie) { // parsez fiecare linie in functie de clasificare cu inceputul
  if (linie.startsWith("U=")) {
    int u = linie.indexOf("U=");
    int l = linie.indexOf("lumina=");
    int t = linie.indexOf("temp=");
    if (u != -1) unghi    = linie.substring(u + 2, linie.indexOf(' ', u));
    if (l != -1) lumina   = linie.substring(l + 7, linie.indexOf(' ', l));
    if (t != -1) temp     = linie.substring(t + 5);
  }

  if (linie.startsWith("BESTU=")) {
    int u = linie.indexOf("BESTU=");
    int l = linie.indexOf("lumina=");
    int t = linie.indexOf("temp=");
    if (u != -1) bestUnghi  = linie.substring(u + 6, linie.indexOf(' ', u));
    if (l != -1) bestLumina = linie.substring(l + 7, linie.indexOf(' ', l));
    if (t != -1) temp       = linie.substring(t + 5);
  }

  if (linie.startsWith("STATUS=")) {
    pornit = linie.substring(7) == "ON";
  }
}

void handleRoot() { // pagina html
  String stare = pornit ? "PORNIT" : "OPRIT";
  String culoare = pornit ? "green" : "red";

  String html = "<html><head><title>Solar Tracker</title></head>";
  html += "<body style='display:flex;justify-content:center;align-items:center;flex-direction:column;'>";
  html += "<h2>Solar Tracker</h2>";
  html += "<p>Unghi: " + unghi + " grade</p>";
  html += "<p>Lumina: " + lumina + "</p>";
  html += "<p>Temperatura: " + temp + " C</p>";
  html += "<p>Best unghi: " + bestUnghi + " grade, lumina: " + bestLumina + "</p>";
  html += "<br>";
  html += "<p>Sistem: <b style='color:" + culoare + "'>" + stare + "</b></p>";
  html += "<script>setTimeout(() => location.reload(), 2000);</script>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(9600);
  unoSerial.begin(9600);

  WiFi.softAP("SolarTracker", "testtest");
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot); // rute pentru fiecare comanda
  server.begin();
}

void loop() {
  server.handleClient();

  // citeste date de la Arduino constant
  if (unoSerial.available()) {
    String linie = unoSerial.readStringUntil('\n');
    linie.trim();
    if (linie.length() > 0) parseDate(linie);
  }
}