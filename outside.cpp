#include <DHT.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>


DHT dht(0, DHT22 ); // DHT szenzor PIN és tipus

//WiFi
const char* ssid ="**********"; 
const char* password ="********"; 
//MQTT broker
char hostname[] ="*********"; 
#define TOKEN "temalab"

WiFiClient wifiClient;
PubSubClient client(wifiClient);

int status = WL_IDLE_STATUS;


//Változók
float hum;  
float temp;
String s_temp; 
String s_hum; 
#define MAX_SIZE (100)
char c_temp[MAX_SIZE];
char c_hum[MAX_SIZE];




void setup()
{
  Serial.begin(115200);
  WiFi.begin(ssid, password); // Csatlakozás WiFi-hez

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
  hum = dht.readHumidity();// Páratartalom kiolvasása
  temp= dht.readTemperature(); //Hőmérsék kiolvasása
  
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

  // Konvertálás stringé
  s_temp="";
  s_temp += temp;
  s_hum="";
  s_hum += hum; 


  // MQTT-re küldés
  s_temp.toCharArray(c_temp, MAX_SIZE);
  client.publish("OutsideTemp", c_temp); 
  s_hum.toCharArray(c_hum, MAX_SIZE);
  client.publish("OutsideHum", c_hum); 
}

// Újra csatlakozás
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