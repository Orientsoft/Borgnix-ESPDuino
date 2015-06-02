/**
 * \file
 *       ESP8266 MQTT Bridge example
 * \author
 *       Tuan PM <tuanpm@live.com>
 */
#include <SoftwareSerial.h>
#include <espduino.h>
#include <mqtt.h>

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
      mqtt.connect("voyager.orientsoft.cn", 11883, false);
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
  mqtt.subscribe("/topic/0"); //or mqtt.subscribe("topic"); /*with qos = 0*/
  mqtt.subscribe("/topic/1");
  mqtt.subscribe("/topic/2");
  // mqtt.publish("/topic/3", "data0");

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
  
  mqtt.publish("/topic/3", "data0");
}
void mqttPublished(void* response)
{

}
void setup() {
  Serial.begin(19200);
  debugPort.begin(19200);
  esp.enable();
  delay(500);
  esp.reset();
  delay(500);
  while(!esp.ready());

  debugPort.println(F("ARDUINO: setup mqtt client"));
  if(!mqtt.begin("DVES_duino", "admin", "Isb_C4OGD4c3", 120, 1)) {
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
