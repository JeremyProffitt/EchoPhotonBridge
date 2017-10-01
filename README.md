# EchoPhotonBridge

### How it works

It's Simple.  An Alexa Skill interfaces to a Library on the Particle device which abstracts away all the calls and communication required to implement Amazon Home Automation Skills.  So now you can concentrate on bringing your devices to life instead of writting Lambdas, oAuth Servers and interfaces.

What does that really mean?  Simple.  You link your Particle.IO account to your Amazon Alexa account, then when you make a device that includes home automation, you simply say, "Alexa, Discover Devices" and it will find your device.  And this interface supports a wide range of controls, not just on and off. 

Using the EchoPhotonBridge, you'll be able to use phrases like:
    ```Alexa, Turn Photon Light On```,   ```Alexa, Make Photon Light Red```,
    ```Alexa, Lock File Cabinet```,  ```Alexa, what is the temperture of the blanket```
    
### What can it do?
The EchoPhotonBridge supports

  - Turn On & Off (like a Light or Fan)
  - Change a Device's Color (like a Light)
  - Set, Increase or Decrease a Percentage on a Device (like a Light Dimmer or Fan Speed)
  - Set, Increase or Decrease a Target Temperature (like for a Thermostat)
  - Ask for the Current Temperature of a Device (like how cold a room is)
  - Ask for the Target Temperature of a Device (like what the Thermostat is set to)
  - Lock a Device (like a door lock) - Alexa does not support unlocking
  - Ask for the Status of a Lock (locked or unlocked)
  - Set, Increase or Decrease a Color Temperature (like a light bulb)

### What you need?
  - [Purchase a Particle Photon](https://www.amazon.com/Particle-PHOTON-Comprehensive-Development-Access/dp/B016YNU1A0/ref=sr_1_fkmr0_1?ie=UTF8&qid=1501629481&sr=8-1-fkmr0&keywords=particle.io+photon) when you get it, Create a Particle IO Account then add your Photon to your Account using your smart phone.
  -  Program your Photon, in the Particle.io IDE, click on Libraries, search for EchoPhotonBridge and click on EchoPhotonBridge, then under examples, choose your what example to try.  Then flash your device.
  - Add the [EchoPhotonBridge Particle IO Bridge Alexa Skill](https://www.amazon.com/Particle-IO-Bridge-IOT-Pimp/dp/B074M1SLKY/ref=sr_1_1?s=digital-skills&ie=UTF8&qid=1503176563&sr=1-1&keywords=particle) to your Amazon Echo.  You will be asked for your Particle.IO Bridge login, so login - this will give the skill access to your devices.  The bridge will scan your particle account for devices running the EchoPhotonBridge Library and interface them to Amazon.  
  - Ask "Alexa Discover Devices" to trigger scanning for new devices and find your new Photon.

### Got a Code Example?
You just define the name of the device and attach a function to call - it's easy - how easy?  5 Lines of code easy!

```
        //Line 1 - Add the EchoPhontonBridge Library to your code
#include "EchoPhotonBridge.h"
        //Line 2 - Create a new EchoPhotonBridge instance
EchoPhotonBridge epb;  

        //Line 3 - Create a function to call when you tell Alexa to turn something on or off
int functionOnOff(int device, bool onOff, String rawParameters) { 
        //Line 4 - Do something when you turn something on or off, like turn on a light.
    digitalWrite(D7, (onOff) ? HIGH : LOW);
}

void setup() {
    pinMode(D7, OUTPUT);
            //Line 5 - This line adds "Photon Light" to your Alexa, 
            //         when you say "Alexa Photon Light On" the functionOnOff is called.
    epb.addEchoDeviceV2OnOff("Photon Light", &functionOnOff);  //Line 5 - Add the Photon Light to your Alexa.
}

void loop() {
}
```

### Need More Code?

Check out the examples!

### Need Help?

The Particle IO Bridge is available free.  Should you need assistance, our suggestion is to visit the [Particle.IO Community Site](https://community.particle.io/). Should you require professional assistance at our standard hourly rate, please feel free to contact [Jeremy](mailto:proffitt.jeremy@gmail.com) directly using the contact form on the home page.

If you found a bug, please document it and open an issue on github.

### Demo Video
[![EchoPhotonBridge](https://i.ytimg.com/vi/eNSgS2_YVjQ/hqdefault.jpg?sqp=-oaymwEXCNACELwBSFryq4qpAwkIARUAAIhCGAE=&rs=AOn4CLBODEUqFiHbcKjYdSeF34wuJOe7yA)](https://www.youtube.com/watch?v=eNSgS2_YVjQ "EchoPhotonBridge")

### License

[Attribution-NonCommercial-ShareAlike 4.0 International CC BY-NC-SA 4.0](ttps://creativecommons.org/licenses/by-nc-sa/4.0/)
