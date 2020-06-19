#include "config.h"  // Sustituir con datos de vuestra red

    ESP8266WebServer server(80);
    float t;
    float h;
    int m;
    int s;

  
  void sensorValues(float temp,float hum,int moist,int state){
     t = temp;
     h = hum;
     m = moist;
     s = state;
  }
  
  // Funcion al recibir petición GET
  void getLED() 
  {
     // devolver respuesta
     server.send(200, "text/plain", String("GET ") + server.arg(String("Id")));
  }
   
  // Funcion al recibir petición POST
  void setLED() 
  {
     // mostrar por puerto serie
     Serial.println(server.argName(0));
     
     // devolver respuesta
     server.send(200, "text/plain", String("POST ") + server.arg(String("Id")) + " " + server.arg(String("Status")));
  }
  
  // Funcion que se ejecutara en la URI '/'
  void handleRoot() 
  {
     server.send(200, "text/plain", "Hola mundo! Temp: " + String(t) + "ºC Humedad: " + String(h) + "% Humedad tierra: " + String(m) + "%");
  }
   
  // Funcion que se ejecutara en URI desconocida
  void handleNotFound() 
  {
     server.send(404, "text/plain", "Not found");
  }
  
  void initServer()
  {
     // Ruteo para '/'
     server.on("/", handleRoot);
   
     // Ruteo para '/inline' usando función lambda
     server.on("/inline", []() {
        server.send(200, "text/plain", "Esto tambien funciona");
     });
  
     // Definimos dos routeos
     server.on("/led", HTTP_GET, getLED);
     server.on("/led", HTTP_POST, setLED);
     
     // Ruteo para URI desconocida
     server.onNotFound(handleNotFound);
   
     // Iniciar servidor
     server.begin();
     Serial.println("HTTP server started");
  }

  void connectWiFi_STA(bool useStaticIP = false)
  {
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
  
