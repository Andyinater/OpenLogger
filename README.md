# OpenLogger
An open source, esp32 datalogger for vehicles. This project is still in it's infancy, but my intentions are for using this device as a platform for development of more sophisticated and customizable data analysis tools and driver aids.

Currently, "BasicLogger_9250-bn220" is the most up to date code for the esp32. This is the proof of concept, which successfully reads sensors and writes data to a text file for later analysis.

# How to setup

## Parts list
- **ESP32 microcontroller:** The 38 pin breakout was used and will be how connections are described.
- **BN220 GPS module:** Beitian BN220 or equivalent is used. Serial utput has been set for the gps serial connection via a serial-to-usb cable and UBlox software. This will be covered in detail later
- **BNO055 IMU:** Or equivalent motion sensor, however examples in this repo only exist for the MPU9250 and the BNO055 (current). Communication is done via IIC.
- **Micro-SD Expansion board:** A standard breakout board was used. Specifically, an arduino board, so 5V is applied to the Vin to prevent dropout.
- **3D printed housing:** In the "3D Files" folder, an stl for a housing I designed that is compatible with a widely-available go-pro clip mount is available. Any other custom mounting solution will do, as long as considerations specified in the readme for that folder are followed.


## Connections

### BNO055
Vin - ESP_5V

gnd - GND

sda - ESP_GPIO_33

scl - ESP_GPIO_32


### BN220
Vin - ESP_5V

rx  - ESP_GPIO17

tx  - ESP_GPIO16

gnd - GND

### MicroSD
gnd - GND

vcc - 5V

Miso- ESP_GPIO19

Mosi- ESP_GPIO23

Sck - ESP_GPIO18

CS  - ESP_GPIO5


# ROADMAP


# TO-DO

## Physical
- model an appropriate housing to allow for more real-world testing
- print out and ensure fitment with gopro mounting solution


## Software

### Python
- refactor existing visualization code to make gradient polylines much more simple to create
- develop more standard visualizations
- update the file ingestion to handle the mixed data - perhaps just another column in the datafram to say GPS or ACCEL, to later groupby
- update the logic on detecting if datapoints are consequetive during the gradient logic - with 50 color levels in current data set, there is some issue causing loss of polylines for segments of ouellette
- work on popups and on-clicks. devise a system for highlighting or indicating start and end-points to open  further analysis, showing accelerometer values or more etc..


### Arduino
- Create a calibration firmware which can be used with the device on the dash of the desired car, with the car being on as close to level ground as possible. The calibration values should then be written into the esp32 EEPROM such that the esp32, mpu9250, and specific vehicle mounting solution are tied together. If the device is never removed, calibration will never need to be redone.
- Optimize the file writing. The file should remaine open within the loop, with the data writing functions not requiring the opening of the file. This should increase the logging rate (only necessary for accelerometer), as well as lower any latency. However, file corruption risks must be mitigated. Currently, the gps is logged at its true 18hz rate, the accelerometer fits 4 readings between each gps, so ~72hz, much lower than true. 
