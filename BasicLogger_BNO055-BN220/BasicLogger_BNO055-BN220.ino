
// Required libraries
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <Wire.h>
#include <HardwareSerial.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>
#include <EEPROM.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <WiFiAP.h>

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Arduino_JSON.h>
#include "SPIFFS.h"


// declare web server variables
const char* ssid = "OpenLogger";
const char* password = "1234567890";
const char* header = "<!DOCTYPE html>"
                     "<html>"
                     "<head>"
                     "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
                     "<style>"
                     "  h1 {"
                     "    border-bottom: 1px solid #c0c0c0;"
                     "    margin-bottom: 10px;"
                     "    padding-bottom: 10px;"
                     "    white-space: nowrap;"
                     "  }"
                     "  table {"
                     "    border-collapse: collapse;"
                     "  }"
                     "  th {"
                     "    cursor: pointer;"
                     "  }"
                     "  td.detailsColumn {"
                     "    -webkit-padding-start: 2em;"
                     "    text-align: end;"
                     "    white-space: nowrap;"
                     "  }"
                     "  a.icon {"
                     "    -webkit-padding-start: 1.5em;"
                     "    text-decoration: none;"
                     "  }"
                     "  a.icon:hover {"
                     "    text-decoration: underline;"
                     "  }"
                     "  a.file {"
                     "    background : url(\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAIAAACQkWg2AAAABnRSTlMAAAAAAABupgeRAAABHUlEQVR42o2RMW7DIBiF3498iHRJD5JKHurL+CRVBp+i2T16tTynF2gO0KSb5ZrBBl4HHDBuK/WXACH4eO9/CAAAbdvijzLGNE1TVZXfZuHg6XCAQESAZXbOKaXO57eiKG6ft9PrKQIkCQqFoIiQFBGlFIB5nvM8t9aOX2Nd18oDzjnPgCDpn/BH4zh2XZdlWVmWiUK4IgCBoFMUz9eP6zRN75cLgEQhcmTQIbl72O0f9865qLAAsURAAgKBJKEtgLXWvyjLuFsThCSstb8rBCaAQhDYWgIZ7myM+TUBjDHrHlZcbMYYk34cN0YSLcgS+wL0fe9TXDMbY33fR2AYBvyQ8L0Gk8MwREBrTfKe4TpTzwhArXWi8HI84h/1DfwI5mhxJamFAAAAAElFTkSuQmCC \") left top no-repeat;"
                     "  }"
                     "  a.dir {"
                     "    background : url(\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAGXRFWHRTb2Z0d2FyZQBBZG9iZSBJbWFnZVJlYWR5ccllPAAAAd5JREFUeNqMU79rFUEQ/vbuodFEEkzAImBpkUabFP4ldpaJhZXYm/RiZWsv/hkWFglBUyTIgyAIIfgIRjHv3r39MePM7N3LcbxAFvZ2b2bn22/mm3XMjF+HL3YW7q28YSIw8mBKoBihhhgCsoORot9d3/ywg3YowMXwNde/PzGnk2vn6PitrT+/PGeNaecg4+qNY3D43vy16A5wDDd4Aqg/ngmrjl/GoN0U5V1QquHQG3q+TPDVhVwyBffcmQGJmSVfyZk7R3SngI4JKfwDJ2+05zIg8gbiereTZRHhJ5KCMOwDFLjhoBTn2g0ghagfKeIYJDPFyibJVBtTREwq60SpYvh5++PpwatHsxSm9QRLSQpEVSd7/TYJUb49TX7gztpjjEffnoVw66+Ytovs14Yp7HaKmUXeX9rKUoMoLNW3srqI5fWn8JejrVkK0QcrkFLOgS39yoKUQe292WJ1guUHG8K2o8K00oO1BTvXoW4yasclUTgZYJY9aFNfAThX5CZRmczAV52oAPoupHhWRIUUAOoyUIlYVaAa/VbLbyiZUiyFbjQFNwiZQSGl4IDy9sO5Wrty0QLKhdZPxmgGcDo8ejn+c/6eiK9poz15Kw7Dr/vN/z6W7q++091/AQYA5mZ8GYJ9K0AAAAAASUVORK5CYII= \") left top no-repeat;"
                     "  }"
                     "  a.up {"
                     "    background : url(\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAGXRFWHRTb2Z0d2FyZQBBZG9iZSBJbWFnZVJlYWR5ccllPAAAAmlJREFUeNpsU0toU0EUPfPysx/tTxuDH9SCWhUDooIbd7oRUUTMouqi2iIoCO6lceHWhegy4EJFinWjrlQUpVm0IIoFpVDEIthm0dpikpf3ZuZ6Z94nrXhhMjM3c8895977BBHB2PznK8WPtDgyWH5q77cPH8PpdXuhpQT4ifR9u5sfJb1bmw6VivahATDrxcRZ2njfoaMv+2j7mLDn93MPiNRMvGbL18L9IpF8h9/TN+EYkMffSiOXJ5+hkD+PdqcLpICWHOHc2CC+LEyA/K+cKQMnlQHJX8wqYG3MAJy88Wa4OLDvEqAEOpJd0LxHIMdHBziowSwVlF8D6QaicK01krw/JynwcKoEwZczewroTvZirlKJs5CqQ5CG8pb57FnJUA0LYCXMX5fibd+p8LWDDemcPZbzQyjvH+Ki1TlIciElA7ghwLKV4kRZstt2sANWRjYTAGzuP2hXZFpJ/GsxgGJ0ox1aoFWsDXyyxqCs26+ydmagFN/rRjymJ1898bzGzmQE0HCZpmk5A0RFIv8Pn0WYPsiu6t/Rsj6PauVTwffTSzGAGZhUG2F06hEc9ibS7OPMNp6ErYFlKavo7MkhmTqCxZ/jwzGA9Hx82H2BZSw1NTN9Gx8ycHkajU/7M+jInsDC7DiaEmo1bNl1AMr9ASFgqVu9MCTIzoGUimXVAnnaN0PdBBDCCYbEtMk6wkpQwIG0sn0PQIUF4GsTwLSIFKNqF6DVrQq+IWVrQDxAYQC/1SsYOI4pOxKZrfifiUSbDUisif7XlpGIPufXd/uvdvZm760M0no1FZcnrzUdjw7au3vu/BVgAFLXeuTxhTXVAAAAAElFTkSuQmCC \") left top no-repeat;"
                     "  }"
                     "  html[dir=rtl] a {"
                     "    background-position-x: right;"
                     "  }"
                     "  #parentDirLinkBox {"
                     "    margin-bottom: 10px;"
                     "    padding-bottom: 10px;"
                     "  }"
                     "  #listingParsingErrorBox {"
                     "    border: 1px solid black;"
                     "    background: #fae691;"
                     "    padding: 10px;"
                     "    display: none;"
                     "  }"
                     "</style>"
                     "<title id=\"title\">Web FileBrowser</title>"
                     "</head>"
                     "<body>";

