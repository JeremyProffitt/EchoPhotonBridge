/*  This example uses PWM to turn on and off and set speed/level on two pins, 
one was set up for a fan, the second for a light.  These can be DC devices
controlled through MOSFET's or SSR's to control AC devices.  The OLED is the
very popular 128x64 OLED I2C device and is useful for troubleshooting. */

#include <EchoPhotonBridge.h>
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

#define OLED_RESET D4
Adafruit_SSD1306 display(OLED_RESET);
EchoPhotonBridge epb;

#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 

//Pin assignments
#define FAN A4
#define LIGHT A5

//#define DEBUG_PUBLISH_TO "sumologic"

void setup()   {     
    pinMode(FAN, OUTPUT);
    analogWrite(FAN, 0, 500);
    
    pinMode(LIGHT, OUTPUT);
    analogWrite(LIGHT, 0, 500);
      
    
    String deviceName = "Chair Fan";
    epb.addEchoDeviceV2OnOff(deviceName, &functionOnOff);
    epb.addEchoDeviceV2Percent(deviceName, &functionPercent);
    
    deviceName = "Chair Light";
    epb.addEchoDeviceV2OnOff(deviceName, &functionOnOff);
    epb.addEchoDeviceV2Percent(deviceName, &functionPercent);
    
    display.begin(SSD1306_SWITCHCAPVCC, 0x3c);  // initialize with the I2C addr 0x3D (for the 128x64)
    display.clearDisplay();
    display.display();
    display.setTextSize(3);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.println("Chair");
    display.display();
 
}



void loop() {
  
}

void writeLCD(int Size, String Line1, String Line2, String Line3, String Line4)
{
  display.clearDisplay();
  display.display();
  display.setTextSize(Size);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println(Line1);
  display.println(Line2);
  display.println(Line3);
  display.println(Line4);
  display.display();
 
}

//function OnOff - this turns a device on or off
int functionOnOff(int device, bool onOff, String rawParameters)
{
    #ifdef DEBUG_PUBLISH_TO
        Particle.publish(DEBUG_PUBLISH_TO, String("functionOnOff  " + String(device) + "  " + String(onOff) + "  " + rawParameters));
    #endif
    
    writeLCD(2,"Chair",(device==0) ? " Fan" : " Light", "  Is",(onOff) ? "   On" : "   Off");
    
    analogWrite((device == 0) ? FAN : LIGHT, (onOff) ? 250 : 0, 500);
            
    return 0;
}




int functionPercent(int device, int percent, int changeAmount, String rawParameters)
{
    #ifdef DEBUG_PUBLISH_TO
        Particle.publish(DEBUG_PUBLISH_TO, String("functionPercent  " + String(device) + "  " + String(percent) + "  " + String(changeAmount) + "  " + rawParameters));
    #endif
    
    writeLCD(2,"Chair",(device==0) ? " Fan" : " Light", "  at","     " + String(percent) + "%");
    
    analogWrite((device == 0) ? FAN : LIGHT, percent * 2.5, 500);
   
    return percent;
}

