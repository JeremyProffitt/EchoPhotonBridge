/*

This work is released under the following license:

Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International (CC BY-NC-SA 4.0)

https://build.particle.io/build/593076f5806fa665f4000e5e/tab/EchoPhotonBridge.cpp#verifyhttps://creativecommons.org/licenses/by-nc-sa/4.0/

You may not use this code for commercial purposes without the owner's permission.

Copyright Jeremy Proffitt <jeremy@iotpimp.com> 

For more information, visit http://www.iotpimp.com

Known issues:
  The temperature sometimes is off by a degree because of the F to C / C to F conversion
  V3 Multimedia extensions are currently untested and unsupported.

*/

#include <EchoPhotonBridge.h>
#include <math.h>
//public:

EchoPhotonBridge::EchoPhotonBridge() 
{
}

void _init_class() 
{
    _init_required = false;
    Particle.variable("echoDeviceV2", EchoPhotonBridge::echoDeviceConfigsV2);
    Particle.function("echoCmdV2",  &EchoPhotonBridge::callEchoFunctionV2, this);
    Particle.variable("echoDeviceV3", EchoPhotonBridge::echoDeviceConfigsV3);
    Particle.function("echoCmdV3",  &EchoPhotonBridge::callEchoFunctionV3, this);
}


int EchoPhotonBridge::callEchoFunctionV2(String value)
{
    if (_init_required) _init_class();
    int device = _stoi(_getValue(value, ':', 0));
    int function = _stoi(_getValue(value, ':', 1));
    String params = _getValue(value, ':', 2);
    
    int delta = 0;
    bool on = false;
    bool requestStatus = false;
    bool requestStatus2 = false;
    
    //Check if the device number is within range..
    if (device < 0 or device > _echoDeviceCountV2) {
        return -2;
    }
    
    switch (function)
    {
        case DEVICETYPEV2_ONOFF:
            if (_v2devices[device].fOnOff == NULL) return -1;
            on = (_stoi(params) > 0);
            delta = _v2devices[device].fOnOff (device, on, params);
            return 1000 + delta;
            
        case DEVICETYPEV2_PERCENT:
            if (_v2devices[device].fPercent == NULL) return -1;
            delta = _getDelta(params, _v2devices[device].percent);
            _v2devices[device].percent += delta;
            
            if (_v2devices[device].percent > 100) _v2devices[device].percent = 100;
            if (_v2devices[device].percent < 0) _v2devices[device].percent = 0;
            
            return _v2devices[device].fPercent (device, _v2devices[device].percent, delta, params);
            
        case DEVICETYPEV2_COLOR:
            if (_v2devices[device].fColor == NULL) return -1;
            _v2devices[device].rgb.red = _stoi(_getValue(params, ',', 0));
            _v2devices[device].rgb.green = _stoi(_getValue(params, ',', 1));
            _v2devices[device].rgb.blue = _stoi(_getValue(params, ',', 2));
            return _v2devices[device].fColor (device, _stoi(_getValue(params, ',', 0)), _stoi(_getValue(params, ',', 1)), _stoi(_getValue(params, ',', 2)), params);
            
        case DEVICETYPEV2_LOCK:
            if (_v2devices[device].fLock == NULL) return -1;
            requestStatus = (_stoi(_getValue(params, ',', 0)) > 0);
            on = _stoi(_getValue(params, ',', 1)) > 0;
            return _v2devices[device].fLock (device, requestStatus, on, params);
            
        case DEVICETYPEV2_TEMP:
            if (_v2devices[device].fTemp == NULL) return -1;
            requestStatus = (_stoi(_getValue(params, ',', 0)) == 1);
            requestStatus2 = (_stoi(_getValue(params, ',', 0)) == 2);
            delta = _getDelta(_getValue(params, ',', 1), _v2devices[device].temp);
            _v2devices[device].temp += delta;
            return _v2devices[device].fTemp (device, requestStatus, requestStatus2, _v2devices[device].temp, delta, params);
            
        case DEVICETYPEV2_LIGHTTEMP:
            if (_v2devices[device].fLightTemp == NULL) return -1;
            delta = _getDelta(_getValue(params, ',', 1), _v2devices[device].lightTemp);
            _v2devices[device].lightTemp += delta;
            return _v2devices[device].fLightTemp (device, _stoi(params), params);
            
        default:
            return 0;
    }
}

