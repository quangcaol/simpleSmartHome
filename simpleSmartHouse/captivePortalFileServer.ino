#include <Servo.h>
#include <DHT.h>
#include <DHT_U.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include <math.h>

#define DBG_OUTPUT_PORT Serial
#define DHTPIN D5
#define DHTTYPE DHT11

#define AVG_TEMP 30

char A[5];
const byte DNS_PORT = 53;
const char *ssid = "SmartHouse";
const char *pass = "12345678";
const char *metaRefreshStr = "<head><meta http-equiv=\"refresh\" content=\"0; url=http://192.168.1.1/index.html\" /></head><body></body>";
const char *metaRefreshStr1 = "<head><meta http-equiv=\"refresh\" content=\"0; url=http://192.168.1.1/main.html\" /></head><body></body>";
bool lightStat = 0;
bool fanStat = 0;
bool roofStat = 0;
bool alertStat = 0;
bool manual = 1;
bool dayNight = 1;
float pretem,tem = 0.0;
float hud = 0.0;
int speedo = 500;
float lux = 0;
long int previous, now = 0;

IPAddress apIP(192, 168, 1, 1);

DNSServer dnsServer;

ESP8266WebServer webServer(80);

DHT dht(DHTPIN, DHTTYPE);

Servo servo;

void handleSpeed() {
  if (fanStat) {
  DBG_OUTPUT_PORT.println("HandleSpeed");
  speedo = atoi(webServer.arg("vel").c_str());
  DBG_OUTPUT_PORT.println(speedo);
  analogWrite(D6,speedo);
  }
  else {analogWrite(D6,0);}
}
float luxCal(){
  float raw = analogRead(A0);
  float rV = raw*(3.3/1024.0);
  float idr = 3.30-rV;
  float res = idr/rV*1000;
  res = 12518931*pow(res,-1.045);
  return res;
  
}

void handleLight() {
  DBG_OUTPUT_PORT.println("HandleLight");
  lightStat = !lightStat;
  digitalWrite(D7,lightStat);
  webServer.send(200,"text/plain",String(lightStat));
}
void handleFan() {
  DBG_OUTPUT_PORT.println("HandleFan");
  fanStat = !fanStat;
  if (fanStat) {
    analogWrite(D6,speedo);
  }
  else {
    analogWrite(D6,0);
  }
  webServer.send(200,"text/plain",String(fanStat));
}
void handleAlert() {
  DBG_OUTPUT_PORT.println("HandleAlert");
  alertStat = !alertStat;
  if (alertStat) tone(D4,700);
  else noTone(D4);
  webServer.send(200,"text/plain",String(alertStat));
}
int tempControl() {
  float error = tem - pretem;
  int prop = int(error*20.0);
  if ((prop > 100) || (prop <-100)) {
      prop = (prop*50)/(-prop);  
  }
  if (prop<-30) {fanStat = 0; return 0;}
  else fanStat = 1;
  return map(prop,700,1024,-30,50);
}
void handleRoof() {
  DBG_OUTPUT_PORT.println("HandleRoof");
  roofStat = !roofStat;
  if (roofStat) {
    servo.write(90); }
  else {
    servo.write(180);
  }
  webServer.send(200,"text/plain",String(roofStat));
}
void handlePara(){
  DBG_OUTPUT_PORT.println("HandleTemp");
  webServer.send(200,"text/plain",String(tem)+","+String(hud)+","+String(lux));
}
void root(){
  DBG_OUTPUT_PORT.println("HandleRoot");
  webServer.send(200,"text/plain",String(tem)+","+String(hud)+","+String(lightStat)+"," + String(fanStat)+","+String(alertStat)+","+String(roofStat)+","+String(manual)+","+String(lux));
}
void handleMode() {
  DBG_OUTPUT_PORT.println("HandleMode");
  manual = bool(webServer.arg("cbox"));
  DBG_OUTPUT_PORT.println(manual);
}


void setup() {
  pinMode(D7,OUTPUT);
  pinMode(D4,OUTPUT);
  pinMode(D6,OUTPUT);
  pinMode(D8,OUTPUT);
  pinMode(A0,INPUT);
  servo.attach(D3);
  dht.begin();
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssid,pass);

  // if DNSServer is started with "*" for domain name, it will reply with
  // provided IP to all DNS request
  dnsServer.start(DNS_PORT, "*", apIP);

  //start debug port
  DBG_OUTPUT_PORT.begin(115200);
  DBG_OUTPUT_PORT.print("\n");
  DBG_OUTPUT_PORT.setDebugOutput(true);
  SPIFFS.begin();

  //redirect all traffic to index.html
  webServer.onNotFound([]() {
    if(!handleFileRead(webServer.uri())){
      webServer.send(200, "text/html", metaRefreshStr);
    }
  });
  webServer.on("/root", root); 
  webServer.on("/light", handleLight); 
  webServer.on("/parameter",handlePara);
  webServer.on("/alert",handleAlert);
  webServer.on("/fan",handleFan);
  webServer.on("/roof",handleRoof);
  webServer.on("/mode",handleMode);
  webServer.on("/speed",handleSpeed);
  webServer.begin();
  servo.write(180);
}

void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();
  now = millis();
  
  if ((now - previous) > 5000) {
    pretem = tem;
    tem = dht.readTemperature();
    hud = dht.readHumidity();
    previous = now;
    lux = luxCal();
  }
  if (!manual) {
    if ((lux<500)&&(lightStat == 0)) {lightStat = 1; digitalWrite(D7,lightStat);}
    else {lightStat = 0; digitalWrite(D7,lightStat);}
    if ((hud >85)&&(lux>500)) {roofStat = 1; servo.write(90); }
    else { roofStat = 0; servo.write(180); }
    analogWrite(D6,tempControl());
  }
}



String getContentType(String filename){
  if(webServer.hasArg("download")) return "application/octet-stream";
  else if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  else if(filename.endsWith(".eot")) return "text/css";
  else if(filename.endsWith(".otf")) return "text/css";
  else if(filename.endsWith(".svg")) return "text/css";
  else if(filename.endsWith(".ttf")) return "text/css";
  return "text/plain";
}

//Given a file path, look for it in the SPIFFS file storage. Returns true if found, returns false if not found.
bool handleFileRead(String path){
  DBG_OUTPUT_PORT.println("handleFileRead: " + path);
  if(path.endsWith("/")) path += "index.html";
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)){
    if(SPIFFS.exists(pathWithGz))
      path += ".gz";
    File file = SPIFFS.open(path, "r");
    size_t sent = webServer.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}
