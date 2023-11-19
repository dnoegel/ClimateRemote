#include "Setup.h"

#include <IRremote.hpp>

#include <PubSubClient.h>
#include <WiFi.h>

WiFiClient espClient;
PubSubClient client(espClient);

String ip;
char charBuf[40];
bool acActive = false;

void setup()
{
  Serial.begin(SERIAL_SPEED);
  ensureWifiConnected();
  setupWebUpdater();

#ifdef READ_MODE
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
#else
  IrSender.begin(IR_SEND_PIN);
#endif

#ifndef READ_MODE
  setupMqtt();
#endif
}


void loop()
{
#ifndef READ_MODE
  loopMqtt();
#endif

  loopWebUpdater();


#ifdef READ_MODE
  handleRead();
#endif

#ifndef READ_MODE
  checkACActive();
#endif

  delay(200);


}

void checkACActive()
{
  int photodiodeValue = analogRead(PHOTO_DIODE_PIN);
  bool newActive = photodiodeValue < PHOTO_DIODE_THRESHOLD;

   itoa(photodiodeValue, charBuf, 10);
  client.publish("mqtt.0.climateremote.debug_diaod", charBuf);
  Serial.println(photodiodeValue);

  // change
  if (newActive != acActive) {
    acActive = newActive;
    Serial.print("Update status, new status is ");
    Serial.println(acActive);
    String boolAsString = acActive ? "true" : "false";
    client.publish("mqtt.0.climateremote.actual_state", boolAsString.c_str());
  }

}

void sendOff()
{
#ifndef READ_MODE
  // if AC is already off: do nothing
  checkACActive();
  if (acActive == false) {
    return;
  }

  client.publish("mqtt.0.climateremote.message", "OFF");
  IrSender.sendNEC(0x80, 0x9C, 3);

  
  //IrSender.sendNECRaw(0x639C7F80,2);
#endif
}

void sendOn()
{
#ifndef READ_MODE
  checkACActive();
  // if AC is already on: do nothing
  if (acActive == true) {
    return;
  }

  Serial.println("Send on");
  client.publish("mqtt.0.climateremote.message", "ON");
  IrSender.sendNEC(0x80, 0x9C, 3);
  //IrSender.sendNECRaw(0x639C7F80,2);
#endif
}


#ifdef READ_MODE
void handleRead()
{
  if (IrReceiver.decode()) {
    Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX); // Print "old" raw data

    IrReceiver.printIRResultShort(&Serial); // Print complete received data in one line
    IrReceiver.printIRSendUsage(&Serial);   // Print the statement required to send this data

    IrReceiver.resume();
  }

}
#endif
