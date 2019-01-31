
//NOTE:  FOR THIS EXAMPLE YOU MUST INCLUDE THE FAST LED LIBRARY "FastLED"
//       You also need the gradiantPalettes.h from the samples

#include <FastLED.h>
FASTLED_USING_NAMESPACE;


#define PARTICLE_NO_ARDUINO_COMPATIBILITY 1

#include <EchoPhotonBridge.h>
#define MAX_ECHO_DEVICESV2 20
#include "gradiantPalettes.h"

#define NUM_LEDS 32 // 300 for bedroom
#define COLOR_ORDER_GRB false  //RGB
#define CURRENT_PALETTE christmas
#define GLITTER false
#define TESTING false
#define FASTLED_PIN D6  //D2 for bedroom
#define LOOPDELAY 125  //delay between loops in MS 10 for fire
#define DEFAULTMODE 2



CRGB leds[NUM_LEDS];
EchoPhotonBridge epb;

// Forward declarations of an array of cpt-city gradient palettes, and
// a count of how many there are.  The actual color palette definitions
// are at the bottom of this file.
extern const TProgmemRGBGradientPalettePtr gGradientPalettes[];
extern const uint8_t gGradientPaletteCount;
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))
// Current palette number from the 'playlist' of color palettes
uint8_t gCurrentPaletteNumber = 0;
CRGBPalette16 gCurrentPalette( CRGB::Black);
CRGBPalette16 gTargetPalette( gGradientPalettes[0] );

// Gradient palette "fire", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/neota/elem/tn/fire.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 28 bytes of program space.

DEFINE_GRADIENT_PALETTE( digital_fire ) {
    0,   1,  1,  0,
   20,  32,  5,  0,
   60, 192, 24,  0,
  100, 220,105,  5,
  240, 252,255, 31,
  250, 252,255,111,
  255, 255,255,255};


/*
DEFINE_GRADIENT_PALETTE( heatmap_gp ) {
  0,     0,  0,  0,   //black
128,   255,  0,  0,   //red
129,     0,255,  0,   //green
180,     0,  0,255,   //blue
224,   255,255,  0,   //bright yellow
255,   255,255,255 }; //full white
*/

DEFINE_GRADIENT_PALETTE( christmas ) {
    0, 255,   0,   0, //Red
   59, 255,   0,   0,
   60,   0,   0, 255,
  119,   0,   0, 255,
  120,   0, 255,   0,
  179,   0, 255,   0,
  180, 255, 255, 255,
  255,   0,   0,   0};

DEFINE_GRADIENT_PALETTE( halloween ) {
    0,   1,  1,    0,
   20, 230,  15, 230,
  170, 230,  15, 230,
  180, 255, 100,   0,
  250, 255, 100,   0,
  255, 255, 255, 255};

int gHue = 0; // rotating "base color" used by many of the patterns
bool glitter = false;
int LightsR = 0;
int LightsG = 0;
int LightsB = 0;

typedef void (*Pattern)();
typedef Pattern PatternList[];
typedef struct {
  Pattern pattern;
  String name;
} PatternAndName;
typedef PatternAndName PatternAndNameList[];

// List of patterns to cycle through.  Each is defined as a separate function below.
int mode = -1;
PatternAndNameList patterns = {
  //{ america, "America" },  // Mode 0
  //{ colorwaves, "Color Waves" },  //Mode 0
  //{ pride, "Pride" },  // Mode 1
  { rainbow, "Rainbow" },  //2
  { confetti, "Confetti" },  //3
  //{ sinelon, "Sylon" },  //5
  { fire, "Fire" }  //4
  //{ juggle, "Juggle" } //5
  //{ bpm, "BPM" }
};
int patternCount = ARRAY_SIZE(patterns);

int lastMillis = 0;


int functionOnOff(int device, bool onOff, String rawParameters)
{
    if (onOff == false) {
        return functionMainOnOff(device,onOff,rawParameters);  //Let the main onoff function handle turning devies off.
    } else {
        mode = device;
    }
    lastMillis = millis() - LOOPDELAY;
    return mode;
}

