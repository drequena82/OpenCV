ESP8266WebServer server(80);

float t = 0;
float h = 0;
int m = 0;
int s = 0;

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

void InitServer()
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
