# MVP3000 Application Note: CO2-Lights (CO2-Ampel)

The CO2-Lights serves as a showcase application that highlights the versatility of the [MVP3000](https://github.com/Production3000/mvp3000esp) framework. This implementation uses the sensor module and the LED module. It features a custom landing page accessible via any smartphone or browser. The page provides real-time ambient CO2 concentration data alongside a historical graph, which updates automatically through WebSocket.

See also [here](https://co2ampel.production3000.com/) for additional information and images.


## Hardware

The original CO2-Ampel was developed by purecrea GmbH and is available via [Bastelgarage.ch](https://www.bastelgarage.ch/co2-ampel-mit-smileys-und-balkenanzeige) and [co2ampel.ch](https://www.co2ampel.ch). The [official firmware](https://github.com/bastelgarage/co2ampel) is open source as well.

Specifications:
 *  ESP8266 micro controller
 *  [Sensirion SDC30](https://sensirion.com/products/catalog/SCD30) CO2 sensor, additionally measuring temperature and relative humidity. See also the reference [implementation](https://github.com/Sensirion/arduino-i2c-scd30).
 *  WS2812B strip with 12 LED
 *  Photo resistor as ambient light sensor
 *  Push button


## Implementation

The implementation consists of two main components: the main script and a web display for the end-user. The [MVP3000](https://github.com/Production3000/mvp3000esp) framework manages essential tasks such as data processing, connectivity, and communication.

### Main Script

The main script on the ESP8266 delivers core functionality. Please refer to the comments in the [code](/co2ampel.ino) for detailed explanations.

 *  Initialize the MVP3000 framework and its modules.
 *  Interface with the CO2 sensor, conduct measurements, and pass the collected data to the sensor module of the framework.
 *  Provide a callback to the sensor module of the framework to handle sensor calibration upon user request (via landing page). 
 *  Define CO2 concentration thresholds and set the LED colors using the LED module of the framework.


### Web Display

The web display for end-users, hosted on the ESP8266, provides an intuitive landing page that displays current CO2 concentration and a recent data graph, with live updates via WebSocket. 

The HTML, CSS, JavaScript, and image files are stored in the [/landing_page](/landing_page/) folder for quick design development. The JavaScript classes *MicroPlot3000* and *WebSocket3000* are tools provided by the [MVP3000](https://github.com/Production3000/mvp3000esp) framework. The Python script *webpagebuilder.py*, also part of the framework, can be used to combine all files into a single string and automatically insert it into the ESP8266 code.

    python3 webpagebuilder.py

To access the landing page browse to the IP address of the ESP8266. Make sure to be connected to the same network as the ESP8266, either its own access point or if credentials are given the shared network.

    http://[deviceIp] 

All MVP3000 settings are moved to a sub-URL.

    http://[deviceIp]/mvp3000


## License

Licensed under the Apache License. See `LICENSE` for more information.

Copyright Production 3000
