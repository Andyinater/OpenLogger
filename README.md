# OpenLogger
An open source, esp32 datalogger for vehicles. This project is still in it's infancy, but my intentions are for using this device as a platform for development of more sophisticated and customizable data analysis tools and driver aids.

Currently, "BasicLogger_9250-bn220" is the most up to date code for the esp32. This is the proof of concept, which successfully reads sensors and writes data to a text file for later analysis.

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
- update the logic on detecting if datapoints are consequetive during the gradient logic - 

### Arduino
- Create a calibration firmware which can be used with the device on the dash of the desired car, with the car being on as close to level ground as possible. The calibration values should then be written into the esp32 EEPROM such that the esp32, mpu9250, and specific vehicle mounting solution are tied together. If the device is never removed, calibration will never need to be redone.
- Optimize the file writing. The file should remaine open within the loop, with the data writing functions not requiring the opening of the file. This should increase the logging rate (only necessary for accelerometer), as well as lower any latency. However, file corruption risks must be mitigated. Currently, the gps is logged at its true 18hz rate, the accelerometer fits 4 readings between each gps, so ~72hz, much lower than true. 
