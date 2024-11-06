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


/// Sensor specific code /////////////////////////////////////////////////////////////

#include <Wire.h>
#include <SensirionI2cScd30.h> // Sensirion I2C SCD30 https://github.com/Sensirion/arduino-i2c-scd30
SensirionI2cScd30 sdc30;

uint16_t atmosphericCO2 = 419; // environmental CO2 ppm for calibration
uint16_t measurementInterval = 2; // s
uint16_t siteAltitude = 450; // m
uint16_t temperatureCompensation = 350; // °C*100

// Sensor data is: CO2, T, rH
const uint8_t valueCount = 3;

// Local data variable
float_t data[valueCount];


/// MVP3000 Framwork and Modules /////////////////////////////////////////////////
#include <MVP3000.h>
extern MVP3000 mvp;

// State machine
enum OperatingState: uint8_t {
    ERROR = 0,
    MEASURE = 1,
    CALIBRATE = 2,
} operatingState;
uint8_t calibrationCounter = 0;


/// Sensor module ////////////////////////////////////////////////////////////////
#include <XmoduleSensor/XmoduleSensor.h>

// Initialize the sensor module with the number of values
XmoduleSensor xmoduleSensor(valueCount);

// Add a description of the sensor for the web interface
String infoName = "Sensirion SDC30";
String infoDescription = "Sensirion SDC30 ambient CO2 sensor module, measures also temperature and relative humidity.";
String sensorTypes[valueCount] = {"CO2", "T", "rH"};
String sensorUnits[valueCount] = {"ppm", "0.1 &deg;C", "0.1 &percnt;"};

// Conversion of native sensor units to the desired integer units
//  ppm
//  °C -> 0.1 °C
//  % -> 0.1 %
int8_t exponent[valueCount] = {0, 1, 1};


/// LED module ////////////////////////////////////////////////////////////////
#include <XmoduleLED/XmoduleLED.h>

const uint8_t ledPin = D8; // NeoPixels pin
const uint8_t ledCount = 12; // Number of NeoPixels

// Initialize the LED module with the number of LED on the strip
XmoduleLED xmoduleLED(ledPin, ledCount);

// Standard colors for the LED smileys and bars
uint32_t standardColors[ledCount] = {
    // LED smileys: red, yellow, green
    ColorRGB(255, 0, 0),
    ColorRGB(255, 0, 0),
    ColorRGB(255, 255, 0),
    ColorRGB(255, 255, 0),
    ColorRGB(0, 255, 0),
    ColorRGB(0, 255, 0),
    // LED bars: red, yellow, green
    ColorRGB(0, 255, 0),
    ColorRGB(0, 255, 0),
    ColorRGB(255, 255, 0),
    ColorRGB(255, 255, 0),
    ColorRGB(255, 0, 0),
    ColorRGB(255, 0, 0)
};

// Flag for very high CO2 concentrations
bool isBlinking = false;


