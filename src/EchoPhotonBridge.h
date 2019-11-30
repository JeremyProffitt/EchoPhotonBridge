/*

This work is released under the following license:

Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International (CC BY-NC-SA 4.0)

https://creativecommons.org/licenses/by-nc-sa/4.0/

You may not use this code for commercial purposes without the owner's permission.

Copyright Jeremy Proffitt <jeremy@iotpimp.com> 

For more information, visit http://www.iotpimp.com

For known issues and notes, see the cpp file.

*/

#ifndef __INC_EchoPhotonBridge_H
#define __INC_EchoPhotonBridge_H

#ifndef MAX_ECHO_DEVICESV2
#define MAX_ECHO_DEVICESV2 20
#endif

#ifndef MAX_ECHO_DEVICESV3
#define MAX_ECHO_DEVICESV3 5
#endif

#define DEVICETYPEV2_ONOFF 1
#define DEVICETYPEV2_PERCENT 2
#define DEVICETYPEV2_COLOR 3
#define DEVICETYPEV2_LOCK 4
#define DEVICETYPEV2_TEMP 5
#define DEVICETYPEV2_LIGHTTEMP 6

#define DEVICETYPEV3_ONOFF 1
#define DEVICETYPEV3_VOLUMEPERCENT 2
#define DEVICETYPEV3_VOLUMESTEP 3
#define DEVICETYPEV3_CHANNEL 4
#define DEVICETYPEV3_MEDIACONTROL 5
#define DEVICETYPEV3_INPUTCONTROL 6

#include "Particle.h"

class EchoPhotonBridge
{

public:
    //V2 functions.
    typedef int (*functionOnOff)(int, bool, String);
    typedef int (*functionPercent)(int, int, int, String);
    typedef int (*functionColor)(int, int, int, int, String);
    typedef int (*functionLock)(int, bool, bool, String);
    typedef int (*functionTemp)(int, bool, bool, int, int, String);
    typedef int (*functionLightTemp)(int, int, String);

    EchoPhotonBridge();

    //V2 devices
    void addEchoDeviceV2(String deviceName,
                         functionOnOff fOnOff,
                         functionPercent fPercent,
                         functionColor fColor,
                         functionLock fLock,
                         functionTemp fTemp,
                         functionLightTemp fLightTemp);

    void addEchoDeviceV2OnOff(String deviceName, functionOnOff fOnOff);
    void addEchoDeviceV2Percent(String deviceName, functionPercent fPercent);
    void addEchoDeviceV2Color(String deviceName, functionColor fColor);
    void addEchoDeviceV2Lock(String deviceName, functionLock fLock);
    void addEchoDeviceV2Temp(String deviceName, functionTemp fTemp);
    void addEchoDeviceV2LightTemp(String deviceName, functionLightTemp fLightTemp);

    //V3 functions.
    typedef int (*functionVolumePercent)(int, bool, int, int, String);
    typedef int (*functionVolumeStep)(int, bool, int, int, String);
    typedef int (*functionChannel)(int, int, int, String, String);
    typedef int (*functionMediaControl)(int, String);
    typedef int (*functionInputControl)(int, int, int, String, String);

    //V3 Devices
    void addEchoDeviceV3(String deviceName,
                         functionOnOff fOnOff,
                         functionVolumePercent fVolumePercent,
                         functionVolumeStep fVolumeStep,
                         functionChannel fChannel,
                         String commaSeperatedChannelList,
                         functionMediaControl fMediaControl,
                         functionInputControl fInputControl,
                         String commaSeperatedInputList);

    void addEchoDeviceV3OnOff(String deviceName, functionOnOff fOnOff);
    void addEchoDeviceV3VolumePercent(String deviceName, functionVolumePercent fVolumePercent);
    void addEchoDeviceV3VolumeStep(String deviceName, functionVolumeStep fVolumeStep);
    void addEchoDeviceV3Channel(String deviceName, String commaSeperatedChannelList, functionChannel fChannel);
    void addEchoDeviceV3MediaControl(String deviceName, functionMediaControl fMediaControl);
    void addEchoDeviceV3InputControl(String deviceName, String commaSeperatedInputList, functionInputControl fInputControl);