int EchoPhotonBridge::callEchoFunctionV3(String value)
{
    if (_init_required) _init_class();
    int device = _stoi(_getValue(value, ':', 0));
    int function = _stoi(_getValue(value, ':', 1));
    String params = _getValue(value, ':', 2);
    
    int delta = 0;
    bool on = false;
    bool muted = false;
    String data = "";
    int iValue = 0;
    
    //Check if the device number is within range..
    if (device < 0 or device > _echoDeviceCountV3) {
        return -2;
    }
    
    switch (function)
    {
        case DEVICETYPEV3_ONOFF:
            if (_v3devices[device].fOnOff == NULL) return -1;
            on = (_stoi(params) > 0);
            return _v3devices[device].fOnOff (device, on, params);
            
        case DEVICETYPEV3_VOLUMEPERCENT:
            if (_v3devices[device].fVolumePercent == NULL) return -1;
            muted = (_stoi(_getValue(params,',', 0))) == 1;
            delta = _getDelta(_getValue(params,',',1), _v3devices[device].volumePercent);
            _v3devices[device].volumePercent += delta;
            if (_v3devices[device].volumePercent > 100) _v3devices[device].volumePercent = 100;
            if (_v3devices[device].volumePercent < 0) _v3devices[device].volumePercent = 0;
            _v3devices[device].isMuted = muted;
            return _v3devices[device].fVolumePercent (device, muted, _v3devices[device].volumePercent, delta, params);
            
        case DEVICETYPEV3_VOLUMESTEP:
            if (_v3devices[device].fVolumeStep == NULL) return -1;
            muted = (_stoi(_getValue(params,',', 0))) == 1;
            delta = _getDelta(_getValue(params,',',1), _v3devices[device].volumeStep);
            _v3devices[device].volumeStep += delta;
            _v3devices[device].isMuted = muted;
            return _v3devices[device].fVolumeStep (device, muted, _v3devices[device].volumeStep, delta, params);
            
        case DEVICETYPEV3_CHANNEL:
            if (_v3devices[device].fChannel == NULL) return -1;
            delta = _getDelta(_getValue(params,',', 0), _v3devices[device].currentChannel);
            data = _getValue(params,',', 1);
            if (data.length() > 0) iValue = _getIndex(data, _v3devices[device].channels, ',');
            if (iValue > 0) 
            {
                _v3devices[device].currentChannel = iValue;
                delta = 0;
            }
            return _v3devices[device].fChannel (device, _v3devices[device].currentChannel, delta, data, params);
            
        case DEVICETYPEV3_MEDIACONTROL:
            if (_v3devices[device].fMediaControl == NULL) return -1;
            return _v3devices[device].fMediaControl (device, params);
            
        case DEVICETYPEV3_INPUTCONTROL:
            if (_v3devices[device].fInputControl == NULL) return -1;
            delta = _getDelta(_getValue(params,',', 0), _v3devices[device].currentInput);
            data = _getValue(params,',', 1);
            if (data.length() > 0) iValue = _getIndex(data, _v3devices[device].inputs, ',');
            if (iValue > 0) 
            {
                _v3devices[device].currentInput = iValue;
                delta = 0;
            }
            return _v3devices[device].fInputControl (device, _v3devices[device].currentInput, delta, data, params);
            
        default:
            return 0;
    }
}

void EchoPhotonBridge::addEchoDeviceV2(String deviceName, 
                    functionOnOff fOnOff,
                    functionPercent fPercent,
                    functionColor fColor,
                    functionLock fLock,
                    functionTemp fTemp,
                    functionLightTemp fLightTemp)
{
    if (_init_required) _init_class();
    //Check if we have enough devices available to use.
    if (_echoDeviceCountV2 > MAX_ECHO_DEVICESV2) return;
    int deviceIndex = _getDeviceIndexV2(deviceName);
    if (deviceIndex == -1) return;
    
    //Now store functions and device identifier
    if (fOnOff != NULL) _addEchoDeviceV2OnOff(deviceIndex, fOnOff); 
    if (fPercent != NULL) _addEchoDeviceV2Percent(deviceIndex, fPercent);
    if (fColor != NULL) _addEchoDeviceV2Color(deviceIndex, fColor);
    if (fLock != NULL) _addEchoDeviceV2Lock(deviceIndex, fLock);
    if (fTemp != NULL) _addEchoDeviceV2Temp(deviceIndex, fTemp);
    if (fLightTemp != NULL) _addEchoDeviceV2LightTemp(deviceIndex, fLightTemp);
    _updateDeviceConfigs();
}

