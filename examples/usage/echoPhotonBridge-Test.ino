/*

This is the test of the Lock, Temperature, On/Off and Color functions.  This is a great place to 
understand how the different functions work.

You need a 8 or more LED NeoPixel / WS2812 array or anything FastLED capabile attached on pin 6.

Don't forget to ask, "Alexa, Discover Devices" once everything is set up to detect new test devices.


*/
#include <FastLED.h>
FASTLED_USING_NAMESPACE;
#define PARTICLE_NO_ARDUINO_COMPATIBILITY 1

#include <EchoPhotonBridge.h>

#define NUM_LEDS 32
#define FASTLED_PIN 6


CRGB leds[NUM_LEDS];
EchoPhotonBridge epb;

int currentTemp = 81;
int setTemp = 78;

/*

Test Device Names: Photon, Blanket, Test Room

Test Utterances:

Alexa, Turn Photon On
Alexa, Turn Photon Off
Alexa, Make Photon Red
Alexa, Make Photon Blue
Alexa, Set Photon to 10 Percent
Alexa, Set Photon to 90 Percent

Alexa, Turn Test Room On
Alexa, Turn Test Room Off
Alexa, Make Test Room Red
Alexa, Make Test Room Blue
Alexa, Set Test Room to 10 Percent
Alexa, Set Test Room to 90 Percent
Alexa, Lock Test Room

Alexa, Set the Temperature of my blanket to 90 degrees
Alexa, What is the temperature of my blanket?
Alexa, Turn Blanket On
Alexa, Turn Blanket Off

*/


int functionOnOff(int device, bool onOff, String rawParameters)
{
    leds[device] = (onOff) ? CRGB::White : CRGB::Black;
    FastLED.show();
    return 0;
}

int functionPercent(int device, int percent, int changeAmount, String rawParameters)
{
    leds[device + 3].r = 255 - (255 * percent / 100);
    leds[device + 3].g = percent / 100 * 255;
    leds[device + 3].b = percent + 50;
    FastLED.show();
    return 0;
}

int functionColor(int device, int R, int G, int B, String rawParameters)
{
    leds[device].r = R;
    leds[device].g = G;
    leds[device].b = B;
    FastLED.show();
    return 0;
}

int functionTemp(int device, bool requestCurrentTemp, bool requestSetTemp, int temperature, int changeAmount, String rawParameters)
{
    leds[6] = CRGB::White;
    if (requestCurrentTemp) return currentTemp;
    if (requestSetTemp) return setTemp;
    setTemp = temperature;
    leds[6].r = setTemp;
    leds[6].g = setTemp;
    leds[6].b = setTemp;
    FastLED.show();
    return 0;
}

bool virtualLock = false;

//functionLock tells a device to change color or requests the status of a lock.
//(int, bool, bool, String);
int functionLock(int device, bool requestStatus, bool locked, String rawParameters)
{
    if (requestStatus) return ((virtualLock) ? 1 : 0);
    
    virtualLock = locked;
    
    leds[7] = (virtualLock) ? CRGB::Red : CRGB::Green; 
    FastLED.show();
    
    return 0;
}


void setup() 
{
    FastLED.addLeds<WS2812, FASTLED_PIN, GRB>(leds, NUM_LEDS);

    String deviceName = "Photon";
    epb.addEchoDeviceV2OnOff(deviceName, &functionOnOff);
    epb.addEchoDeviceV2Percent(deviceName, &functionPercent);
    epb.addEchoDeviceV2Color(deviceName, &functionColor);
    
    deviceName = "Test Room";
    epb.addEchoDeviceV2OnOff(deviceName, &functionOnOff);
    epb.addEchoDeviceV2Percent(deviceName, &functionPercent);
    epb.addEchoDeviceV2Color(deviceName, &functionColor);
    epb.addEchoDeviceV2Lock(deviceName, &functionLock);
    
    deviceName = "Blanket";
    epb.addEchoDeviceV2OnOff(deviceName, &functionOnOff);
    epb.addEchoDeviceV2Temp(deviceName, &functionTemp);
    
    leds[0] = CRGB::Pink;
    leds[1] = CRGB::Blue;
    leds[2] = CRGB::Orange;
    leds[3] = CRGB::Purple;
    leds[4] = CRGB::Yellow;
    leds[5] = CRGB::Green;
    leds[6] = CRGB::Red;
    leds[7] = CRGB::White;
    FastLED.show();
}

void loop() 
{
    
}