int functionMainOnOff(int device, bool onOff, String rawParameters)
{
    if (onOff == false) {
        mode = -1;
        glitter = GLITTER;
        for (int i = 0; i < NUM_LEDS; i++) {
            leds[i] = CRGB::Black;
        }
        FastLED.show();
    } else {
        glitter = GLITTER;
        LightsG=255;
        LightsR=255;
        LightsB=255;
        mode = DEFAULTMODE; 
    }
    return mode;
}

int functionGlitterOnOff(int device, bool onOff, String rawParameters) {
    glitter = onOff;
    lastMillis = millis() - LOOPDELAY;
    return 0;
}

int functionPercent(int device, int percent, int changeAmount, String rawParameters)
{
    setBrightness(percent);
    FastLED.show();
    return percent;
}



int functionColor(int device, int R, int G, int B, String rawParameters)
{
    mode = -2;
    LightsG=G;
    LightsR=R;
    LightsB=B;
    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i].g = G;
        leds[i].r = R;
        leds[i].b = B;
    }
    FastLED.show();
    return 0;
}

int setMode(String StrMode) {
    char inputStr[64];
    StrMode.toCharArray(inputStr,64);
    mode = atoi(inputStr);
    return mode;
}

void nameHandler(const char *topic, const char *data) {
    String deviceName = String(data);
    deviceName = deviceName.replace("_"," ");
    
    String name = "";
    
    for (int i = 0; i < patternCount; i++) {
        name = deviceName + " " + patterns[i].name;
        epb.addEchoDeviceV2OnOff(name, &functionOnOff);
        epb.addEchoDeviceV2Percent(name, &functionPercent);
        epb.addEchoDeviceV2Color(name, &functionColor);
    }
    
    //These have to be after the device with pattern name defintions so that device is the same as the mode.
    epb.addEchoDeviceV2OnOff(deviceName, &functionMainOnOff);
    epb.addEchoDeviceV2Percent(deviceName, &functionPercent);
    epb.addEchoDeviceV2Color(deviceName, &functionColor);
    
    epb.addEchoDeviceV2OnOff(deviceName + " Glitter", &functionGlitterOnOff);
}


void setup() 
{
    Particle.subscribe("particle/device/name", nameHandler);
    Particle.publish("particle/device/name");
    
    if (COLOR_ORDER_GRB) {
        FastLED.addLeds<WS2812, FASTLED_PIN, GRB>(leds, NUM_LEDS);
    } else {
        FastLED.addLeds<WS2812, FASTLED_PIN>(leds, NUM_LEDS);
    }
    
    if (TESTING) {
        leds[0] = CRGB::Red;
        leds[1] = CRGB::Green;
        
        leds[2] = CRGB::Blue;
        leds[3] = CRGB::White;
        leds[4] = CRGB::Red;
        
        leds[5] = CRGB::Green;
        leds[6] = CRGB::Blue;
        leds[7] = CRGB::White;
    } else {
        mode = DEFAULTMODE;
    }
    Particle.variable("Mode",mode);
    Particle.variable("Glitter",glitter);
    Particle.function("setMode",  &setMode);
    
    FastLED.show();
    lastMillis = millis();
    

}





void loop() 
{
    //digitalClockDisplay();
    if(millis() - lastMillis >= LOOPDELAY)
    {
    
        // Add entropy to random number generator; we use a lot of it.
        random16_add_entropy(random(65535));
        
        if (mode > -1) {
            patterns[mode].pattern();
        } else if (mode == -2) {
            for (int i = 0; i < NUM_LEDS; i++) {
                leds[i].g = LightsG;
                leds[i].r = LightsR;
                leds[i].b = LightsB;
            }
        }
        
        if (glitter) 
        {
            addGlitter(50);
        }
        
        FastLED.show();
        lastMillis = millis();
    }
}


int americaIndex = 0;
void america() {
    for (int i = 0; i < NUM_LEDS; i++) {
        switch ((i + americaIndex) % 3) {
            case 0: 
                leds[i] = CRGB::Red;
                break;
            case 1: 
                leds[i] = CRGB::White;
                break;
            case 2: 
                leds[i] = CRGB::Blue;
                break;
        }
    }
    americaIndex = (americaIndex++ % 3);
}