const char* footer =  "<hr>"
                      "<br>Made with &hearts; and &#9749; by ripper121<br>"
                      "<hr>"
                      "</body>"
                      "</html>";

const char* script = "<script>function sortTable(l){var e=document.getElementById(\"theader\"),n=e.cells[l].dataset.order||\"1\",s=0-(n=parseInt(n,10));e.cells[l].dataset.order=s;var t,a=document.getElementById(\"tbody\"),r=a.rows,d=[];for(t=0;t<r.length;t++)d.push(r[t]);for(d.sort(function(e,t){var a=e.cells[l].dataset.value,r=t.cells[l].dataset.value;return l?(a=parseInt(a,10),(r=parseInt(r,10))<a?s:a<r?n:0):r<a?s:a<r?n:0}),t=0;t<d.length;t++)a.appendChild(d[t])}</script>";

String uploadPath = "";
WebServer server(88);

AsyncWebServer server2(80);
AsyncEventSource events("/events");
JSONVar readings;

// Timer variables
unsigned long lastTime = 0;  
unsigned long lastTimeTemperature = 0;
unsigned long lastTimeAcc = 0;
unsigned long gyroDelay = 10;
unsigned long temperatureDelay = 1000;
unsigned long accelerometerDelay = 100;


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
unsigned long ledFlashLength = 300;
unsigned long touchFlashLength = 50;
unsigned long serverFlashLength = 85;
unsigned long server2FlashLength = 135;
unsigned long lastFlash;
boolean ledOn = false;
boolean pinTouch = false;
unsigned long lastTouch;
unsigned long longPress = 1000;
int deviceMode = 0; //0=logging, 1=webserver,files, 2=webserver,visualization
float gyroX, gyroY, gyroZ;
float accX, accY, accZ;