    //Interfaces for Echo Commands
    int callEchoFunctionV2(String value);
    int callEchoFunctionV3(String value);
    //_echoDevices holds the configuration string
    String echoDeviceConfigsV2 = "";
    String echoDeviceConfigsV3 = "";

    //Kelvin to RGB Conversion
    struct colorInRGB
    {
        int red;
        int green;
        int blue;
    };

    colorInRGB kelvinToRGB(int);
    colorInRGB getLightColor(int deviceIndex);

    void init();

private:
    bool _init_required = true;

    //V2 Add Devices
    void _addEchoDeviceV2OnOff(int deviceIndex, functionOnOff fOnOff);
    void _addEchoDeviceV2Percent(int deviceIndex, functionPercent fPercent);
    void _addEchoDeviceV2Color(int deviceIndex, functionColor fColor);
    void _addEchoDeviceV2Lock(int deviceIndex, functionLock fLock);
    void _addEchoDeviceV2Temp(int deviceIndex, functionTemp fTemp);
    void _addEchoDeviceV2LightTemp(int deviceIndex, functionLightTemp fLightTemp);

    //V2 Function Arrays
    struct deviceV2
    {
        //Device Name and Type (type is an integer here but think of it as 16 booleans because we are using bit manipulation)
        String name;
        int type;

        //Device Functions
        functionOnOff fOnOff;
        functionPercent fPercent;
        functionColor fColor;
        functionLock fLock;
        functionTemp fTemp;
        functionLightTemp fLightTemp;

        //Current Device Settings, Temp, percent, Color
        int percent;
        int temp;
        int lightTemp;
        colorInRGB rgb = {.red = 255, .green = 255, .blue = 255};
    };

    deviceV2 _v2devices[MAX_ECHO_DEVICESV2];

    //V3 Add Devices
    void _addEchoDeviceV3OnOff(int deviceIndex, functionOnOff fOnOff);
    void _addEchoDeviceV3VolumePercent(int deviceIndex, functionVolumePercent fVolumePercent);
    void _addEchoDeviceV3VolumeStep(int deviceIndex, functionVolumeStep fVolumeStep);
    void _addEchoDeviceV3Channel(int deviceIndex, String commaSeperatedChannelList, functionChannel fChannel);
    void _addEchoDeviceV3MediaControl(int deviceIndex, functionMediaControl fMediaControl);
    void _addEchoDeviceV3InputControl(int deviceIndex, String commaSeperatedInputList, functionInputControl fInputControl);

    //V3 Function Arrays
    struct deviceV3
    {
        //Device Name and Type (type is an integer here but think of it as 16 booleans because we are using bit manipulation)
        String name;
        int type;

        //V3 Function Arrays
        functionOnOff fOnOff;
        functionVolumePercent fVolumePercent;
        functionVolumeStep fVolumeStep;
        functionChannel fChannel;
        functionMediaControl fMediaControl;
        functionInputControl fInputControl;

        //Strings for holding CSV versions of Channel and Input arrays.
        String channels;
        int currentChannel;
        String inputs;
        int currentInput;
        bool isMuted;
        int volumePercent;
        int volumeStep;
    };

    deviceV3 _v3devices[MAX_ECHO_DEVICESV3];

    //Utility and Helper Functions
    int _stoi(String number);
    int _getDelta(String params, int currentValue);
    String _getValue(String data, char separator, int index);
    int _getIndex(String data, String csvData, char separator);
    int _setBit(int input, int bitIndex);
    int _clearBit(int input, int bitIndex);

    //Utility function to get a device index based on name
    int _getDeviceIndexV2(String deviceName);
    int _getDeviceIndexV3(String deviceName);

    //update the _econDeviceConfigs String
    void _updateDeviceConfigs();

    //Keep track of how many devices we have.
    int _echoDeviceCountV2 = 0;
    int _echoDeviceCountV3 = 0;
};
#endif