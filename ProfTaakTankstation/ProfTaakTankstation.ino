//COM3
#include <Timer.h>
#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>

#include <Servo.h>

Servo servo;

#define PIN 6
Adafruit_NeoPixel strip = Adafruit_NeoPixel(4, PIN, NEO_GRB + NEO_KHZ800);

int red_led[4];
int green_led[4];
int blue_led[4];
String pumpPlate[4];

// Choose two pins for SoftwareSerial
SoftwareSerial rSerial(2, 3); // RX, TX

const int tagLen = 16;
const int idLen = 13;
const int kTags = 5;

String command;
String fuelType;
String lastPlate = "";

Timer timer;
int tankId = 0;
int i = 0;
bool leesUit = false;
bool once = true;

// Put your known tags here!
char knownTags[kTags][idLen] =
{
  "0100026F402C",
  "3D00A8972321",
  "3D0045BE64A2",
  "41003D882EDA",
  "41003D45B68F"
};

// Empty array to hold a freshly scanned tag
char newTag[idLen];

void setup()
{
  // Starts the hardware and software serial ports
  Serial.begin(9600);
  rSerial.begin(9600);

  randomSeed(analogRead(0));

  servo.attach(9);
  
  strip.begin();
  strip.show();
}

void loop()
{
  timer.update();
  // Counter for the newTag array
  // Variable to hold each byte read from the serial buffer
  int readByte;
  // Flag so we know when a tag is over
  boolean tag = false;

  // This makes sure the whole tag is in the serial buffer before
  // reading, the Arduino can read faster than the ID module can deliver!
  if (rSerial.available() == tagLen)
  {
    tag = true;
  }

  command = getCommand();
  
  if (command == "Petrol" || command == "LPG" || command == "Diesel") {
    tankId = random(0, 4);
    while(pumpPlate[tankId] != ""){
      tankId = random(0, 4);      
    }
        
    setPumpLed(tankId, command);
    setServo(command);
    
    pumpPlate[tankId] = lastPlate;
    
    
    if(once){
      Serial.println("%ready#");
      once = false;    
    }
  }
  
  if(command == "Paid:GL-07-KL" || command == "Paid:RD-08-MN" || command == "Paid:BL-09-PQ"){
    String plate = command.substring(5);
    for(int i = 0; i < 4; i++){
      if(pumpPlate[i] == plate){
       setPumpLed(i, "Off");
       pumpPlate[i] = "";
      }
    }
  }
  /*
    if (fuelType == "Petrol")
    {
      digitalWrite(ledPinRed, HIGH);
      timer.after(5000, turnLedsOff);
      //  timer.pulse(ledPinRed, 5000, HIGH);
    }
    else if (fuelType == "Diesel")
    {
      digitalWrite(ledPinYellow, HIGH);
      timer.after(5000, turnLedsOff);
    }
    else if (fuelType == "LPG")
    {
      digitalWrite(ledPinBlue, HIGH);
      timer.after(5000, turnLedsOff);
    }
  */



  if (rSerial.available() > 0 && tag) // && tag == true is hetzelfde als && tag
  {
    for (int a = 0;  a < tagLen ; a++)
    {
      readByte = rSerial.read();
      /* This will skip the first byte (2, STX, start of text) and the last three,
            ASCII 13, CR/carriage return, ASCII 10, LF/linefeed, and ASCII 3, ETX/end of -> Isn't it End of Transmission?
            text, leaving only the unique part of the tag string. It puts the byte into
            the first space in the array, then steps ahead one spot */
      if (readByte != 2 && readByte != 13 && readByte != 10 && readByte != 3)
      {
        newTag[i] = readByte;
        i++;
      }

      // If we see ASCII 3, ETX, the tag is over
      if (readByte == 3)
      {
        tag = false;
        i = 0;
      }


    }


    // don't do anything if the newTag array is full of zeroes
    if (strlen(newTag) != 0)
    {
      if (checkTag(newTag, "0100026F402C"))
      {
        once = true;
        lastPlate = "GL-07-KL";
        Serial.println(lastPlate);
      }

      else if (checkTag(newTag, "3D00A8972321"))
      {
        once = true;
        lastPlate = "RD-08-MN";
        Serial.println(lastPlate);
      }

      else if (checkTag(newTag, "3D0045BE64A2"))
      {
        once = true;
        lastPlate = "BL-09-PQ";
        Serial.println(lastPlate);
      }

      else if (checkTag(newTag, "41003D882EDA"))
      {
        once = true;
        lastPlate = "BZ-XW-62";
        Serial.println(lastPlate);
      }

      else if (checkTag(newTag, "41003D45B68F"))
      {
        once = true;
        lastPlate = "CX-TV-46";
        Serial.println(lastPlate);
      }

      else
      {
        Serial.print("Unknown tag! <");
        Serial.print(newTag);
        Serial.println(">");
      }
    }


    // Once newTag has been checked, fill it with zeroes
    // to get ready for the next tag read
    for (int c = 0; c < idLen; c++)
    {
      newTag[c] = 0;
    }
  }
}

// This function steps through both newTag and one of the known
// tags. If there is a mismatch anywhere in the tag, it will return 0,
// but if every character in the tag is the same, it returns 1
bool checkTag(char nTag[], char oTag[]) {
  for (int i = 0; i < idLen; i++) {
    if (nTag[i] != oTag[i]) {
      return false;
    }
  }
  return true;
}
