#include <DHT.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>


DHT dht(0, DHT22 ); // DHT szenzor PIN és tipus

//WiFi
const char* ssid ="ZTE_H168N009B78"; 
const char* password ="bagameri"; 
//MQTT broker
char hostname[] ="152.66.244.113"; 
#define TOKEN "tomi"

WiFiClient wifiClient;
PubSubClient client(wifiClient);

int status = WL_IDLE_STATUS;


//Változók
float hum;  
float temp;
float light;
String s_temp; 
String s_hum; 
String s_light; 
#define MAX_SIZE (100)
char c_temp[MAX_SIZE];
char c_hum[MAX_SIZE];
char c_light[MAX_SIZE];



void setup()
{
  Serial.begin(115200);
  WiFi.begin(ssid, password);// Csatlakozás WiFi-hez

  while (WiFi.status() != WL_CONNECTED) 
  {
  delay(500);
  Serial.print(".");
  }
  Serial.println("Connected to WiFi");
  Serial.println("ESP8266 AS PUBLISHER");
  client.setServer(hostname, 1883 );
  dht.begin();
}

void loop()
{
  if ( !client.connected() ) 
  {
  reconnect();
  }
  MQTTPOST();  
  delay(5000);  // frissítési gyakoriság
}

void MQTTPOST()
{
  light = (1024 - analogRead(A0))/10.24; // A fényerősség ellőállítása százalékra
  hum = dht.readHumidity(); // Páratartalom kiolvasása
  temp= dht.readTemperature(); // Hőmérséklet kiolvasása
  
  //Adatok kiírása soros monitorra
  /*
  Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.print(" %, Temp: ");
  Serial.print(temp);
  Serial.println(" Celsius");
  Serial.println("Analog reading = ");
  Serial.print(light);
  */ 

  // Floatok konvertálása stringé
  s_temp="";
  s_temp += temp;
  s_hum="";
  s_hum += hum; 
  s_light="";
  s_light += light;

  //MQTT publishok
  s_temp.toCharArray(c_temp, MAX_SIZE); 
  client.publish("InsideTemp", c_temp); 
  s_hum.toCharArray(c_hum, MAX_SIZE);
  client.publish("InsideHum", c_hum); 
  s_light.toCharArray(c_light, MAX_SIZE);
  client.publish("Light", c_light); 
}

//WiFi újra csatlakozás
void reconnect() 
{
  while (!client.connected()) 
  {
    status = WiFi.status();
    if ( status != WL_CONNECTED) 
    {
      WiFi.begin(ssid, password);
      while (WiFi.status() != WL_CONNECTED) 
      {
        delay(500);
        Serial.print(".");
      }
      Serial.println("Connected to AP");
    }
    Serial.print("Connecting to Broker …");
    Serial.print("152.66.244.113");

    if ( client.connect("ESP8266 Device", TOKEN, NULL) )
    {
      Serial.println("[DONE]" );
    }
    else {
      Serial.println( " : retrying in 5 seconds]" );
      delay( 5000 );
    }
  }
}