void setup() {

    /// MVP3000 Framwork and Modules /////////////////////////////////////////////////

    // Init the sensor module and add it to the mvp framework
    xmoduleSensor.setSensorInfo(infoName, infoDescription, sensorTypes, sensorUnits);
    xmoduleSensor.setSampleAveraging(1); // The SCD30 has already averaged data ready every 1.5 s. Averaging in the framework can be set to a low value.
    xmoduleSensor.setReportingThreshold(5, 0); // Recording threshold change of 0.5 percent of the CO2 value
    xmoduleSensor.setSampleToIntExponent(exponent);
    xmoduleSensor.setNetworkCtrlCallback(networkCtrlCallback); // Add a callback for network control commands / calibration 
    mvp.addXmodule(&xmoduleSensor);

    // Init the led module and add it to the mvp framework
    xmoduleLED.adaptiveGlobalBrightness(A0); // Use a potentiometer on A0 to adjust the global brightness
    xmoduleLED.setFixedColorIndividual(standardColors);
    mvp.addXmodule(&xmoduleLED);

    // Set the CO2-Ampel landing page, mvp settings are automatically moved
    mvp.setAlternateRoot(altResponseFiller);

    // Disable MQTT and UDP discovery
    mvp.mqttHardDisable();
    mvp.udpHardDisable();

    // Start mvp framework
    mvp.setup();


    /// Sensor specific code /////////////////////////////////////////////////////////////

    // I2C
    Wire.begin();

    // Init SCD30 sensor
    sdc30.begin(Wire, SCD30_I2C_ADDR_61);
    
    // Sometimes the sensor does not come up, maybe only after flashing.
    int16_t sdcError = sdc30.softReset(); // Blocking 2000 ms
    if (sdcError != NO_ERROR) {
        operatingState = OperatingState::ERROR;
        // Indicate error by red LED
        xmoduleLED.setFixedColorSync(ColorRGB(255, 0, 0));
        static char errorMessage[128];
        errorToString(sdcError, errorMessage, sizeof errorMessage);
        mvp.logFormatted("Error starting sensor: %s", errorMessage);
        return;
    }

    // Set sensor settings. Those are saved by the SDC30 and possibly different depending on previous uploaded firmware.
    operatingState = OperatingState::MEASURE;
    sdc30.setMeasurementInterval(measurementInterval);
    sdc30.setAltitudeCompensation(siteAltitude);
    sdc30.setTemperatureOffset(temperatureCompensation);

    // Get and print SDC30 sensor information
    uint8_t major = 0;
    uint8_t minor = 0;
    sdc30.readFirmwareVersion(major, minor);
    uint16_t interval;
    sdc30.getMeasurementInterval(interval);
    uint16_t altitude;
    sdc30.getAltitudeCompensation(altitude);
    uint16_t temperatureOffset;
    sdc30.getTemperatureOffset(temperatureOffset);
    // uint16_t co2RefConcentration;
    // sdc30.getForceRecalibrationStatus(co2RefConcentration); // This always returns 400 after softReset/power cycle. However, the sensor still uses the updated calibration curve.
    uint16_t isActive;
    sdc30.getAutoCalibrationStatus(isActive);
    mvp.logFormatted("SDC30: Firmware version %d.%d, interval %d s, altitude %d m, temp offset %d C, auto calibration %s", major, minor, interval, altitude, temperatureOffset, (isActive) ? "active" : "disabled");

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

    // Check if there is data available 
    uint16_t sdcDataReady;
    sdc30.getDataReady(sdcDataReady);
    if (sdcDataReady) {

        // Check if the data can be read
        if (sdc30.readMeasurementData(data[0], data[1], data[2]) == NO_ERROR) {

            // Action depending on the operating state
            if (operatingState == OperatingState::MEASURE) {
                // Check if data makes sense
                // The first CO2 measurement is always 0, probably some running median thing
                if (data[0] > atmosphericCO2 - 100) {
                    // Add data to the sensor module
                    xmoduleSensor.addSample(data);

                    // Set LED brightness: the color is fixed, LED are just switched on/off using brightness
                    if (data[0] < 1500) {
                        // For small concentrations set the LED brightness according to the CO2 value
                        isBlinking = false;
                        uint8_t brightness[ledCount] = {0};
                        selectLedBrightness(brightness);
                        xmoduleLED.setFixedBrightnessIndividual(brightness);
                    } else {
                        // For very high concentrations start the blinking-brightness-effect (once)
                        if (!isBlinking) {
                            isBlinking = true;
                            xmoduleLED.setBrightnessEffect(1000, XledFx::BRIGHTNESSFX::PULSE_FULL);
                        }
                    }
                }
            }

            // Action depending on the operating state
            if (operatingState == OperatingState::CALIBRATE) {
                // The sensor needs 1-2 measurement cycles to update the calibration curve
                if (calibrationCounter++ > 2) {
                    operatingState = OperatingState::MEASURE;
                    // Restore default LED behavior
                    xmoduleLED.setFixedBrightnessSync(255);
                    xmoduleLED.setFixedColorIndividual(standardColors);
                }
            }

        } else {
            mvp.log("Error trying to execute readMeasurementData()");
        }
    }
}


/// Webpage ////////////////////////////////////////////////////////////////
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

/// WebSocket callback /////////////////////////////////////////////////////////////
void networkCtrlCallback(const String& data) {
    if (data.equals("CALIBRATE")) {
        calibrateSensor();
    } else {
        mvp.log("Unknown network control command.");
    }
}

/// Calibration ////////////////////////////////////////////////////////////////
void calibrateSensor() {
    // Data sheet states to operate the sensor at least 2 minutes on fresh air. Could be more with housing.
    if (millis() < 3*60*1000) {
        mvp.log("Calibration not started. The device needs to be OPERATING at FRESH air for AT LEAST 3 minutes before calibration!");
        return;
    }

    // Start calibration
    int16_t sdcError = sdc30.forceRecalibration(atmosphericCO2); // Blocking 10 ms
    if (sdcError != NO_ERROR) {
        // Fail
        operatingState = OperatingState::ERROR;
        // Indicate error by red LED
        xmoduleLED.setFixedColorSync(ColorRGB(255, 0, 0));
        static char errorMessage[128];
        errorToString(sdcError, errorMessage, sizeof errorMessage);
        mvp.logFormatted("Error during calibration: %s", errorMessage);
    } else {
        // Calibration started, the sensor needs 1-2 measurement cycles to update the calibration curve
        operatingState = OperatingState::CALIBRATE;
        calibrationCounter = 0;
        // Indicate calibration by pulsing the LED blue
        xmoduleLED.setFixedColorSync(ColorRGB(0, 0, 255));
        xmoduleLED.setBrightnessEffect(1000, XledFx::BRIGHTNESSFX::PULSE_FULL);
        mvp.logFormatted("Calibration to %d ppm initiated. Restarting measurement ...", atmosphericCO2);
    }
}


/// Select LED brightness depending on CO2 concentration /////////////////////////////////////////////////////////////
void selectLedBrightness(uint8_t* brightness) {
    // LED bars, light all below value
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

    // LED smileys, light selectively
    switch ((uint16_t)data[0]) {
        case 1500 ... 65535: // r + blink
        case 1150 ... 1499: // r
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
