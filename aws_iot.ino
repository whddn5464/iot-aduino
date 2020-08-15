
#include <AWS_IOT.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include "DHT.h"
#define DHTPIN 23 // what digital pin we're connected to
#define LEDPIN 15 // LED
// Uncomment whatever type you're using!
#define DHTTYPE DHT11 // DHT 11
DHT dht(DHTPIN, DHTTYPE);
boolean led_state = false;
AWS_IOT hornbill; // AWS_IOT instance
char WIFI_SSID[] = "KAU-Guest";
char WIFI_PASSWORD[] = "";
char HOST_ADDRESS[] = "aj3mwwhga0db0-ats.iot.ap-northeast-2.amazonaws.com";
char CLIENT_ID[] = "mything";
char TOPIC_NAME_update[] = "$aws/things/mything/shadow/update";
char TOPIC_NAME_delta[] = "$aws/things/mything/shadow/update/delta";
int status = WL_IDLE_STATUS;
int tick = 0, msgCount = 0, msgReceived = 0;
char payload[512];
char rcvdPayload[512];
// subscribe ../update/delta
void callBackDelta(char *topicName, int payloadLen, char *payLoad)
{
  strncpy(rcvdPayload, payLoad, payloadLen);
  rcvdPayload[payloadLen] = 0;
  msgReceived = 1;
}
void setup() {
  Serial.begin(115200);
  delay(2000);
  while (status != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(WIFI_SSID);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(WIFI_SSID, /* WIFI_PASSWORD */ NULL);

    // wait 5 seconds for connection:
    delay(5000);
  }
  Serial.println("Connected to wifi");
  if (hornbill.connect(HOST_ADDRESS, CLIENT_ID) == 0) { // Connect to AWS
    Serial.println("Connected to AWS");
    delay(1000);
    if (0 == hornbill.subscribe(TOPIC_NAME_delta, callBackDelta))
      Serial.println("Subscribe(../update/delta) Successfull");
    else {
      Serial.println("Subscribe(../update/delta) Failed, Check the Thing Name, Certificates");
      while (1);
    }
  }
  else {
    Serial.println("AWS connection failed, Check the HOST Address");
    while (1);
  }
  delay(2000);
  dht.begin(); //Initialize the DHT11 sensor
  pinMode(LEDPIN, OUTPUT);
  pinMode(DHTPIN, INPUT);
  digitalWrite(LEDPIN, LOW);
}
void loop() {
  // at first, handle subscribe messages..
  StaticJsonDocument<200> msg; // reserve stack mem for handling json msg
  if (msgReceived == 1) {
    msgReceived = 0;
    Serial.print("Received Message(Update/Delta):");
    Serial.println(rcvdPayload);
    // Deserialize the JSON document
    if (deserializeJson(msg, rcvdPayload)) { // if error
      Serial.print(("deserializeJson() failed.. \n"));
      while (1);
    }
    // parsing delta msg
    // ex) {"version":63,"timestamp":1573803485,"state":{"led":"OFF"},
    // "metadata":{"led":{"timestamp":1573803485}}}
    JsonObject state = msg["state"];
    String led=state["led"];
    Serial.print("test"+ led);
    if (led.equals("OFF")) { // turn ON
      led_state = 0;
    }
    else if (led.equals("ON")) { // turn OFF
      led_state = 1;
    }digitalWrite(LEDPIN, led_state);
  }
  
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
  if(t<23){
    led_state=1;
  }else{
    led_state=0;
  }
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
  }
  else {
    // publish report msg
    // { "state": {
    // "reported": { "temp": nn, "humid": nn, "led": "ON" | "OFF" }
    // }
    // }
    sprintf(payload, "{\"state\":{\"desired\":{\"led\":\"%s\"}}}",(led_state ? "ON" : "OFF" ));
    if (hornbill.publish(TOPIC_NAME_update, payload) == 0) { // Publish the message
    }
    sprintf(payload, "{\"state\":{\"reported\":{\"temp\":%.1f,\"humid\":%.1f,\"led\":\"%s\"}}}",
            t, h, (led_state ? "ON" : "OFF" ));
    if (hornbill.publish(TOPIC_NAME_update, payload) == 0) { // Publish the message
      Serial.print("Publish Message: ");
      Serial.println(payload);

    }
    else {
      Serial.println("Publish failed");
      
    }
    // publish the temp and humidity every 5 seconds.
    vTaskDelay(5000 / portTICK_RATE_MS);
  }
}