void EchoPhotonBridge::addEchoDeviceV2OnOff(String deviceName, functionOnOff fOnOff)
{
    if (_init_required) _init_class();
    int deviceIndex = _getDeviceIndexV2(deviceName);
    if (deviceIndex == -1) return;
    _addEchoDeviceV2OnOff(deviceIndex, fOnOff);
    _updateDeviceConfigs();
}

void EchoPhotonBridge::addEchoDeviceV2Percent(String deviceName, functionPercent fPercent)
{
    if (_init_required) _init_class();
    int deviceIndex = _getDeviceIndexV2(deviceName);
    if (deviceIndex == -1) return;
    _addEchoDeviceV2Percent(deviceIndex, fPercent);
    _updateDeviceConfigs();
}

void EchoPhotonBridge::addEchoDeviceV2Color(String deviceName, functionColor fColor)
{
    if (_init_required) _init_class();
    int deviceIndex = _getDeviceIndexV2(deviceName);
    if (deviceIndex == -1) return;
    _addEchoDeviceV2Color(deviceIndex, fColor);
    _updateDeviceConfigs();
}

void EchoPhotonBridge::addEchoDeviceV2Lock(String deviceName, functionLock fLock)
{
    if (_init_required) _init_class();
    int deviceIndex = _getDeviceIndexV2(deviceName);
    if (deviceIndex == -1) return;
    _addEchoDeviceV2Lock(deviceIndex, fLock);
    _updateDeviceConfigs();
}

void EchoPhotonBridge::addEchoDeviceV2Temp(String deviceName, functionTemp fTemp)
{
    if (_init_required) _init_class();
    int deviceIndex = _getDeviceIndexV2(deviceName);
    if (deviceIndex == -1) return;
    _addEchoDeviceV2Temp(deviceIndex, fTemp);
    _updateDeviceConfigs();
}

void EchoPhotonBridge::addEchoDeviceV2LightTemp(String deviceName, functionLightTemp fLightTemp)
{
    if (_init_required) _init_class();
    int deviceIndex = _getDeviceIndexV2(deviceName);
    if (deviceIndex == -1) return;
    _addEchoDeviceV2LightTemp(deviceIndex, fLightTemp);
    _updateDeviceConfigs();
}

EchoPhotonBridge::colorInRGB EchoPhotonBridge::getLightColor(int deviceIndex) {
    if (_init_required) _init_class();
    return _v2devices[deviceIndex].rgb;
}



void EchoPhotonBridge::addEchoDeviceV3(String deviceName,
                    functionOnOff fOnOff,
                    functionVolumePercent fVolumePercent,
                    functionVolumeStep fVolumeStep,
                    functionChannel fChannel,
                    String commaSeperatedChannelList, 
                    functionMediaControl fMediaControl,
                    functionInputControl fInputControl,
                    String commaSeperatedInputList) 
{
    if (_init_required) _init_class();
     //Check if we have enough devices available to use.
    if (_echoDeviceCountV3 > MAX_ECHO_DEVICESV3) return;
    int deviceIndex = _getDeviceIndexV3(deviceName);
    if (deviceIndex == -1) return;
    
    //Now store functions and device identifier
    if (fOnOff != NULL) _addEchoDeviceV3OnOff(deviceIndex, fOnOff); 
    if (fVolumePercent != NULL) _addEchoDeviceV3VolumePercent(deviceIndex, fVolumePercent);
    if (fVolumeStep != NULL) _addEchoDeviceV3VolumeStep(deviceIndex, fVolumeStep);
    if (fChannel != NULL) _addEchoDeviceV3Channel(deviceIndex, commaSeperatedChannelList, fChannel);
    if (fMediaControl != NULL) _addEchoDeviceV3MediaControl(deviceIndex, fMediaControl);
    if (fInputControl != NULL) _addEchoDeviceV3InputControl(deviceIndex, commaSeperatedInputList, fInputControl);
    _updateDeviceConfigs();
}

