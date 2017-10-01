/*  This sample includes the use of an ST7735 LCD, 1820 One Wire Temp Sensor
and controls the output of 2 pins which are hooked up to a fan, one for 
ossocilation and the second for fan speed through two SSR's. */

#include <OneWire.h>
#include <DS18.h>
#include <Adafruit_ST7735.h>
#include <EchoPhotonBridge.h>
//Load Fast LED Library and set it's options.
#include <FastLED.h>
FASTLED_USING_NAMESPACE;
#define PARTICLE_NO_ARDUINO_COMPATIBILITY 1

//Set Defines to set up Hardware.
#define NUM_LEDS 32
#define FASTLED_PIN 6

#define FAN D0
#define ONEWIRE D2
#define BL  D1
#define CS  A2
#define DC  D4
#define RST A1

#define TEMPLOCKTIME 30 //30 minutes

//Local Variables
int setTemp = 80;
int currentTemp;
String lastUpdate = "";
unsigned long oldTime = 0;
unsigned long tempLockTime = 0;
bool thermostat = true;


CRGB leds[NUM_LEDS];
EchoPhotonBridge epb;
Adafruit_ST7735 tft = Adafruit_ST7735(CS, DC, RST);
DS18 sensor(ONEWIRE);

int functionOnOff(int device, bool onOff, String rawParameters)
{
    tft.setTextColor((onOff) ? ST7735_GREEN : ST7735_RED, ST7735_BLACK);
    tft.setTextSize(1);
    tft.setCursor(0,140);
    tft.fillRect(0,140,128,160,ST7735_BLACK);
    switch (device)
    {
        case 0: //Fan
            analogWrite(FAN, (onOff) ? 250 : 0, 500);
            tft.println((onOff) ? "Fan On" : "Fan Off");
            tempLockTime = millis() + (1000 * 60 * TEMPLOCKTIME);
            updateThermostatDisplay("Paused");
            break;
        case 1: //LCD BackLight
            analogWrite(BL, (onOff) ? 250 : 0, 500);
            tft.println((onOff) ? "Backlight On" : "Backlight Off");
            break;
        case 2: //termostat
            thermostat = onOff;
            updateThermostatDisplay((onOff) ? "On" : "Off");
            break;
        case 3: //Fast LED Array
            for (int i = 0; i < NUM_LEDS; i++) 
            {
                leds[i] = (onOff) ? CRGB::White : CRGB::Black;
            }
                FastLED.show();
            tft.println((onOff) ? "Light On" : "Light Off");
            break;
    }
    return 0;
}

int functionPercent(int device, int percent, int changeAmount, String rawParameters)
{
    tft.setTextColor(ST7735_BLUE, ST7735_BLACK);
    tft.setTextSize(1);
    tft.setCursor(0,140);
    tft.fillRect(0,140,128,160,ST7735_BLACK);
    switch (device)
    {
        case 0: //Fan
            analogWrite(FAN, percent * 2.5, 500);
            tft.println("Fan to " + String(percent) + "%");
            tempLockTime = millis() + (1000 * 60 * TEMPLOCKTIME);
            updateThermostatDisplay("Paused");
            break;
        case 1: //LCD BackLight
            analogWrite(BL, percent * 2.5, 500);
            tft.println("LCD BL to " + String(percent) + "%");
            break;
        case 3: //Fast LED
            
            FastLED.show();
            break;    
    }
    
    return 0;
}

int functionColor(int device, int R, int G, int B, String rawParameters)
{
    for (int i = 0; i < NUM_LEDS; i++) 
    {
        leds[i].r = R;
        leds[i].g = G;
        leds[i].b = B;
    }
    FastLED.show();
    
    return device * 1000000;
}



int functionTemp(int device, bool requestCurrentTemp, bool requestSetTemp, int temperature, int changeAmount, String rawParameters)
{
    if (requestCurrentTemp) return currentTemp;
    if (requestSetTemp) return setTemp;
    setTemp = temperature;
    thermostat = true;
    updateThermostatDisplay("On");
    return 0;
}

void updateThermostatDisplay(String state) 
{
    tft.setCursor(0,80);
    tft.println("Thermostat " + String(setTemp) + " F");
    tft.fillRect(0,80,128,120,ST7735_BLACK);
    tft.setCursor(0,100);
    tft.println(state);
}

void setup() {
    pinMode(FAN, OUTPUT);
    analogWrite(FAN, 0, 500);
    
    pinMode(BL, OUTPUT);
    analogWrite(BL, 250, 500);
    
    FastLED.addLeds<WS2812, FASTLED_PIN, GRB>(leds, NUM_LEDS);

    String deviceName = "Cool Breeze";
    String deviceType = "Fan";
    epb.addEchoDeviceV2OnOff(deviceName + " " + deviceType, &functionOnOff);
    epb.addEchoDeviceV2Percent(deviceName + " " + deviceType, &functionPercent);
    epb.addEchoDeviceV2Temp(deviceName + " " + deviceType, &functionTemp);
    
    deviceType = "BackLight";
    epb.addEchoDeviceV2OnOff(deviceName + " " + deviceType, &functionOnOff);
    epb.addEchoDeviceV2Color(deviceName + " " + deviceType, &functionColor);
    epb.addEchoDeviceV2Percent(deviceName + " " + deviceType, &functionPercent);
    
    deviceType = "Thermostat";
    epb.addEchoDeviceV2OnOff(deviceName + " " + deviceType, &functionOnOff);
   
    deviceType = "Light";
    epb.addEchoDeviceV2OnOff(deviceName + " " + deviceType, &functionOnOff);
    epb.addEchoDeviceV2Color(deviceName + " " + deviceType, &functionColor);
    epb.addEchoDeviceV2Percent(deviceName + " " + deviceType, &functionPercent);
    
    tft.initG();
    tft.fillScreen(ST7735_BLACK);
    //tft.setFont(TIMESNEWROMAN8);
    tft.setTextSize(1);
    tft.setTextColor(ST7735_WHITE);
	tft.setCursor(10,120);
    tft.setTextColor(ST7735_WHITE);
    tft.setTextWrap(true);
    tft.println(deviceName);  
    
    functionOnOff(2,true,""); //Turn on Thermostat.
    oldTime = millis();
}

void loop() {
    
    if(millis() - oldTime >= 1000)
    {
        
        if (sensor.read()) {
            tft.setTextColor(ST7735_BLACK);
            tft.setCursor(20, 20);
            tft.setTextSize(4);
            tft.print(lastUpdate);
            tft.setTextColor(ST7735_WHITE);
            tft.setCursor(20,20);
            lastUpdate = String::format("%1.1f",sensor.fahrenheit());
            tft.print(lastUpdate);
            Particle.publish("sumologic", lastUpdate);
            
            currentTemp = sensor.fahrenheit();
            if (tempLockTime < millis() && thermostat) 
            {
                if (currentTemp > setTemp + 1)
                {
                    functionOnOff(0,true,"");
                }
                else if (currentTemp < setTemp - 1)
                {
                    functionOnOff(0,false,"");                
                }
            }
        }   
        oldTime = millis(); //update old_time to current millis()
    }
}