/* 
 * IoT moitpring system for Temperature and humidity control devices with relays using ThingSpeak.
 * used devices:
 *    Relay
 *    Buzzer
 *      Buzzer_PIN 4
 *    DHT11     
 *      DHT_PIN 2
 */


#include <DHT.h>  // Including library for dht
 #include <ESP8266WiFi.h>
 
String apiKey = "YP6BDJ80A1AF3HNJ";     //  Enter your Write API key from ThingSpeak
const char *ssid =  "msh";     // replace with your wifi ssid and wpa2 key
const char *pass =  "12345678";
const char* server = "api.thingspeak.com";
#define BUZZER_PIN 4
#define BUZZER(x){digitalWrite(BUZZER_PIN,x);}
#define DHTPIN 2          //pin where the dht11 is connected 
DHT dht(DHTPIN, DHT11);
 
WiFiClient client;
 
void setup(){
  Serial.begin(115200);
  delay(10);
  pinMode(BUZZER_PIN,OUTPUT);
  dht.begin();
  Serial.println("Connecting to ");Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}
 
void loop(){
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)){
    Serial.println("Failed to read from DHT sensor!");
    return;
   }
  if(t>30){
    BUZZER(1);
delay(1000);
    BUZZER(0);
delay(1000);
  }
 if (client.connect(server,80)){    //   "184.106.153.149" or api.thingspeak.com
    //GET https://api.thingspeak.com/update?api_key=YP6BDJ80A1AF3HNJ&field1=0

    String postStr=apiKey;
    postStr +="&field1=";
    postStr += String(t);
    postStr +="&field2=";
    postStr += String(h);
    postStr += "\r\n\r\n";
    
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
    Serial.println(postStr);
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print(" degrees Celcius, Humidity: ");
    Serial.print(h);
    Serial.println("%. Send to Thingspeak.");
  }
  client.stop();
  Serial.println("Waiting...");
  // thingspeak needs minimum 15 sec delay between updates
  delay(1000);
}
