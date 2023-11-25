#include "Setup.h"

#include <IRremote.hpp>

#include <PubSubClient.h>
#include <WiFi.h>

WiFiClient espClient;
PubSubClient client(espClient);

// For reading the IR code use File->Examples->IRemote->SimpleReceiver

String ip;
char charBuf[40];
bool acActive = false;

void setup()
{
  Serial.begin(SERIAL_SPEED);
  ensureWifiConnected();
  setupWebUpdater();

  IrSender.begin(IR_SEND_PIN);

  setupMqtt();
}


void loop()
{
  loopMqtt();
  loopWebUpdater();

  checkACActive();


  delay(200);


}

void checkACActive()
{
  int photodiodeValue = analogRead(PHOTO_DIODE_PIN);
  bool newActive = photodiodeValue < PHOTO_DIODE_THRESHOLD;

  // change
  if (newActive != acActive) {
    acActive = newActive;
    Serial.print("Update status, new status is ");
    Serial.println(acActive);
    String boolAsString = acActive ? "true" : "false";
    client.publish("mqtt.0.climateremote.actual_state", boolAsString.c_str());
  }

  if (random(1, 101) < 10) {
    itoa(photodiodeValue, charBuf, 10);
    client.publish("mqtt.0.climateremote.debug_diaod", charBuf);
    Serial.println(photodiodeValue);
  }


}

void sendOff()
{
  // if AC is already off: do nothing
  checkACActive();
  if (acActive == false) {
    return;
  }

  client.publish("mqtt.0.climateremote.message", "OFF");
  IrSender.sendNEC(0x80, 0x9C, 3);
}

void sendOn()
{
  checkACActive();
  // if AC is already on: do nothing
  if (acActive == true) {
    return;
  }

  Serial.println("Send on");
  client.publish("mqtt.0.climateremote.message", "ON");
  IrSender.sendNEC(0x80, 0x9C, 3);
}
