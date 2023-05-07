// Taken from the author's website with some modification
// Author - (c) Michael Schoeffler 2019, http://www.mschoeffler.de
//

#include <SPI.h>
#include <SD.h>
#include <TFT.h> 

#define PIN_SD_CS 4
#define PIN_TFT_CS 10
#define PIN_A0 9
#define PIN_RST 8

#define DELAY_IMAGE_SWAP 10000 // each image is shown for 10 seconds

TFT tftScreen = TFT(PIN_TFT_CS, PIN_A0, PIN_RST);

void setup() {
  // initialize default serial connection to send debug information
  Serial.begin(9600);
  while (!Serial) {
    // wait until default serial connection is fully set up
  }

  // This is not true in May 2023
  //The following two lines replace "tftScreen.begin();" => avoids that red and blue (?) are swapped/interchanged
  //  tftScreen.initR(INITR_BLACKTAB);
  //  tftScreen.setRotation(1);
  tftScreen.begin();
  tftScreen.setRotation(2); // set rotation
  Serial.println("set rotation to 2");  
  
  tftScreen.background(50, 50, 50); // prints black screen to TFT display
  
  init_SD(); // function call that initializes SD card
}

void init_SD() {
  // try to init SD card
  Serial.print(F("SD card init..."));
  if (!SD.begin(PIN_SD_CS)) {
    Serial.println(F("ERROR")); // failed
    return;
  }
  Serial.println(F("SUCCESS")); // ok
}

void loop() {
  File dir = SD.open("/"); // open root path on SD card
  File entry;
  char name[16];
  bool worked_once = false;

  while (entry = dir.openNextFile()) { // iteratively opens all files on SD card. 
    Serial.print(F("Opened File: "));
    Serial.println(entry.name());
    strcpy(name, entry.name()); // file name is copied to variable "name"
    entry.close(); // After copying the name, we do not need the entry anymore and, therefore, close it.
    int filename_len = strlen(name);
    if ((filename_len >= 4 && strcmp(name + filename_len - 4, ".BMP") == 0)) { // check if the current filename ends with ".BMP". If so, we might have an image.
      PImage image = tftScreen.loadImage(name); // loads the file from the SD card
      if (image.isValid()) { // If the loaded image is valid, we can display it on the TFT.
        Serial.println(F("Image is valid... drawing image."));
        tftScreen.image(image, 0, 0); // this function call displays the image on the TFT. 
        worked_once = true; // we set this variable to true, in order to indicate that at least one image could be displayed
        delay(DELAY_IMAGE_SWAP);
        tftScreen.background(50, 50, 50);
      } else {
        Serial.println(F("Image is not valid... image is not drawn."));  
      }
      image.close(); // image is closed as we do not need it anymore.
    } else {
      Serial.println(F("Filename does not end with BMP!"));  
    }
  }
  dir.close(); // directory is closed

  if (worked_once == false) { // if not a single image could be shown, we reconnect to the SD card reader.
    Serial.println(F("Warning: Printing an image did not work once! Trying to reinitalize SD card reader."));        
    SD.end();
    init_SD();
  }

}
