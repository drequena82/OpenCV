#include "config.h"  // Sustituir con datos de vuestra red

AsyncWebServer server(80);
float t;
float h;
int m;
int s;

int aw,uh,tb;
bool regando,isSaving;

void sensorValues(float temp,float hum,int moist,int state,int activeSwitch,int umbralH,int timerBomba){
  t = temp;
  h = hum;
  m = moist;
  s = state;
  aw = activeSwitch;
  uh = umbralH;
  tb = timerBomba;
}

// Funcion que se ejecutara en la URI '/'
void handleRefresh(AsyncWebServerRequest *request)
{
  String json =  "{ \"temperature\":\"" + String(t) + "\", \"humidity\": \"" + String(h) + "\", \"moisture\": \"" + String(m) + "\",\"switch\": \"" + String(aw) + "\", \"umbral\": \"" + String(uh) + "\", \"tiempo\": \"" + String(tb) + "\"}";
  Serial.println(json);
  request->send(200, "application/json",json);
}

void handleWatering(AsyncWebServerRequest *request)
{
  Serial.println("Se ha activado el riego");
  regando = true;
  request->send(200, "text/text","regando");
}

void handleSave(AsyncWebServerRequest *request)
{
  String umbral = request->arg("rangoUmbral");
  String tiempo = request->arg("rangoTiempo");
  String activeSwitch = request->arg("switch");

  Serial.print("Params: ");
  Serial.print(umbral);
  Serial.print(" ");
  Serial.print(tiempo);
  Serial.print(" ");
  Serial.print(activeSwitch);

  if(activeSwitch.equals("on")){
    aw = 1;
  }else{
    aw = 0;
  }

    uh= umbral.toInt();
    tb= tiempo.toInt() * 1000;

  isSaving = true;

  request->send(200, "text/text","Datos guardados");
}

bool getIsSaving(){
  return isSaving;
}

void setIsSaving(bool saving){
  isSaving = saving;
}

int getUmbral(){
  return uh;
}

int getTiempo(){
  return tb;
}

int getSwitch(){
  return aw;
}

bool getRegando(){
  return regando;
}

void setRegando(bool isRegando){
  regando = isRegando;
}

void initServer()
{
   server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
   server.on("/stateSensors", HTTP_GET, handleRefresh);
   server.on("/watering",HTTP_GET,handleWatering);
   server.on("/save",HTTP_POST,handleSave);
   
   server.onNotFound([](AsyncWebServerRequest *request) {
    request->send(400, "text/plain", "Esta página no existe");
   });

   server.begin();
   Serial.println("HTTP server started");

   SPIFFS.begin();
  
   if (!SPIFFS.exists("/index.html")) {
    Serial.println("Please wait 30 secs for SPIFFS to be formatted");
   }
    
}

  void connectWiFi_STA(bool useStaticIP = false)
  {
     Serial.println(WiFi.macAddress());
     Serial.println("Connecting...");
     WiFi.mode(WIFI_STA);
     WiFi.begin(ssid, password);
     if(useStaticIP) WiFi.config(ip, gateway, subnet);
     while (WiFi.status() != WL_CONNECTED) 
     { 
       delay(100);  
       Serial.print('.'); 
     }
   
     Serial.println("");
     Serial.print("Iniciado STA:\t");
     Serial.println(ssid);
     Serial.print("IP address:\t");
     Serial.println(WiFi.localIP());
     Serial.print("MAC adress: \t");
     Serial.println(WiFi.macAddress());
  }
  
  void connectWiFi_AP(bool useStaticIP = false)
  { 
     Serial.println("");
     WiFi.mode(WIFI_AP);
     while(!WiFi.softAP(ssid, password))
     {
       Serial.println(".");
       delay(100);
     }
     if(useStaticIP) WiFi.softAPConfig(ip, gateway, subnet);
  
     Serial.println("");
     Serial.print("Iniciado AP:\t");
     Serial.println(ssid);
     Serial.print("IP address:\t");
     Serial.println(WiFi.softAPIP());
  }
  