void fire()
{
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;

  // uint8_t sat8 = beatsin88( 87, 220, 250);
  uint8_t brightdepth = beatsin88( 341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = sHue16;//gHue * 256;
  uint16_t hueinc16 = beatsin88(113, 300, 1500);

  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis ;
  sLastMillis  = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88( 400, 5, 9);
  uint16_t brightnesstheta16 = sPseudotime;

  for ( int i = 0 ; i < NUM_LEDS; i++) {
    if (random(0,255) > 200) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;
    uint16_t h16_128 = hue16 >> 7;
    if ( h16_128 & 0x100) {
      hue8 = 255 - (h16_128 >> 1);
    } else {
      hue8 = h16_128 >> 1;
    }

    brightnesstheta16  += brightnessthetainc16;
    uint16_t b16 = sin16( brightnesstheta16  ) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);

    uint8_t index = random(0,255);
    //index = triwave8( index);
    index = scale8( index, 240);
    gCurrentPalette = CURRENT_PALETTE;
    CRGB newcolor = ColorFromPalette(gCurrentPalette, index, bri8);

    nblend(leds[i], newcolor, 128);
    }
  }
}

int brightness = 100;

void setBrightness(int value)
{
  // don't wrap around at the ends
  if (value > 255)
    value = 255;
  else if (value < 0) value = 0;

  brightness = value;

  FastLED.setBrightness(brightness);

}



void rainbow()
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 10);
}


void addGlitter( int chanceOfGlitter)
{
  if ( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti()
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16(13, 0, NUM_LEDS - 1);
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for ( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
  }
}

void juggle()
{
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for ( int i = 0; i < 8; i++)
  {
    leds[beatsin16(i + 7, 0, NUM_LEDS)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

// Pride2015 by Mark Kriegsman: https://gist.github.com/kriegsman/964de772d64c502760e5
// This function draws rainbows with an ever-changing,
// widely-varying set of parameters.
void pride() {
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;

  uint8_t sat8 = beatsin88( 87, 220, 250);
  uint8_t brightdepth = beatsin88( 341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = sHue16;//gHue * 256;
  uint16_t hueinc16 = beatsin88(113, 1, 3000);

  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis ;
  sLastMillis  = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88( 400, 5, 9);
  uint16_t brightnesstheta16 = sPseudotime;

  for ( uint16_t i = 0 ; i < NUM_LEDS; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;

    brightnesstheta16  += brightnessthetainc16;
    uint16_t b16 = sin16( brightnesstheta16  ) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);

    CRGB newcolor = CHSV( hue8, sat8, bri8);

    nblend(leds[i], newcolor, 64);
  }
}

// ColorWavesWithPalettes by Mark Kriegsman: https://gist.github.com/kriegsman/8281905786e8b2632aeb
// This function draws color waves with an ever-changing,
// widely-varying set of parameters, using a color palette.
void colorwaves()
{
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;

  // uint8_t sat8 = beatsin88( 87, 220, 250);
  uint8_t brightdepth = beatsin88( 341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = sHue16;//gHue * 256;
  uint16_t hueinc16 = beatsin88(113, 300, 1500);

  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis ;
  sLastMillis  = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88( 400, 5, 9);
  uint16_t brightnesstheta16 = sPseudotime;

  for ( uint16_t i = 0 ; i < NUM_LEDS; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;
    uint16_t h16_128 = hue16 >> 7;
    if ( h16_128 & 0x100) {
      hue8 = 255 - (h16_128 >> 1);
    } else {
      hue8 = h16_128 >> 1;
    }

    brightnesstheta16  += brightnessthetainc16;
    uint16_t b16 = sin16( brightnesstheta16  ) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);

    uint8_t index = hue8;
    //index = triwave8( index);
    index = scale8( index, 240);
    gCurrentPalette = rainbowsherbet_gp;
    CRGB newcolor = ColorFromPalette(gCurrentPalette, index, bri8);

    nblend(leds[i], newcolor, 128);
  }
}





