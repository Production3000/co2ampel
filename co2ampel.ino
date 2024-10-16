/*
Copyright Production 3000

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/


#include <MVP3000.h>
extern MVP3000 mvp;

// Sensirion I2C SCD30 https://github.com/Sensirion/arduino-i2c-scd30
// This is a very nice library that returns actually usefull error codes, which we mostly ignore here.
#include <SensirionI2cScd30.h>
#include <Wire.h>
SensirionI2cScd30 sdc30;

uint16_t atmosphericCO2 = 419; // ppm
uint16_t siteAltitude = 450; // m
uint16_t temperatureCompensation = 350; // °C*100

enum OperatingState: uint8_t {
    ERROR = 0,
    MEASURE = 1,
    CALIBRATE = 2,
} operatingState;
uint8_t calibrationCounter = 0;


const uint8_t valueCount = 3;

// Add a description of the sensor for the web interface
String infoName = "Sensirion SDC30";
String infoDescription = "Sensirion SDC30 ambient CO2 sensor module, measures also temperature and relative humidity.";
String sensorTypes[valueCount] = {"CO2", "T", "rH"};
String sensorUnits[valueCount] = {"ppm", "0.1 &deg;C", "0.1 &percnt;"};

// Local data variable
float_t data[valueCount];

// Convert native sensor units to the desired units
//  ppm
//  °C -> 0.1 °C
//  % -> 0.1 %
int8_t exponent[valueCount] = {0, 1, 1};

// Init sensor module
XmoduleSensor xmoduleSensor(valueCount);


#include <Adafruit_NeoPixel.h>
// NeoPixels
#define LED_PIN D8      // NeoPixels pin
#define NUMPIXELS 12    // Number of NeoPixels


#include <XmoduleLED.h>
XmoduleLED xmoduleLED(LED_PIN, NUMPIXELS);


uint32_t standardColors[NUMPIXELS] = {
    // LED smileys
    Adafruit_NeoPixel::Color(255, 0, 0),
    Adafruit_NeoPixel::Color(255, 0, 0),
    Adafruit_NeoPixel::Color(255, 255, 0),
    Adafruit_NeoPixel::Color(255, 255, 0),
    Adafruit_NeoPixel::Color(0, 255, 0),
    Adafruit_NeoPixel::Color(0, 255, 0),
    // LED bars
    Adafruit_NeoPixel::Color(0, 255, 0),
    Adafruit_NeoPixel::Color(0, 255, 0),
    Adafruit_NeoPixel::Color(255, 255, 0),
    Adafruit_NeoPixel::Color(255, 255, 0),
    Adafruit_NeoPixel::Color(255, 0, 0),
    Adafruit_NeoPixel::Color(255, 0, 0)
};



void setup() {
    // Init the sensor module and add it to the mvp framework
    xmoduleSensor.setSensorInfo(infoName, infoDescription, sensorTypes, sensorUnits);
    xmoduleSensor.setSampleAveraging(1); // The SCD30 has already averaged data ready every 1.5 s. Averaging in the framework can be set to a low value.
    xmoduleSensor.setSampleToIntExponent(exponent);
    xmoduleSensor.setNetworkCtrlCallback(networkCtrlCallback);
    mvp.addXmodule(&xmoduleSensor);

    // Add the custom module to the mvp framework
    mvp.addXmodule(&xmoduleLED);
    xmoduleLED.adaptiveGlobalBrightness(A0);
    xmoduleLED.setFixedColorIndividual(standardColors);

    // CO2-Ampel landing page
    mvp.setAlternateRoot(altResponseFiller);
    mvp.mqttHardDisable();
    mvp.udpHardDisable();

    // Start mvp framework
    mvp.setup();

    // I2C
    Wire.begin();

    // Init SCD30 sensor
    sdc30.begin(Wire, SCD30_I2C_ADDR_61);
    // sdc30.stopPeriodicMeasurement();
    int16_t sdcError = sdc30.softReset(); // Blocking 2000 ms
    if (sdcError != NO_ERROR) {
        operatingState = OperatingState::ERROR;
        xmoduleLED.setFixedColorSync(Adafruit_NeoPixel::Color(255, 0, 0));
        static char errorMessage[128];
        errorToString(sdcError, errorMessage, sizeof errorMessage);
        mvp.logFormatted("Error starting sensor: %s", errorMessage);
        return;
    }

    operatingState = OperatingState::MEASURE;
    sdc30.setTemperatureOffset(temperatureCompensation);

    // Get and print sensor information
    uint8_t major = 0;
    uint8_t minor = 0;
    sdc30.readFirmwareVersion(major, minor);
    uint16_t altitude;
    sdc30.getAltitudeCompensation(altitude);
    uint16_t temperatureOffset;
    sdc30.getTemperatureOffset(temperatureOffset);
    // uint16_t co2RefConcentration;
    // sdc30.getForceRecalibrationStatus(co2RefConcentration); // This always returns 400 after softReset/power cycle. However, the sensor still uses the updated calibration curve.
    uint16_t isActive;
    sdc30.getAutoCalibrationStatus(isActive);
    mvp.logFormatted("SDC30: Firmware version %d.%d, altitude %d m, temp offset %d C, auto calibration %s", major, minor, altitude, temperatureOffset, (isActive) ? "active" : "disabled");

    // Start periodic measurement
    // This is ambiguous in the library: "Setting the ambient pressure will overwrite previous settings of altitude compensation. Setting the argument to zero will deactivate the ambient pressure compensation."
    // Does this mean setting to zero will result in compensation for the current altitude? One can assume so, but just set the altitude again to make sure ...
    sdc30.startPeriodicMeasurement(0);
    sdc30.setAltitudeCompensation(siteAltitude);
}

void loop() {
    // Do the work
    mvp.loop();

    if (operatingState == OperatingState::ERROR) {
        return;
    }

    uint16_t sdcDataReady;
    sdc30.getDataReady(sdcDataReady);
    if (sdcDataReady) {
        if (sdc30.readMeasurementData(data[0], data[1], data[2]) == NO_ERROR) {
            if (operatingState == OperatingState::MEASURE) {
                // Add data to the sensor module
                // The first CO2 measurement is always 0, probably some running median thing
                if (data[0] > atmosphericCO2 - 100) { 
                    xmoduleSensor.addSample(data);
                    uint8_t brightness[NUMPIXELS] = {0};
                    onDemandSetter(brightness);
                    xmoduleLED.setFixedBrightnessIndividual(brightness);

                    // Blink faster if CO2 is high
                    // uint16_t interval = constrain(2300 - (uint16_t)data[0], 100, 2300);

                    // TODO blink effect - needs to turn off again if CO2 goes below threshold
                    // Blink faster if CO2 is high
                    // xmoduleLED.setBrightnessEffect(constrain(2300 - (uint16_t)data[0], 100, 2300), XledFx::BRIGHTNESSFX::BLINK);

                }
            }
            if (operatingState == OperatingState::CALIBRATE) {
                // The sensor needs 1-3 measurement cycles to update the calibration curve
                if (calibrationCounter++ > 2) {
                    operatingState = OperatingState::MEASURE;
                    xmoduleLED.setFixedBrightnessSync(255);
                    xmoduleLED.setFixedColorIndividual(standardColors);
                }
            }
        } else {
            mvp.log("Error trying to execute readMeasurementData()");
        }
    }
}


#include "webpage.h"
size_t altResponseFiller(uint8_t *buffer, size_t maxLen, size_t index) {
    // Chunked response filler for the html template
    size_t len = strlen_P(altHtml);
    if (index + maxLen > len) {
        maxLen = len - index;
    }
    memcpy_P(buffer, altHtml + index, maxLen);
    return maxLen;
}

void networkCtrlCallback(const String& data) {
    if (data.equals("CALIBRATE")) {
        calibrateSensor();
    } else {
        mvp.log("Unknown network control command.");
    }
}


void calibrateSensor() {
    // Data sheet states 2 minutes for the sensor. Could be more with housing.
    if (millis() < 5*60*1000) {
        mvp.log("Calibration not started. The device needs to be OPERATING at FRESH air for AT LEAST 5 minutes before calibration!");
        return;
    }

    // Set calibrating step

    int16_t sdcError = sdc30.forceRecalibration(atmosphericCO2); // Blocking 10 ms
    if (sdcError != NO_ERROR) {
        operatingState = OperatingState::ERROR;
        xmoduleLED.setFixedColorSync(Adafruit_NeoPixel::Color(255, 0, 0));
        static char errorMessage[128];
        errorToString(sdcError, errorMessage, sizeof errorMessage);
        mvp.logFormatted("Error during calibration: %s", errorMessage);
    } else {
        operatingState = OperatingState::CALIBRATE;
    calibrationCounter = 0;
        xmoduleLED.setFixedColorSync(Adafruit_NeoPixel::Color(0, 0, 255));
        xmoduleLED.setBrightnessEffect(1000, XledFx::BRIGHTNESSFX::PULSE_FULL);
        mvp.logFormatted("Calibration to %d ppm initiated. Restarting measurement ...", atmosphericCO2);
    }
}


void onDemandSetter(uint8_t* brightness) {
    // LED bars
    switch ((uint16_t)data[0]) {
        case 1150 ... 65535: // ggyyrr
            brightness[11] = 255;
        case 1000 ... 1149: // ggyyr
            brightness[10] = 255;
        case 850 ... 999: // ggyy
            brightness[9] = 255;
        case 700 ... 849: // ggy
            brightness[8] = 255;
        case 550 ... 699: // gg
            brightness[7] = 255;
        case 0 ... 549: // g
            brightness[6] = 255;
        default:
            break;
    }

    // LED smileys
    switch ((uint16_t)data[0]) {
        case 1300 ... 65535: // r + blink
        case 1150 ... 1299: // r
        case 1000 ... 1149:
            brightness[0] = 255;
            brightness[1] = 255;
            break;
        case 850 ... 999: // y
        case 700 ... 849:
            brightness[2] = 255;
            brightness[3] = 255;
            break;
        case 550 ... 699: // g
        case 0 ... 549:
            brightness[4] = 255;
            brightness[5] = 255;
        default:
            break;
    }
}