# What does this do?
This arduino sketch reads an IR signal from e.g. an IR remote using a KY-022 receiver and allows you to programatically send that IR signal using a KY-005 emitter. 

# Requirements
* KY-022 receiver to read an IR signal
* KY-005 transmitter to send the IR signal
* MQTT broker - to monitor status of the ESP32 and to control which IR signal to send

# Setup

Rename `Setup.h.orig` to  `Setup.h` and adjust the settings. Most of the settings should be self-explanatory.

## OTA_HASH
The `OTA_HASH` is used during OTA updates of your ESP32 to authorize the update

`#define OTA_HASH "OTA-HASH"`

Replace "OTA-HASH with the MD5 hash of your desired password using e.g. a command like `echo -n "PASSWORD"|md5`

## READ_MODE
This sketch basically has two modes: Receiving an IR signal, decoding and printing it to the serial monitor - and transmitting a (hardcoded) IR signal. 

This design was choosen so that you can connect a KY-022 receiver to get the IR signal you are interested in - and then remove that sensor from the permanent setup and only have a KY-005 connected to send the signals. This way the permanent installation of the ESP32 focuses on sending the configured signal - and does not rely on any IR receiver being connected permanently. 

In order to activate read mode, just uncomment this line: 

```
// #define READ_MODE
```

To define the PINs for receiving/sending IR signals, use the following constants:

```
#ifdef READ_MODE
#define IR_RECEIVE_PIN 32
#else
#define IR_SEND_PIN 32
#endif
```

In read mode, open the serial monitor of your arduino IDE, press the button on your IR remote you are interested in - and note the output that is printed to serial. 

Once you noted the IR codes, uncomment the `READ_MODE` line again and adjust the sketch as described below. 

# Adjusting the sketch
Currently the sketch subscribes to the MQTT topic `mqtt.0.climateremote.desired_status` and calls the function `sendOn` or `sendOff` depending on the value of the MQTT message. To change the MQTT topic or the values it should react to, please modify the function `callback` in `setup.ino`.

The IR codes that should be send are defined in `ClimateRemote.ino` in the functions `sendOff` and `sendOn`. 

The IR codes (and even the whole function calls to use) are printed out when the `READ_MODE` of the sketch is active. In my case it is the raw code `0x639C7F80`. This can be send either using the `sendNEC` method or the `sendNECRaw`.

```
// recommended format
IrSender.sendNEC(0x80, 0x9C, 3);	
// legacy format
IrSender.sendNECRaw(0x639C7F80, 3);
```

