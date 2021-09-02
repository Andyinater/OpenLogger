
// Required libraries
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <Wire.h>
#include <HardwareSerial.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>



// declare the BNO055 object
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);

// declare the GPSSerial on hardware serial 1
HardwareSerial GPSSerial(1); //bn220 serial object

//BNO055 Sample Rate
uint16_t BNO055_SAMPLERATE_DELAY_MS = 10;

// Runtime Variables
String logName;
boolean gpsLock = false;
const byte ledPin = 27;
unsigned long ledFlashLength = 200;
unsigned long touchFlashLength = 50;
unsigned long lastFlash;
boolean ledOn = false;
boolean pinTouch = false;
unsigned long lastTouch;

void setup() {
  Serial.begin(115200);
  delay(10000);
  // LED pin
  pinMode(ledPin, OUTPUT);

  // pin 16 is rx, pin 17 is tx, on the esp. feed gps in appropriately (38 pin esp)
  GPSSerial.begin(115200, SERIAL_8N1, 16, 17);
  
  // init the mpu
//  Serial.begin(115200);
  Serial.println("Orientation Sensor Test"); Serial.println("");

  /* Initialise the sensor */
  if (!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }


  // initialize the micro SD card on standard MISO/MOSI
  if(!SD.begin()){
    Serial.println("Card Mount Failed");
      return;
  }
  uint8_t cardType = SD.cardType();

  if(cardType == CARD_NONE){
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if(cardType == CARD_MMC){
      Serial.println("MMC");
  } else if(cardType == CARD_SD){
      Serial.println("SDSC");
  } else if(cardType == CARD_SDHC){
      Serial.println("SDHC");
  } else {
      Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);


//   initialize log file, to be parameterized
  String curNum = "";
  String temp;
  File root = SD.open("/");
  int highestLog = 1;
  while (true) {
    File entry = root.openNextFile();
    Serial.print("huh");Serial.println(entry.name());
    if (! entry) {
      // no more files
      if (highestLog > curNum.toInt()) {
        temp = "/log"+String(highestLog)+".txt";
        Serial.print(temp);
        writeFile(SD,temp.c_str(),"rx,ry,rz,ax,ay,az,longDir,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15");
      } else {
        temp = "/log"+curNum+".txt";
        Serial.print(temp);
        writeFile(SD,temp.c_str(),"rx,ry,rz,ax,ay,az,longDir,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15");
      }
      break;
    }
    if (entry.isDirectory()) {
      continue;
    } else {
      String curName = entry.name();
      Serial.print(curName);
      if (curName.indexOf("log") != 0){
        int ind1 = curName.indexOf("g") + 1;
        curNum = "";
        for (int i = ind1; i < curName.indexOf("."); i++){
          curNum += curName[i];
        }
        Serial.print("curNum:");Serial.println(curNum);
        Serial.println(curNum.toInt());
        highestLog += 1;
        continue;
      }
    }
  }
  logName = temp;
  Serial.print(logName);
  delay(1000);
//  writeFile(SD,"/log1.txt","ax,ay,az");
}

void loop() {
  // Check if being touched
  if (pinTouch == false && touchRead(15) < 10){
    lastTouch = millis();
    pinTouch = true;
    lastFlash = millis();
  }
  if (pinTouch){
    if(ledOn && millis() - lastFlash > touchFlashLength){
      ledOn = false;
      lastFlash = millis();
      digitalWrite(ledPin, LOW);
    } else if (ledOn == false && millis() - lastFlash > touchFlashLength){
      lastFlash = millis();
      ledOn = true;
      digitalWrite(ledPin, HIGH);
    }
    if (touchRead(15) > 50){
      pinTouch = false;
      lastTouch = millis();
    }
  }

  // collect mpu data
  //could add VECTOR_ACCELEROMETER, VECTOR_MAGNETOMETER,VECTOR_GRAVITY...
  sensors_event_t orientationData , angVelocityData , linearAccelData, magnetometerData, accelerometerData, gravityData;
  bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
  bno.getEvent(&linearAccelData, Adafruit_BNO055::VECTOR_LINEARACCEL);
  String BNO_DATA = "";
  BNO_DATA += getEventData(&orientationData);
  BNO_DATA += ",";
  BNO_DATA += getEventData(&linearAccelData);
  
  // write mpu data
  writeAccelData(SD,logName.c_str(),BNO_DATA);

  // set gps flags and variables
  boolean GpsDataReady = false;
  String gpsBuff = "";
  int commaCount = 0;

  // while there are bytes to read
  while(GPSSerial.available() > 0){
    // add bytes to data line
    char dLine = char(GPSSerial.read());
    gpsBuff.concat(dLine);
    if(dLine == ','){
      commaCount ++;
    } else if(commaCount == 18) { // if at the commaCount for the first time
      if(dLine == '0'){
        gpsLock = false; 
      } else {
        gpsLock = true;
      }
      commaCount = 1000; // set comma count way out, only check first char after 18th comma
    }

    // set data ready flag
    GpsDataReady = true;
  }

  // if there is gps data to read
  if(GpsDataReady){

    // reset the flag
    GpsDataReady = false;

    // write the data
    writeGpsData(SD,logName.c_str(),gpsBuff);
  }

  // do LED logic
 if(gpsLock){
  if (pinTouch == false) {
    digitalWrite(ledPin, HIGH); 
  }
 } else{
  if (pinTouch == false) {
    digitalWrite(ledPin, LOW);
  }
 }

 if(millis() - lastFlash >= ledFlashLength && pinTouch == false){
  if(ledOn){
    ledOn = false;
    digitalWrite(ledPin, LOW);
  } else{
    ledOn = true;
    digitalWrite(ledPin,HIGH);
  }
  lastFlash = millis();
 }
 
}



//==================== FUNCTIONS ===========================//

String getEventData(sensors_event_t* event) {
  double x = -1000000, y = -1000000 , z = -1000000; //dumb values, easy to spot problem
  if (event->type == SENSOR_TYPE_ACCELEROMETER) {
//    Serial.print("Accl:");
    x = event->acceleration.x;
    y = event->acceleration.y;
    z = event->acceleration.z;
  }
  else if (event->type == SENSOR_TYPE_ORIENTATION) {
//    Serial.print("Orient:");
    x = event->orientation.x;
    y = event->orientation.y;
    z = event->orientation.z;
  }
  else if (event->type == SENSOR_TYPE_MAGNETIC_FIELD) {
//    Serial.print("Mag:");
    x = event->magnetic.x;
    y = event->magnetic.y;
    z = event->magnetic.z;
  }
  else if (event->type == SENSOR_TYPE_GYROSCOPE) {
//    Serial.print("Gyro:");
    x = event->gyro.x;
    y = event->gyro.y;
    z = event->gyro.z;
  }
  else if (event->type == SENSOR_TYPE_ROTATION_VECTOR) {
//    Serial.print("Rot:");
    x = event->gyro.x;
    y = event->gyro.y;
    z = event->gyro.z;
  }
  else if (event->type == SENSOR_TYPE_LINEAR_ACCELERATION) {
//    Serial.print("Linear:");
    x = event->acceleration.x;
    y = event->acceleration.y;
    z = event->acceleration.z;
  }
  else {
    Serial.print("Unk:");
  }

  return(String(String(x)+","+String(y)+","+String(z)));
}




void writeGpsData(fs::FS &fs, const char * path, String gData){
  File file = fs.open(path, FILE_APPEND);

  if(!file){
    Serial.println("Failed to open file for appending GPS");
    return;
  }
  file.print(gData);
  file.close();
}

void writeAccelData(fs::FS &fs, const char * path, String ACCELDATA){
  File file = fs.open(path, FILE_APPEND);

  if(!file){
    Serial.println("Failed to open file for appending ACCEL");
    return;
  }
  file.println(ACCELDATA);
  file.close();
}


void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void createDir(fs::FS &fs, const char * path){
    Serial.printf("Creating Dir: %s\n", path);
    if(fs.mkdir(path)){
        Serial.println("Dir created");
    } else {
        Serial.println("mkdir failed");
    }
}

void removeDir(fs::FS &fs, const char * path){
    Serial.printf("Removing Dir: %s\n", path);
    if(fs.rmdir(path)){
        Serial.println("Dir removed");
    } else {
        Serial.println("rmdir failed");
    }
}

void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    if(file.println(message)){
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
    file.close();
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("File renamed");
    } else {
        Serial.println("Rename failed");
    }
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\n", path);
    if(fs.remove(path)){
        Serial.println("File deleted");
    } else {
        Serial.println("Delete failed");
    }
}

void testFileIO(fs::FS &fs, const char * path){
    File file = fs.open(path);
    static uint8_t buf[512];
    size_t len = 0;
    uint32_t start = millis();
    uint32_t end = start;
    if(file){
        len = file.size();
        size_t flen = len;
        start = millis();
        while(len){
            size_t toRead = len;
            if(toRead > 512){
                toRead = 512;
            }
            file.read(buf, toRead);
            len -= toRead;
        }
        end = millis() - start;
        Serial.printf("%u bytes read for %u ms\n", flen, end);
        file.close();
    } else {
        Serial.println("Failed to open file for reading");
    }


    file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }

    size_t i;
    start = millis();
    for(i=0; i<2048; i++){
        file.write(buf, 512);
    }
    end = millis() - start;
    Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
    file.close();
}