void EchoPhotonBridge::addEchoDeviceV3OnOff(String deviceName, functionOnOff fOnOff)
{
    if (_init_required) _init_class();
    int deviceIndex = _getDeviceIndexV3(deviceName);
    if (deviceIndex == -1) return;
    _addEchoDeviceV3OnOff(deviceIndex, fOnOff);
    _updateDeviceConfigs();
}

void EchoPhotonBridge::addEchoDeviceV3VolumePercent(String deviceName, functionVolumePercent fVolumePercent)
{
    if (_init_required) _init_class();
    int deviceIndex = _getDeviceIndexV3(deviceName);
    if (deviceIndex == -1) return;
    _addEchoDeviceV3VolumePercent(deviceIndex, fVolumePercent);
    _updateDeviceConfigs();
}

void EchoPhotonBridge::addEchoDeviceV3VolumeStep(String deviceName, functionVolumeStep fVolumeStep)
{
    if (_init_required) _init_class();
    int deviceIndex = _getDeviceIndexV3(deviceName);
    if (deviceIndex == -1) return;
    _addEchoDeviceV3VolumeStep(deviceIndex, fVolumeStep);
    _updateDeviceConfigs();
}

void EchoPhotonBridge::addEchoDeviceV3Channel(String deviceName, String commaSeperatedChannelList, functionChannel fChannel)
{
    if (_init_required) _init_class();
    int deviceIndex = _getDeviceIndexV3(deviceName);
    if (deviceIndex == -1) return;
    _addEchoDeviceV3Channel(deviceIndex, commaSeperatedChannelList, fChannel);
    _updateDeviceConfigs();
}

void EchoPhotonBridge::addEchoDeviceV3MediaControl(String deviceName, functionMediaControl fMediaControl)
{
    if (_init_required) _init_class();
    int deviceIndex = _getDeviceIndexV3(deviceName);
    if (deviceIndex == -1) return;
    _addEchoDeviceV3MediaControl(deviceIndex, fMediaControl);
    _updateDeviceConfigs();
}

void EchoPhotonBridge::addEchoDeviceV3InputControl(String deviceName, String commaSeperatedInputList, functionInputControl fInputControl)
{
    if (_init_required) _init_class();
    int deviceIndex = _getDeviceIndexV3(deviceName);
    if (deviceIndex == -1) return;
    _addEchoDeviceV3InputControl(deviceIndex, commaSeperatedInputList, fInputControl);
    _updateDeviceConfigs();
}

EchoPhotonBridge::colorInRGB EchoPhotonBridge::kelvinToRGB(int tempInKelvin) {
    EchoPhotonBridge::colorInRGB answer;
    
    //Algorithm from http://www.tannerhelland.com/4435/convert-temperature-rgb-algorithm-code/
    if (_init_required) _init_class();
    int temp = tempInKelvin / 100;
    
    if (temp <= 66) {
        answer.red = 255;
        answer.blue = 255;
        answer.green = 99.4708025861 * log(answer.green) - 161.1195681661;
    }
    else 
    {
        answer.red = 329.698727446 * pow((temp - 60),-0.1332047592);
        answer.blue = 138.5177312231 * log(temp - 60) - 305.0447927307;
        answer.green = 288.1221695283 * pow((temp - 60),-0.0755148492);
    }
    
    answer.red = (answer.red < 0) ? 0 : answer.red;
    answer.red = (answer.red > 255) ? 255 : answer.red;
    
    answer.blue = (answer.blue < 0) ? 0 : answer.blue;
    answer.blue = (answer.blue > 255) ? 255 : answer.blue;
    
    answer.green = (answer.green < 0) ? 0 : answer.green;
    answer.green = (answer.green > 255) ? 255 : answer.green;
    
    return answer;
}


//private:


