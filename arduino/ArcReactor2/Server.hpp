#include "config.h"  // Sustituir con datos de vuestra red

AsyncWebServer server(80);
short bootType = 0;

//Devolvemos el estado de la lampara.
short getBootType(){
  return bootType; 
}

//Seteamos el estado de la lampara.
void setBootType(short b){
  bootType = b; 
}
  
// Funcion al recibir petición GET
void getArcLamp(AsyncWebServerRequest *request){
   //devolver respuesta
   String json = "{ \"status\":\"" + String(bootType) + "\"}";
   Serial.println(json);
   request->send(200, "application/json",json);
}
   
// Funcion al recibir petición POST
void setArcLamp(AsyncWebServerRequest *request){
   // mostrar por puerto serie
   //Serial.println(server.argName(0));
     
   String type = request->arg("flexRadio");
   Serial.println(type);
   
   //Activamos la lampara con los efectos de fundido, o con los parpadeos
   setBootType(type.toInt());
     
   // devolver respuesta
   String json = "{ \"status\":\"" + String(bootType) + "\"}";
   request->send(200, "application/json",json);
  }
  
  // Funcion que se ejecutara en la URI '/'
  /*
  void handleRoot(AsyncWebServerRequest *request){
     request->send(200, "text/plain", "Hola mundo!");
  }
  */
  
  void initServer(){
   // Ruteo para '/'
   //server.on("/", handleRoot);
  
   // Definimos dos routeos
   server.on("/status", HTTP_GET, getArcLamp);
   server.on("/save", HTTP_POST, setArcLamp);
     
     
   server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

   // Ruteo para URI desconocida
   server.onNotFound([](AsyncWebServerRequest *request) {
      request->send(400, "text/plain", "Esta página no existe");
   });

   server.begin();

   SPIFFS.begin();
  
   if (!SPIFFS.exists("/index.html")) {
    Serial.println("Please wait 30 secs for SPIFFS to be formatted");
   }
   Serial.println("HTTP server started");
  }

  void connectWiFiSTA(bool useStaticIP = false){
     Serial.println("Connecting...");
     WiFi.mode(WIFI_STA);
     WiFi.begin(ssid, password);
     if(useStaticIP) WiFi.config(ip, gateway, subnet);
     while (WiFi.status() != WL_CONNECTED){ 
       delay(100);  
       Serial.print('.'); 
     }
   
     Serial.println("");
     Serial.print("Iniciado STA:\t");
     Serial.println(ssid);
     Serial.print("IP address:\t");
     Serial.println(WiFi.localIP());
  }
  
  void connectWiFiAP(bool useStaticIP = false){ 
     Serial.println("");
     WiFi.mode(WIFI_AP);
     while(!WiFi.softAP(ssid, password)){
       Serial.println(".");
       delay(100);
     }
     if(useStaticIP){ 
      WiFi.softAPConfig(ip, gateway, subnet);
     }
  
     Serial.println("");
     Serial.print("Iniciado AP:\t");
     Serial.println(ssid);
     Serial.print("IP address:\t");
     Serial.println(WiFi.softAPIP());
  }
  
