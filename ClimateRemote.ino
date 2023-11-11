#include "Setup.h"

#include <IRremote.hpp>

#include <PubSubClient.h>
#include <WiFi.h>

WiFiClient espClient;
PubSubClient client(espClient);

String ip;
char charBuf[40];

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

  delay(100);


}


void sendOff()
{
#ifndef READ_MODE
  client.publish("mqtt.0.climateremote.message", "OFF");
  IrSender.sendNEC(0x80, 0x9C, 3);
  //IrSender.sendNECRaw(0x639C7F80,2);
#endif
}

void sendOn()
{
#ifndef READ_MODE
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
