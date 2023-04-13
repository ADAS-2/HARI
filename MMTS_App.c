#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
// WiFi
const char *ssid = "Redmi Note 7"; // Enter your WiFi name
const char *password = "123456789";  // Enter WiFi password

// MQTT Broker
const char *mqtt_broker = "broker.hivemq.com";
const char *topic = "resulttopic";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  // Set software serial baud to 115200;
  Serial.begin(115200);
  // connecting to a WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  //connecting to a mqtt broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  while (!client.connected()) {
      String client_id = "esp8266-client-";
      client_id += String(WiFi.macAddress());
      Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
      if (client.connect(client_id.c_str())) {
          Serial.println("Public  mqtt broker connected");
      } else {
          Serial.print("failed with state ");
          Serial.print(client.state());
          delay(2000);
      }
  }
  // publish and subscribe
  client.publish(topic, "Topic Published");
  client.subscribe("starttopic");
}

void callback(char *topic, byte *payload, unsigned int length) {
 Serial.print("Received [");
  Serial.print(topic);
  Serial.print("]: ");
  for (int i = 0; i < length; i++)
  {
         Serial.print((char)payload[i]);
  }

   Serial.println();
  
}

void loop() {
  client.loop();

   if(Serial.available())
  {
    char rec = Serial.read();
       publishMessage(rec);


  }
}
void publishMessage(char rec)
{
  StaticJsonDocument<200> doc;
 // doc["time"] = millis();

  doc["Status"] =rec;
  
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client
 
  client.publish("resulttopic", jsonBuffer);

}