void EchoPhotonBridge::_addEchoDeviceV2OnOff(int deviceIndex, functionOnOff fOnOff)
{
    if (_init_required) _init_class();
    _v2devices[deviceIndex].fOnOff = fOnOff;
    _v2devices[deviceIndex].type = _setBit(_v2devices[deviceIndex].type,DEVICETYPEV2_ONOFF);
}

void EchoPhotonBridge::_addEchoDeviceV2Percent(int deviceIndex, functionPercent fPercent)
{
    if (_init_required) _init_class();
    _v2devices[deviceIndex].fPercent = fPercent;
    _v2devices[deviceIndex].type = _setBit(_v2devices[deviceIndex].type,DEVICETYPEV2_PERCENT);
}

void EchoPhotonBridge::_addEchoDeviceV2Color(int deviceIndex, functionColor fColor)
{
    if (_init_required) _init_class();
    _v2devices[deviceIndex].fColor = fColor;
    _v2devices[deviceIndex].type = _setBit(_v2devices[deviceIndex].type,DEVICETYPEV2_COLOR);
}

void EchoPhotonBridge::_addEchoDeviceV2Lock(int deviceIndex, functionLock fLock)
{
    if (_init_required) _init_class();
    _v2devices[deviceIndex].fLock = fLock;
    _v2devices[deviceIndex].type = _setBit(_v2devices[deviceIndex].type,DEVICETYPEV2_LOCK);
}

void EchoPhotonBridge::_addEchoDeviceV2Temp(int deviceIndex, functionTemp fTemp)
{
    if (_init_required) _init_class();
    _v2devices[deviceIndex].fTemp = fTemp;
    _v2devices[deviceIndex].type = _setBit(_v2devices[deviceIndex].type,DEVICETYPEV2_TEMP);
}

void EchoPhotonBridge::_addEchoDeviceV2LightTemp(int deviceIndex, functionLightTemp fLightTemp)
{
    if (_init_required) _init_class();
    _v2devices[deviceIndex].fLightTemp = fLightTemp;
    _v2devices[deviceIndex].type = _setBit(_v2devices[deviceIndex].type,DEVICETYPEV2_LIGHTTEMP);
}

void EchoPhotonBridge::_addEchoDeviceV3OnOff(int deviceIndex, functionOnOff fOnOff)
{
    if (_init_required) _init_class();
    _v3devices[deviceIndex].fOnOff = fOnOff;
    _v3devices[deviceIndex].type = _setBit(_v3devices[deviceIndex].type,DEVICETYPEV3_ONOFF);
}

void EchoPhotonBridge::_addEchoDeviceV3VolumePercent(int deviceIndex, functionVolumePercent fVolumePercent)
{
    if (_init_required) _init_class();
    _v3devices[deviceIndex].fVolumePercent = fVolumePercent;
   _v3devices[deviceIndex].type = _setBit(_v3devices[deviceIndex].type,DEVICETYPEV3_VOLUMEPERCENT);
}

void EchoPhotonBridge::_addEchoDeviceV3VolumeStep(int deviceIndex, functionVolumeStep fVolumeStep)
{
    if (_init_required) _init_class();
    _v3devices[deviceIndex].fVolumeStep = fVolumeStep;
    _v3devices[deviceIndex].type = _setBit(_v3devices[deviceIndex].type,DEVICETYPEV3_VOLUMESTEP);
}

void EchoPhotonBridge::_addEchoDeviceV3Channel(int deviceIndex, String commaSeperatedChannelList, functionChannel fChannel)
{
    if (_init_required) _init_class();
    _v3devices[deviceIndex].fChannel = fChannel;
    _v3devices[deviceIndex].type = _setBit(_v3devices[deviceIndex].type,DEVICETYPEV3_CHANNEL);
    //Turn the channel list into a json list.
    _v3devices[deviceIndex].channels = "\"" + commaSeperatedChannelList.replace(",","\",\"") + "\"";
}

void EchoPhotonBridge::_addEchoDeviceV3MediaControl(int deviceIndex, functionMediaControl fMediaControl)
{
    if (_init_required) _init_class();
    _v3devices[deviceIndex].fMediaControl = fMediaControl;
    _v3devices[deviceIndex].type = _setBit(_v3devices[deviceIndex].type,DEVICETYPEV3_MEDIACONTROL);
}

