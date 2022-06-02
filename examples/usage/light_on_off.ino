// This #include statement was automatically added by the Particle IDE.
#include "EchoPhotonBridge.h"

EchoPhotonBridge epb;

//function OnOff - this turns a device on or off
int functionOnOff(int device, bool onOff, String rawParameters)
{
    digitalWrite(D7, (onOff) ? HIGH : LOW);
    return 0;
}

//  REMINDER: Tell your echo, "Alexa, Discover Devices" after you program your photon so Amazon picks it up.

void setup()
{
    pinMode(D7, OUTPUT);
    epb.addEchoDeviceV2OnOff("Photon Light", &functionOnOff);
}

void loop() 
{
  
}
