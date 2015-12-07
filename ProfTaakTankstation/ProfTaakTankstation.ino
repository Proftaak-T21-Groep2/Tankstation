// Arduino Mega
/*-----( Import needed libraries )-----*/
#include <Timer.h>
#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>
#include <Servo.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

/*-----( Declare Constants and Pin Numbers )-----*/
#define CE_PIN   9
#define CSN_PIN 10

/*-----( Communication Protocol )-----*/
typedef struct {
  int commandType;
  int param1;
  char param2[9];
  int param3;
} COMMAND;

#define COMMAND_MOVE_TO_PUMP 1
#define COMMAND_START_FUELING 2
#define COMMAND_PAYMENT_COMPLETE 3
#define COMMAND_MOVE_TO_EXIT 4
#define COMMAND_ARRIVED_AT_PUMP 5
#define COMMAND_FINISHED_FUELING 6

/*-----( Communication Pipes )-----*/
const uint64_t pipeTX = 0xE8E8F0F0E1LL; // Define the transmit pipe
const uint64_t pipeRX = 0xE8E8F0F0D2LL; // Define the receive pipe

RF24 radio(CE_PIN, CSN_PIN); // Create a Radio

COMMAND received_command;
COMMAND command_to_send;

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
int inputLength = 0;

Servo servo;

#define PIN 6
Adafruit_NeoPixel strip = Adafruit_NeoPixel(4, PIN, NEO_GRB + NEO_KHZ800);

int red_led[4];
int green_led[4];
int blue_led[4];
String pumpPlate[4];

String command;
String fuelType;
String lastPlate = "";

Timer timer;
int tankId = 0;
int i = 0;
bool leesUit = false;
bool once = true;


void setup()
{
  Serial.begin(9600);
  Serial1.begin(9600);

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

  command = getCommand();

  if (command == "Petrol" || command == "LPG" || command == "Diesel") {
    tankId = random(0, 4);
    while (pumpPlate[tankId] != "") {
      tankId = random(0, 4);
    }

    setPumpLed(tankId, command);
    setServo(command);

    pumpPlate[tankId] = lastPlate;


    if (once) {
      Serial.println("%ready#");
      once = false;
    }
  }

  if (command == "Paid:GL-07-KL" || command == "Paid:RD-08-MN" || command == "Paid:BL-09-PQ") {
    String plate = command.substring(5);
    for (int i = 0; i < 4; i++) {
      if (pumpPlate[i] == plate) {
        setPumpLed(i, "Off");
        pumpPlate[i] = "";
      }
    }
  }

  checkRFID();
}

void serialEvent1() {
  while (Serial1.available()) {
    char inChar = (byte)Serial1.read();
    if ((int)inChar != 3 && (int)inChar != 2 && (int)inChar != 13) {
      if (inChar == '\n') {
        stringComplete = true;
      }
      else {
        inputString += inChar;
        inputLength++;
      }
    }
  }
}

void checkRFID() {
  if (stringComplete) {
    if (inputLength == 12) {
      if (inputString == "3D0045BE64A2") {
        //lastPlate = "BL-09-PQ";
        Serial.println("Blue");
      }
      else if (inputString == "0100026F402C") {
        //lastPlate = "GL-07-KL";
        Serial.println("Yellow");
      }
      else if (inputString == "3D00A8972321") {
        //lastPlate = "RD-08-MN";
        Serial.println("Red");
      }
      else if (inputString == "41003D45B68F") {
        //        lastPlate = "CX-TV-46";
        Serial.println("Tag 17846");
      }
      else if (inputString == "41003D882EDA") {
        //        lastPlate = "BZ-XW-62";
        Serial.println("Tag 34862");
      }
      else {
        Serial.print("Unknown: "); Serial.println(inputString);
      }
    }

    inputLength = 0;
    inputString = "";
    stringComplete = false;
  }
}

void sendCommand()
{
  radio.stopListening();
  radio.write(&command_to_send, sizeof(COMMAND));
  radio.startListening();
}

void receiveCommand()
{
  if (radio.available()) {
    radio.read(&received_command, sizeof(COMMAND));
    Serial.print("{");
    Serial.print("\n\tCommandType: "); Serial.print(received_command.commandType);
    Serial.print("\n\tP1: "); Serial.print(received_command.param1);
    Serial.print("\n\tP2: "); Serial.print(received_command.param2);
    Serial.print("\n\tP3: "); Serial.print(received_command.param3);
    Serial.println("\n}");
  }
}
