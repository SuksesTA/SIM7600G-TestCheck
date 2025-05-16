# GPS Tracker with SIM7600G and MQTT

This project demonstrates a GPS tracker using the SIM7600G module and MQTT for data transmission.

## Features

- **GPS Tracking:** Retrieves GPS coordinates using the SIM7600G module.
- **DHT22 Sensor:** Reads temperature and humidity data from a DHT22 sensor.
- **MPU6050 Sensor:** Reads Gyroscope and Accelerometer data from a MPU6050 sensor.
- **MQTT Communication:** Publishes GPS and sensor data to an MQTT broker.
- **OLED Display:** Displays GPS coordinates and sensor readings on an OLED screen.
- **Data Logging:** Saves GPS coordinates to a file for offline access.

## Hardware Requirements

- ESP32 board (e.g., ESP32-devkit-c)
- SIM7600G GSM/GPRS/GNSS module
- DHT22 temperature and humidity sensor
- MPU6050 gyroscope and accelerometer sensor
- OLED display (SSD1306)

## Software Requirements

- PlatformIO

## Setup

1. **Install Libraries:** Install the required libraries in the PlatformIO.
2. **Connect Hardware:** Connect the hardware components according to the wiring diagram.
3. **Configure Credentials:**
   - **MQTT Broker:** Replace `broker_ip` in `SIM7600G_code.cpp` with your MQTT broker's IP address.
   - **GPRS Credentials:** Replace `apn`, `gprsUser`, and `gprsPass` in `SIM7600G.h` with your GPRS network credentials.
4. **Compile and Upload:** Compile and upload the code to your ESP32 board.

## Usage

1. **Power On:** Power on the ESP32 board.
2. **Data Transmission:** The device will start collecting GPS and sensor data and publish it to the MQTT broker.
3. **Data Visualization:** Use an MQTT client (e.g., MQTT Explorer) to subscribe to the topic and visualize the data.

## Code Structure

- **main.cpp:** Main program file that handles the overall logic.
- **SIM7600G.h:** Header file for the SIM7600G module.
- **SIM7600G_code.cpp:** Implementation file for the SIM7600G module.
- **OLED/SSD1306.h:** Header file for the OLED display.
- **DHT/DHT22.h:** Header file for the DHT22 sensor.
- **MPU6050_code.cpp:** Implementation file for the MPU6050 module.
- **MPU6050.h:** Header file for the MPU6050 sensor
- **Data/DataHandler.h:** Header file for data handling functions.

## Customization

- **MQTT Topic:** Modify the `topic` variable in `SIM7600G_code.cpp` to change the MQTT topic for data publication.
- **Display:** Customize the OLED display content by modifying the `gpsDisplay()`,`dhtDisplay()` and `mpudisplay()` functions in `main.cpp`.

## Notes

- This project is a basic example and can be further extended with additional features.
- Ensure that your SIM card has a data plan and is activated for GPRS.
- The MQTT broker should be accessible from the SIM7600G module.
- The code is provided as-is and may require modifications for specific hardware configurations.
