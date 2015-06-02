/**
 * \file
 *       ESP8266 MQTT Bridge example
 * \author
 *       Tuan PM <tuanpm@live.com>
 */
#include <SoftwareSerial.h>
#include <espduino.h>
#include <mqtt.h>

/* Get your UUID/Token from http://z.borgnix.com/ */

#define UUID "be9cb500-08d7-11e5-a857-87f83ec252b4"
#define TOKEN "da2f22abbe96bcab8d10204841d852f1c3316431"

#define LEDPin 13
#define ButtonPin 0

int buttonState;

SoftwareSerial debugPort(7, 8); // RX, TX
ESP esp(&Serial, &debugPort, 4);
MQTT mqtt(&esp);
boolean wifiConnected = false;



void wifiCb(void* response)
{
  uint32_t status;
  RESPONSE res(response);

  if(res.getArgc() == 1) {
    res.popArgs((uint8_t*)&status, 4);
    if(status == STATION_GOT_IP) {
      debugPort.println(F("WIFI CONNECTED"));
      mqtt.connect("z.borgnix.com", 1883, false);
      wifiConnected = true;
      //or mqtt.connect("host", 1883); /*without security ssl*/
    } else {
      wifiConnected = false;
      mqtt.disconnect();
    }
    
  }
}

void mqttConnected(void* response)
{
  debugPort.println(F("Connected"));
  
  String BorgChannelStr = "/devices/be9cb500-08d7-11e5-a857-87f83ec252b4/out";
  int Channel_len = BorgChannelStr.length() + 1;
  char Channel[Channel_len];
  BorgChannelStr.toCharArray(Channel, Channel_len);    
  mqtt.subscribe(Channel); //or mqtt.subscribe("topic"); /*with qos = 0*/


}
void mqttDisconnected(void* response)
{

}
void mqttData(void* response)
{
  RESPONSE res(response);

  debugPort.print(F("Received: topic="));
  String topic = res.popString();
  debugPort.println(topic);

  debugPort.print(F("data="));
  String data = res.popString();
  debugPort.println(data);

  if (data.equals("1"))
     digitalWrite(LEDPin, HIGH);   // turn the LED on (HIGH is the voltage level)
  else if (data.equals("0"))
     digitalWrite(LEDPin, LOW);
  

}

void mqttPublished(void* response)
{

}

void stateChange()
{
   debugPort.println(F("Button press!"));
   
   buttonState = digitalRead(ButtonPin);
   if(wifiConnected) {
     if (buttonState = HIGH)
       mqtt.publish("/devices/be9cb500-08d7-11e5-a857-87f83ec252b4/in", "1");
      if (buttonState = LOW)
       mqtt.publish("/devices/be9cb500-08d7-11e5-a857-87f83ec252b4/in", "0");  
   }
 
}

void setup() {
  // initialize digital pin 13 as an output for LED.
  pinMode(LEDPin, OUTPUT);  
  attachInterrupt(ButtonPin, stateChange, RISING);
  
  Serial.begin(19200);
  debugPort.begin(19200);
  esp.enable();
  delay(500);
  esp.reset();
  delay(500);
  while(!esp.ready());

  debugPort.println(F("ARDUINO: setup mqtt client"));
  if(!mqtt.begin("espDuino", UUID, TOKEN, 120, 1)) {
    debugPort.println(F("ARDUINO: fail to setup mqtt"));
    while(1);
  }


  debugPort.println(F("ARDUINO: setup mqtt lwt"));
  mqtt.lwt("/lwt", "offline", 0, 0); //or mqtt.lwt("/lwt", "offline");

/*setup mqtt events */
  mqtt.connectedCb.attach(&mqttConnected);
  mqtt.disconnectedCb.attach(&mqttDisconnected);
  mqtt.publishedCb.attach(&mqttPublished);
  mqtt.dataCb.attach(&mqttData);

  /*setup wifi*/
  debugPort.println(F("ARDUINO: setup wifi"));
  esp.wifiCb.attach(&wifiCb);

  esp.wifiConnect("HiWiFi_orientsoft","welcome1");


  debugPort.println(F("ARDUINO: system started"));
}

void loop() {

 
  esp.process();
  if(wifiConnected) {
   }

}