void setup() {
  Serial.begin(115200);
  delay(10000);
  // LED pin
  pinMode(ledPin, OUTPUT);
  EEPROM.begin(1000);

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

  // bno055 check for calibration data
  int eeAddress = 0;
  long bnoID;
  bool foundCalib = false;

  EEPROM.get(eeAddress, bnoID);

  adafruit_bno055_offsets_t calibrationData;
  sensor_t sensor;

  bno.getSensor(&sensor);
  if (bnoID != sensor.sensor_id || touchRead(15) < 10){
    Serial.println("\nNo Calibration Data for this sensor exists in EEPROM");
    delay(500);
  }
  else{
    Serial.println("\nFound Calibration for this sensor in EEPROM.");
    eeAddress += sizeof(long);
    EEPROM.get(eeAddress, calibrationData);

    displaySensorOffsets(calibrationData);

    Serial.println("\n\nRestoring Calibration data to the BNO055...");
    bno.setSensorOffsets(calibrationData);

    Serial.println("\n\nCalibration data loaded into BNO055");
    foundCalib = true;
  }

  delay(1000);

  sensors_event_t event;
  bno.getEvent(&event);
  /* always recal the mag as It goes out of calibration very often */
  if (foundCalib){
    Serial.println("Move sensor slightly to calibrate magnetometers");
    while (!bno.isFullyCalibrated())
      {
      bno.getEvent(&event);
      delay(BNO055_SAMPLERATE_DELAY_MS);
      }
  }
  else{
    Serial.println("Please Calibrate Sensor: ");
    while (!bno.isFullyCalibrated())
    {
      bno.getEvent(&event);

      Serial.print("X: ");
      Serial.print(event.orientation.x, 4);
      Serial.print("\tY: ");
      Serial.print(event.orientation.y, 4);
      Serial.print("\tZ: ");
      Serial.print(event.orientation.z, 4);

      /* Optional: Display calibration status */
      displayCalStatus();

      /* New line for the next sample */
      Serial.println("");

      /* Wait the specified delay before requesting new data */
      delay(BNO055_SAMPLERATE_DELAY_MS);
    }
  }
  Serial.println("\nFully calibrated!");
  Serial.println("--------------------------------");
  Serial.println("Calibration Results: ");
  adafruit_bno055_offsets_t newCalib;
  bno.getSensorOffsets(newCalib);
  displaySensorOffsets(newCalib);

  Serial.println("\n\nStoring calibration data to EEPROM...");

  eeAddress = 0;
  bno.getSensor(&sensor);
  bnoID = sensor.sensor_id;

  EEPROM.put(eeAddress, bnoID);
  EEPROM.commit();

  eeAddress += sizeof(bnoID);
  EEPROM.put(eeAddress, newCalib);
  EEPROM.commit();
  Serial.println("Data stored to EEPROM.");

  Serial.println("\n--------------------------------\n");
  delay(500);

  
  delay(1000);
//  writeFile(SD,"/log1.txt","ax,ay,az");
  

}

