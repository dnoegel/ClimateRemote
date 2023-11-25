# What does this do?
This arduino sketch allows you to programatically send an IR signal using a KY-005 emitter. 

# Requirements
* KY-005 transmitter to send the IR signal
* MQTT broker - to monitor status of the ESP32 and to control which IR signal to send

# Setup

Rename `Setup.h.orig` to  `Setup.h` and adjust the settings. Most of the settings should be self-explanatory.

## OTA_HASH
The `OTA_HASH` is used during OTA updates of your ESP32 to authorize the update

`#define OTA_HASH "OTA-HASH"`

Replace "OTA-HASH with the MD5 hash of your desired password using e.g. a command like `echo -n "PASSWORD"|md5`

## PHOTO_DIODE_*
I use a photo diode (BPW 40) to identify if the AC is running. The diode is glued to one of the LEDs of the AC which only glow when the AC is active. When the value returned by the diode is below a given threshold, the AC is considered active. This information is then published via MQTT so I can show it in e.g. my Iobroker visualization. 

```
#define PHOTO_DIODE_PIN 34 
#define PHOTO_DIODE_THRESHOLD 4000
```

# Adjusting the sketch
Currently the sketch subscribes to the MQTT topic `mqtt.0.climateremote.desired_status` and calls the function `sendOn` or `sendOff` depending on the value of the MQTT message. To change the MQTT topic or the values it should react to, please modify the function `callback` in `setup.ino`.

The IR codes that should be send are defined in `ClimateRemote.ino` in the functions `sendOff` and `sendOn`. 

# How to find the IR code
The IR codes (and even the whole function calls to use) can be found by using the sketch `SimpleReceiver` from `File->Examples->IRemote` in your Arduino IDE. In my case it is the raw code `0x639C7F80`. This can be send either using the `sendNEC` method or the `sendNECRaw`.

```
// recommended format
IrSender.sendNEC(0x80, 0x9C, 3);	
// legacy format
IrSender.sendNECRaw(0x639C7F80, 3);
```