void EchoPhotonBridge::_addEchoDeviceV3InputControl(int deviceIndex, String commaSeperatedInputList, functionInputControl fInputControl)
{
    if (_init_required) _init_class();
    _v3devices[deviceIndex].fInputControl = fInputControl;
    _v3devices[deviceIndex].type = _setBit(_v3devices[deviceIndex].type,DEVICETYPEV3_INPUTCONTROL);
    //Turn the Input list into a json list.
    _v3devices[deviceIndex].inputs = "\"" + commaSeperatedInputList.replace(",","\",\"") + "\"";
}     


/***************************************************************************************
 * 
 *   Helper Functions
 * 
 * *************************************************************************************/

int EchoPhotonBridge::_getDeviceIndexV2(String deviceName)
{
    int i = 0;
    while ( i < _echoDeviceCountV2) {
        if (deviceName.toUpperCase() == _v2devices[i].name.toUpperCase()) {
            return i;
        }
        i++;
    }
    if (i > MAX_ECHO_DEVICESV2) { 
        return -1;      
    }
    i = _echoDeviceCountV2;
    _echoDeviceCountV2++;
    _v2devices[i].name = deviceName;
    return i;
}

int EchoPhotonBridge::_getDeviceIndexV3(String deviceName)
{
    int i = 0;
    while ( i < _echoDeviceCountV3) {
        if (deviceName.toUpperCase() == _v2devices[i].name.toUpperCase()) {
            return i;
        }
        i++;
    }
    if (i > MAX_ECHO_DEVICESV3) { 
        return -1;      
    }
    i = _echoDeviceCountV3;
    _echoDeviceCountV3++;
    _v2devices[i].name = deviceName;
    return i;
}

int EchoPhotonBridge::_stoi(String number) 
{
    char inputStr[64];
    number.toCharArray(inputStr,64);
    return atoi(inputStr);
}

int EchoPhotonBridge::_getDelta(String params, int currentValue) 
{
    int delta = 0;
    if (params.substring(0,1).toUpperCase() == "U") {
        params = params.substring(1,params.length());
        delta = _stoi(params);
    } else if (params.substring(0,1).toUpperCase() == "D") {
        params = params.substring(1,params.length());
        delta = -1 * _stoi(params);
    } else {
        delta = _stoi(params) - currentValue;
    }
    return delta;
}

String EchoPhotonBridge::_getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

int EchoPhotonBridge::_getIndex(String data, String csvData, char separator)
{
    int index = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = csvData.length() - 1;

    for (int i = 0; i <= maxIndex; i++) {
        if (csvData.charAt(i) == separator || i == maxIndex) {
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
            if (csvData.substring(strIndex[0], strIndex[1]).toUpperCase() == data.toUpperCase()) return index;
            index++;
        }
    }
    return -1;
}

int EchoPhotonBridge::_setBit(int input, int bitIndex)
{
    input ^= (-1 ^ input) & (1 << bitIndex);
    return input;
}

int EchoPhotonBridge::_clearBit(int input, int bitIndex)
{
    input ^= (0 ^ input) & (1 << bitIndex);
    return input;
}

void EchoPhotonBridge::_updateDeviceConfigs()
{
    echoDeviceConfigsV2 = "{\"Version\":2,\"Devices\":["; //denotes configuration type 01
    for(int i = 0; i < _echoDeviceCountV2; i++) 
    {
        echoDeviceConfigsV2 += "{\"name\":\"" + _v2devices[i].name + "\",\"type\":" + _v2devices[i].type + "}";
        if ( i + 1 < _echoDeviceCountV2) echoDeviceConfigsV2 += ",";
    }
    echoDeviceConfigsV2 += "]}";
    
    echoDeviceConfigsV3 = "{\"Version\":3,\"Devices\":["; //denotes configuration type 01
    for(int i = 0; i < _echoDeviceCountV3; i++) 
    {
        echoDeviceConfigsV3 += "{\"name\":\"" + _v3devices[i].name + "\",\"type\":" + _v3devices[i].name + "}";
        if (i + 1 < _echoDeviceCountV3) echoDeviceConfigsV3 += ",";
    }
    echoDeviceConfigsV3 += "]}";
}