void loop() {


  if (deviceMode == 0){
    // Check if being touched
    if (pinTouch == false && touchRead(15) < 10){
      lastTouch = millis();
      pinTouch = true;
      lastFlash = millis();
    }
    if (pinTouch){
      if(millis() - lastTouch > longPress){
        deviceMode = 1;
        lastTouch = millis();

        // Start server with onetime run code
//      WiFi.softAP(ssid,password);
//      Serial.println();

        WiFi.mode(WIFI_OFF);
        WiFi.mode(WIFI_STA);
        delay(1000);
        WiFi.begin("HomeWifi","windsorontario");
        while (WiFi.status() != WL_CONNECTED) {
          Serial.print(".");
          delay(1000);
        }
        Serial.println("");
        Serial.println(WiFi.localIP());
        
        server.on("/", []() {
          handleRoot();
        });
      
        server.onNotFound(handleRoot);
      
        server.on("/fupload",  HTTP_POST, []() {
          server.send(200);
        }, handleFileUpload);
      
        server.on("/deleteConfirm", deleteConfirm);
        server.on("/doDelete", doDelete);
        server.on("/mkdir", doMkdir);
        server.begin();
        Serial.println("HTTP server started");
      
        Serial.println();
      
        Serial.println("Initialization done.");
        pinTouch = false;
        lastTouch = millis();
        return;
      }
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
  
   if(millis() - lastFlash >= ledFlashLength && pinTouch == false && gpsLock == false){
    if(ledOn){
      ledOn = false;
      digitalWrite(ledPin, LOW);
    } else{
      ledOn = true;
      digitalWrite(ledPin,HIGH);
    }
    lastFlash = millis();
   }
    //displayCalStatus();
    //Serial.println("");
  
   
  } 
  else if (deviceMode == 1){
    // Server Control
    server.handleClient();
    
    // Blink Control
    Serial.println("WEBSERVER,FILES");
    if (ledOn){
      if (millis() - lastFlash > serverFlashLength){
        digitalWrite(ledPin,LOW);
        lastFlash = millis();
        ledOn = false;
      }
    } else {
      if (millis() - lastFlash > serverFlashLength){
        digitalWrite(ledPin,HIGH);
        lastFlash = millis();
        ledOn = true;
      }
    }

    // Touch Control
    if (pinTouch == false && touchRead(15) < 10){
      lastTouch = millis();
      pinTouch = true;
    }
    if (pinTouch){
      if(millis() - lastTouch > longPress){
        deviceMode = 2;
        

        // Setup Visualization Server
        initSPIFFS();

        WiFi.mode(WIFI_OFF);
        WiFi.mode(WIFI_STA);
        WiFi.begin("HomeWifi","windsorontario");
        while (WiFi.status() != WL_CONNECTED) {
          Serial.print(".");
          delay(1000);
        }
        Serial.println("");
        Serial.println(WiFi.localIP());


        // Handle Web Server
        server2.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
          request->send(SPIFFS, "/index.html", "text/html");
        });
      
        server2.serveStatic("/", SPIFFS, "/");
      
        server2.on("/reset", HTTP_GET, [](AsyncWebServerRequest *request){
          gyroX=0;
          gyroY=0;
          gyroZ=0;
          request->send(200, "text/plain", "OK");
        });
      
        server2.on("/resetX", HTTP_GET, [](AsyncWebServerRequest *request){
          gyroX=0;
          request->send(200, "text/plain", "OK");
        });
      
        server2.on("/resetY", HTTP_GET, [](AsyncWebServerRequest *request){
          gyroY=0;
          request->send(200, "text/plain", "OK");
        });
      
        server2.on("/resetZ", HTTP_GET, [](AsyncWebServerRequest *request){
          gyroZ=0;
          request->send(200, "text/plain", "OK");
        });
      
        // Handle Web Server Events
        events.onConnect([](AsyncEventSourceClient *client){
          if(client->lastId()){
            Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
          }
          // send event with message "hello!", id current millis
          // and set reconnect delay to 1 second
          client->send("hello!", NULL, millis(), 10000);
        });
        server2.addHandler(&events);
      
        server2.begin();

        

        lastTouch = millis();
        return;
      }
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


    
  }
  else if (deviceMode == 2){
    // Blink Control
    Serial.println("WEBSERVER,VISUALIZATION");
    if (ledOn){
      if (millis() - lastFlash > server2FlashLength){
        digitalWrite(ledPin,LOW);
        lastFlash = millis();
        ledOn = false;
      }
    } else {
      if (millis() - lastFlash > server2FlashLength){
        digitalWrite(ledPin,HIGH);
        lastFlash = millis();
        ledOn = true;
      }
    }

    // Touch Control
    if (pinTouch == false && touchRead(15) < 10){
      lastTouch = millis();
      pinTouch = true;
    }
    if (pinTouch){
      if(millis() - lastTouch > longPress){
        deviceMode = 0;
        lastTouch = millis();

        // Disable server with one-time run code
        WiFi.mode(WIFI_OFF);
        return;
      }
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


    // Visualization Control
    sensors_event_t orientationData , angVelocityData , linearAccelData, magnetometerData, accelerometerData, gravityData;
    bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
    bno.getEvent(&linearAccelData, Adafruit_BNO055::VECTOR_LINEARACCEL);
    
    
    if ((millis() - lastTime) > gyroDelay) {
      // Send Events to the Web Server with the Sensor Readings
//      events.send(getBNOReadings(&orientationData).c_str(),"gyro_readings_euler",millis());
      events.send(getBNOQuats().c_str(),"gyro_readings_quat",millis());
      lastTime = millis();
    }
    if ((millis() - lastTimeAcc) > accelerometerDelay) {
      // Send Events to the Web Server with the Sensor Readings
      events.send(getBNOReadings(&linearAccelData).c_str(),"accelerometer_readings",millis());
      lastTimeAcc = millis();
    }
    if ((millis() - lastTimeTemperature) > temperatureDelay) {
      // Send Events to the Web Server with the Sensor Readings
      events.send(getTemperature().c_str(),"temperature_reading",millis());
      lastTimeTemperature = millis();
    }

    
        
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



// Web Functions


String getContentType(String filename) {
  filename.toUpperCase();
  if (server.hasArg("download")) return "application/octet-stream";
  else if (filename.endsWith(".HTM")) return "text/html";
  else if (filename.endsWith(".HTML")) return "text/html";
  else if (filename.endsWith(".CSS")) return "text/css";
  else if (filename.endsWith(".JS")) return "application/javascript";
  else if (filename.endsWith(".PNG")) return "image/png";
  else if (filename.endsWith(".GIF")) return "image/gif";
  else if (filename.endsWith(".JPG")) return "image/jpeg";
  else if (filename.endsWith(".ICO")) return "image/x-icon";
  else if (filename.endsWith(".XML")) return "text/xml";
  else if (filename.endsWith(".PDF")) return "application/x-pdf";
  else if (filename.endsWith(".ZIP")) return "application/x-zip";
  else if (filename.endsWith(".GZ")) return "application/x-gzip";
  return "text/plain";
}

bool handleFileRead(String path) {
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.htm";
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  if (SD.exists(pathWithGz) || SD.exists(path)) {
    if (SD.exists(pathWithGz))
      path += ".gz";
    File file = SD.open(path, "r");
    size_t sent = server.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

void handleRoot() {

  String directory = urldecode(server.uri());
  uploadPath = directory;
  File dir = SD.open(directory);


  String entryName = "";
  String tree = "";
  bool emptyFolder = true;
  while (true) {
    File entry =  dir.openNextFile();
    entryName = entry.name();
    entryName.replace(directory + "/", "");

    if (! entry) {
      // no more files
      break;
    }

    if (entry.isDirectory()) {
      tree += F("<tr>");
      tree += F("<td data-value=\"");
      tree += entryName;
      tree += F("/\"><a class=\"icon dir\" href=\"");
      tree += entry.name();
      tree += F("\">");
      tree += entryName;
      tree += F("/</a></td>");
      tree += F("<td class=\"detailsColumn\" data-value=\"0\">-</td>");
      tree += F("<td class=\"detailsColumn\" data-value=\"0\">");
      tree += F("<button class='buttons' onclick=\"location.href='/deleteConfirm?folder=");
      tree += entry.name();
      tree += F("';\">Delete</button></td>");
      tree += F("</tr>");
      emptyFolder = false;
    } else {
      tree += F("<tr>");
      tree += F("<td data-value=\"");
      tree += entry.name();
      tree += F("\"><a class=\"icon file\" draggable=\"true\" href=\"");
      tree += entry.name();
      tree +=  F("\">");
      tree += entryName;
      tree += F("</a></td>");
      tree += F("<td class=\"detailsColumn\" data-value=\")");
      tree += file_size(entry.size());
      tree += F("\">");
      tree += file_size(entry.size());
      tree += F("</td>");
      tree += F("<td class=\"detailsColumn\" data-value=\"0\">");
      tree += F("<button class='buttons' onclick=\"location.href='/deleteConfirm?file=");
      tree += entry.name();
      tree += F("';\">Delete</button></td>");
      tree += F("</tr>");
      emptyFolder = false;
    }
    entry.close();
  }

  int i, count;
  for (i = 0, count = 0; directory[i]; i++)
    count += (directory[i] == '/');
  count++;

  int parserCnt = 0;
  int rFromIndex = 0, rToIndex = -1;
  String lastElement = "";
  String tempElement = "";
  String path = directory;
  path.remove(0, 1);
  path += "/";
  while (count >= parserCnt) {
    rFromIndex = rToIndex + 1;
    rToIndex = path.indexOf('/', rFromIndex);
    if (count == parserCnt) {
      if (rToIndex == 0 || rToIndex == -1) break;
      tempElement = lastElement;
      lastElement = path.substring(rFromIndex, rToIndex);
    } else parserCnt++;
  }
  /*
    Serial.print("directory:");
    Serial.println(directory);
    Serial.print("path:");
    Serial.println(path);
    Serial.print("lastElement:");
    Serial.println(lastElement);
  */
  String webpage = "";
  webpage += F(header);
  webpage += F("<h1 id=\"header\">Index of ");
  webpage += directory;
  webpage += F("</h1>");

  if (directory != "/") {
    webpage += F("<div id=\"parentDirLinkBox\" style=\"display:block;\">");
    webpage += F("<a id=\"parentDirLink\" class=\"icon up\" href=\"");
    directory.replace(lastElement, "");
    if (directory.length() > 1)
      directory = directory.substring(0, directory.length() - 1);
    webpage += directory;
    webpage += F("\">");
    webpage += F("<span id=\"parentDirText\">[Parent directory]</span>");
    webpage += F("</a>");
    webpage += F("</div>");
  }

  webpage += F(script);

  webpage += F("<table>");
  webpage += F("<thead>");
  webpage += F("<tr class=\"header\" id=\"theader\">");
  webpage += F("<th onclick=\"sortTable(0);\">Name</th>");
  webpage += F("<th class=\"detailsColumn\" onclick=\"sortTable(1);\">Size</th>");
  webpage += F("<th></th>");
  webpage += F("</tr>");
  webpage += F("</thead>");
  webpage += F("<tbody id=\"tbody\">");
  webpage += tree;
  webpage += F("</tbody>");
  webpage += F("</table>");
  webpage += F("<hr>");

  webpage += F("<FORM action='/fupload' method='post' enctype='multipart/form-data'>");
  webpage += F("<input class='buttons' type='file' name='fupload' id = 'fupload' value=''>");
  webpage += F("<button class='buttons' type='submit'>Upload</button></form><br>");

  webpage += F("<FORM action='/mkdir' method='post' enctype='multipart/form-data'>");
  webpage += F("<input type='hidden' id='path' name='path' value='");
  webpage += uploadPath;
  webpage += F("'>");
  webpage += F("<input class='buttons' name='dirName' id ='dirName' value='NewFolder'>");
  webpage += F("<button class='buttons' type='submit'>MkDir</button></form>");
  webpage += F(footer);

  if (tree == "") {
    String dlPath = urldecode(server.uri());
    if (SD.exists(dlPath)) {
      File entry = SD.open(dlPath);
      if (!entry.isDirectory()) {
        Serial.println(dlPath);
        handleFileRead(dlPath);
      }
    }
    else {
      handleNotFound();
    }
  }

  server.send(200, "text/html", webpage);
}

void handleNotFound() {
  String webpage = "";
  webpage += F(header);
  webpage += F("<hr>File Not Found<br>");
  webpage += F("<br>URI:");
  webpage += server.uri();
  webpage += F("<br>Method: ");
  webpage += (server.method() == HTTP_GET) ? "GET" : "POST";
  webpage += F("<br>Arguments: ");
  webpage += server.args();
  webpage += F("<br>");
  for (uint8_t i = 0; i < server.args(); i++) {
    webpage += server.argName(i) + ": " + server.arg(i) + "<br>";
  }
  webpage += F("<br><button class='buttons' onclick=\"location.href='/';\">OK</button>");
  webpage += F(footer);
  server.send(404, "text/html", webpage);
}

void doMkdir() {

  String webpage = "";
  webpage += F(header);
  String path = "";
  String dirName = "";

  for (uint8_t i = 0; i < server.args(); i++) {
    if (server.argName(i) == "dirName") {
      Serial.printf("Dir Name: %s\n", server.arg(i));
      dirName =  server.arg(i);
    }
    if (server.argName(i) == "path") {
      Serial.printf("Path: %s\n", server.arg(i));
      path = server.arg(i);
    }

  }

  if (dirName != "" && path != "") {
    webpage += F("<hr>Creating Dir: <br>");

    if (path == "/")
      path = path + dirName;
    else
      path = path + "/" + dirName;

    webpage += path;

    Serial.printf("Creating Dir: %s\n", path);
    if (SD.mkdir(path)) {
      webpage += F("<br>Dir created<br>");
    } else {
      Serial.println("mkdir failed");
      webpage += F("<br>mkdir failed<br>");
    }
  } else {
    webpage += F("<br>Path or Name empty!");
  }

  webpage += F("<br><button class='buttons' onclick=\"location.href='/';\">OK</button>");
  webpage += F(footer);
  server.send(200, "text/html", webpage);
}

void doDelete() {

  String webpage = "";
  webpage += F(header);
  for (uint8_t i = 0; i < server.args(); i++) {
    if (server.argName(i) == "file") {
      Serial.printf("Deleting file: %s\n", server.arg(i));
      webpage += F("<hr>Deleting file: <br>");
      webpage += server.arg(i);
      if (SD.remove(server.arg(i))) {
        webpage += F("<br>File deleted<br>");
      } else {
        webpage += F("<br>Delete failed<br>");
      }
    }
    if (server.argName(i) == "folder") {
      Serial.printf("Removing Dir: %s\n", server.arg(i));
      webpage += F("<hr>Removing Dir: <br>");
      webpage += server.arg(i);
      if (SD.rmdir(server.arg(i))) {
        webpage += F("<br>Dir removed<br>");
      } else {
        webpage += F("<br>rmdir failed<br>");
      }
    }
  }
  webpage += F("<br><button class='buttons' onclick=\"location.href='/';\">OK</button>");
  webpage += F(footer);
  server.send(200, "text/html", webpage);
}

void deleteConfirm() {
  String webpage = "";
  webpage += F(header);
  for (uint8_t i = 0; i < server.args(); i++) {
    if (server.argName(i) == "file") {
      webpage += F("<hr>Do you want to delete the file:<br>");
      webpage += server.arg(i);
      webpage += F("<br><br><button class='buttons' onclick=\"location.href='/doDelete?file=");
      webpage += server.arg(i);
      webpage += F("';\">Yes</button>");

    }
    if (server.argName(i) == "folder") {
      webpage += F("<hr>Do you want to delete the Directory:<br>");
      webpage += server.arg(i);
      webpage += F("<br><br><button class='buttons' onclick=\"location.href='/doDelete?folder=");
      webpage += server.arg(i);
      webpage += F("';\">Yes</button>");
    }
  }

  webpage += F("<button class='buttons' onclick='window.history.back();'>No</button>");
  webpage += F(footer);
  server.send(200, "text/html", webpage);
}

File UploadFile;
void handleFileUpload() {
  // upload a new file to the Filing system
  HTTPUpload& uploadfile = server.upload(); // See https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer/srcv
  // For further information on 'status' structure, there are other reasons such as a failed transfer that could be used
  if (uploadfile.status == UPLOAD_FILE_START)
  {
    String filename = uploadfile.filename;
    if (!filename.startsWith("/")) filename = uploadPath + "/" + filename;
    Serial.print("Upload File Name: "); Serial.println(filename);
    SD.remove(filename);                         // Remove a previous version, otherwise data is appended the file again
    UploadFile = SD.open(filename, FILE_WRITE); // Open the file for writing in SPIFFS (create it, if doesn't exist)
    filename = String();
  }
  else if (uploadfile.status == UPLOAD_FILE_WRITE)
  {
    if (UploadFile) {
      UploadFile.write(uploadfile.buf, uploadfile.currentSize); // Write the received bytes to the file
    }
  }
  else if (uploadfile.status == UPLOAD_FILE_END)
  {
    if (UploadFile)         // If the file was successfully created
    {
      UploadFile.close();   // Close the file again
      delay(1000);
      Serial.print("Upload Size: "); Serial.println(uploadfile.totalSize);


      String webpage = "";
      webpage += F(header);
      webpage += F("<hr>File was successfully uploaded<br>");
      webpage += F("Uploaded File Name: ");
      webpage += uploadfile.filename + "<br>";
      webpage += F("File Size: ");
      webpage += file_size(uploadfile.totalSize) + "<br>";
      webpage += "<button class='buttons' onclick='window.history.back();'>OK</button>";
      webpage += F(footer);
      server.send(200, "text/html", webpage);
    }
    else
    {
      delay(1000);
      String webpage = "";
      webpage += F(header);
      webpage += F("<hr>Could Not Create Uploaded File (write-protected?)<br>");
      webpage += "<button class='buttons' onclick='window.history.back();'>OK</button>";
      webpage += F(footer);
      server.send(200, "text/html", webpage);
    }
  }
}

String file_size(int bytes) {
  String fsize = "";
  if (bytes < 1024)                 fsize = String(bytes) + " B";
  else if (bytes < (1024 * 1024))      fsize = String(bytes / 1024.0, 3) + " KB";
  else if (bytes < (1024 * 1024 * 1024)) fsize = String(bytes / 1024.0 / 1024.0, 3) + " MB";
  else                              fsize = String(bytes / 1024.0 / 1024.0 / 1024.0, 3) + " GB";
  return fsize;
}

String urldecode(String str)
{

  String encodedString = "";
  char c;
  char code0;
  char code1;
  for (int i = 0; i < str.length(); i++) {
    c = str.charAt(i);
    if (c == '+') {
      encodedString += ' ';
    } else if (c == '%') {
      i++;
      code0 = str.charAt(i);
      i++;
      code1 = str.charAt(i);
      c = (h2int(code0) << 4) | h2int(code1);
      encodedString += c;
    } else {

      encodedString += c;
    }

    yield();
  }

  return encodedString;
}

String urlencode(String str)
{
  String encodedString = "";
  char c;
  char code0;
  char code1;
  char code2;
  for (int i = 0; i < str.length(); i++) {
    c = str.charAt(i);
    if (c == ' ') {
      encodedString += '+';
    } else if (isalnum(c)) {
      encodedString += c;
    } else {
      code1 = (c & 0xf) + '0';
      if ((c & 0xf) > 9) {
        code1 = (c & 0xf) - 10 + 'A';
      }
      c = (c >> 4) & 0xf;
      code0 = c + '0';
      if (c > 9) {
        code0 = c - 10 + 'A';
      }
      code2 = '\0';
      encodedString += '%';
      encodedString += code0;
      encodedString += code1;
      //encodedString+=code2;
    }
    yield();
  }
  return encodedString;

}

unsigned char h2int(char c)
{
  if (c >= '0' && c <= '9') {
    return ((unsigned char)c - '0');
  }
  if (c >= 'a' && c <= 'f') {
    return ((unsigned char)c - 'a' + 10);
  }
  if (c >= 'A' && c <= 'F') {
    return ((unsigned char)c - 'A' + 10);
  }
  return (0);
}

String split(String s, char parser, int index) {
  String rs = "";
  int parserIndex = index;
  int parserCnt = 0;
  int rFromIndex = 0, rToIndex = -1;
  while (index >= parserCnt) {
    rFromIndex = rToIndex + 1;
    rToIndex = s.indexOf(parser, rFromIndex);
    if (index == parserCnt) {
      if (s.substring(rFromIndex, rToIndex) == "")
        break;
    } else parserCnt++;
    rs = s.substring(rFromIndex, rToIndex);
  }
  return rs;
}


// Visualization Functions
void initSPIFFS() {
  if (!SPIFFS.begin()) {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  Serial.println("SPIFFS mounted successfully");
}

String getBNOQuats(){
  imu::Quaternion quat = bno.getQuat();
  readings["quatW"] = String(quat.w());
  readings["quatX"] = String(quat.x());
  readings["quatY"] = String(quat.y());
  readings["quatZ"] = String(quat.z());

  String jsonString = JSON.stringify(readings);
  return jsonString;
}

String getBNOReadings(sensors_event_t* event) {
  double x = -1000000, y = -1000000 , z = -1000000; //dumb values, easy to spot problem
  if (event->type == SENSOR_TYPE_ACCELEROMETER) {
//    Serial.print("Accl:");
    x = event->acceleration.x;
    y = event->acceleration.y;
    z = event->acceleration.z;
    Serial.println("BAD IMU REQUEST1");
  }
  else if (event->type == SENSOR_TYPE_ORIENTATION) {
//    Serial.print("Orient:");
    gyroX = event->orientation.y;
    gyroY = event->orientation.z;
    gyroZ = event->orientation.x;
    readings["gyroX"] = String(gyroX*-3.1415926/180);
    readings["gyroY"] = String(gyroY*3.1415926/180);
    readings["gyroZ"] = String(gyroZ*-3.1415926/180);
  
    String jsonString = JSON.stringify(readings);
    return jsonString;
    // Serial.println("BAD IMU REQUEST2");
  }
  else if (event->type == SENSOR_TYPE_MAGNETIC_FIELD) {
//    Serial.print("Mag:");
    x = event->magnetic.x;
    y = event->magnetic.y;
    z = event->magnetic.z;
    Serial.println("BAD IMU REQUEST3");
  }
  else if (event->type == SENSOR_TYPE_GYROSCOPE) {
//    Serial.print("Gyro:");
    gyroX = event->gyro.x;
    gyroY = event->gyro.y;
    gyroZ = event->gyro.z;
    readings["gyroX"] = String(gyroX);
    readings["gyroY"] = String(gyroY);
    readings["gyroZ"] = String(gyroZ);
  
    String jsonString = JSON.stringify(readings);
    return jsonString;
  }
  else if (event->type == SENSOR_TYPE_ROTATION_VECTOR) {
//    Serial.print("Rot:");
    gyroX = event->gyro.x;
    gyroY = event->gyro.y;
    gyroZ = event->gyro.z;
    readings["gyroX"] = String(gyroX);
    readings["gyroY"] = String(gyroY);
    readings["gyroZ"] = String(gyroZ);
  
    String jsonString = JSON.stringify(readings);
    return jsonString;
  }
  else if (event->type == SENSOR_TYPE_LINEAR_ACCELERATION) {
//    Serial.print("Linear:");
    accX = event->acceleration.x;
    accY = event->acceleration.y;
    accZ = event->acceleration.z;
    readings["accX"] = String(accX);
    readings["accY"] = String(accY);
    readings["accZ"] = String(accZ);
    String accString = JSON.stringify (readings);
    return accString;
  }
  else {
    Serial.print("Unk:");
  }

  // return(String(String(x)+","+String(y)+","+String(z)));
}

String getTemperature(){
  // mpu.getEvent(&a, &g, &temp);
  int temperature = 69; // temp.temperature;
  return String(temperature);
}

void displayCalStatus(void)
{
  /* Get the four calibration values (0..3) */
  /* Any sensor data reporting 0 should be ignored, */
  /* 3 means 'fully calibrated" */
  uint8_t system, gyro, accel, mag;
  system = gyro = accel = mag = 0;
  bno.getCalibration(&system, &gyro, &accel, &mag);

  /* The data should be ignored until the system calibration is > 0 */
  Serial.print("\t");
  if (!system)
  {
    Serial.print("! ");
  }

  /* Display the individual values */
  Serial.print("Sys:");
  Serial.print(system, DEC);
  Serial.print(" G:");
  Serial.print(gyro, DEC);
  Serial.print(" A:");
  Serial.print(accel, DEC);
  Serial.print(" M:");
  Serial.print(mag, DEC);
}

void displaySensorOffsets(const adafruit_bno055_offsets_t &calibData)
{
    Serial.print("Accelerometer: ");
    Serial.print(calibData.accel_offset_x); Serial.print(" ");
    Serial.print(calibData.accel_offset_y); Serial.print(" ");
    Serial.print(calibData.accel_offset_z); Serial.print(" ");

    Serial.print("\nGyro: ");
    Serial.print(calibData.gyro_offset_x); Serial.print(" ");
    Serial.print(calibData.gyro_offset_y); Serial.print(" ");
    Serial.print(calibData.gyro_offset_z); Serial.print(" ");

    Serial.print("\nMag: ");
    Serial.print(calibData.mag_offset_x); Serial.print(" ");
    Serial.print(calibData.mag_offset_y); Serial.print(" ");
    Serial.print(calibData.mag_offset_z); Serial.print(" ");

    Serial.print("\nAccel Radius: ");
    Serial.print(calibData.accel_radius);

    Serial.print("\nMag Radius: ");
    Serial.print(calibData.mag_radius);
}
