#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>
#include <Servo.h>
#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>

/* PIN DEFINES */
#define SERVOPIN 11

#define LEDPUMP1 22
#define LEDPUMP2 24

#define RFID_RX 10
#define RFID_TX 26

#define CE_PIN   9 // Arduino Uno 9  | Mega 9
#define CSN_PIN 53 // Arduino Uno 10 | Mega 53

const uint64_t pipeTX = 0xE8E8F0F0E1LL; // Define the transmit pipe
const uint64_t pipeRX = 0xE8E8F0F0D2LL; // Define the receive pipe

/* end of PIN DEFINES */

/* begin of value defines */

#define PUMP_ONE 1
#define PUMP_TWO 2

#define PUMP_AVAILABLE 10
#define PUMP_RESERVED 11
#define PUMP_NOTAVAILABLE 12

#define WIRED_CMD_UNKNOWN 20
#define WIRED_CMD_CARSCAN 21
#define WIRED_CMD_FUELTYPE 22
#define WIRED_CMD_FINISHEDFUELING 23
#define WIRED_CMD_PAID 24
#define WIRED_CMD_NOTPAID 25

#define WIRELESS_CMD_MOVETOPUMP 31
#define WIRELESS_CMD_STARTFUELING 32
#define WIRELESS_CMD_PAYMENTCOMPLETE 33
#define WIRELESS_CMD_MOVETOEXIT 34
#define WIRELESS_CMD_ARRIVEDATPUMP 35
#define WIRELESS_CMD_FINISHEDFUELING 36
#define WIRELESS_CMD_ARRIVEDATEXIT 37

#define FUELTYPE_LPG 41
#define FUELTYPE_PETROL 42
#define FUELTYPE_DIESEL 43
#define FUELTYPE_GREEN 44
#define FUELTYPE_OFF 45

#define ENTRANCE_OPEN 51
#define ENTRANCE_CLOSED 52

/* end of value defines */

/* begin of structs */
struct WiredCommand {
  int CommandType;
  int LitersFuel;
  String FuelType;
  String LicensePlate;
};

struct WirelessCommand {
  int CommandType;
  int PumpNumber;
  char LicensePlate[9];
  int Fuel;
};

/* end of structs */

/* start of variables */

int leds_red[18];
int leds_green[18];
int leds_blue[18];

Servo entrance;

Adafruit_NeoPixel pump_one = Adafruit_NeoPixel(9, LEDPUMP1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pump_two = Adafruit_NeoPixel(9, LEDPUMP2, NEO_GRB + NEO_KHZ800);

SoftwareSerial rfid(RFID_RX, RFID_TX); 

RF24 radio(CE_PIN, CSN_PIN); // Create a Radio

String newCommand = "";

WiredCommand newWiredCommand;
WirelessCommand rxWirelessCommand = {};
WirelessCommand txWirelessCommand;

String licenseplatePumpOne = "";
int    statusPumpOne = 0;

String licenseplatePumpTwo = "";
int    statusPumpTwo = 0;

int    entranceStatus = ENTRANCE_OPEN;

/* end of variables */

void setup() {
  rfid.begin(9600);
  Serial.begin(9600);
  
  pump_one.begin();
  pump_two.begin();

  entrance.attach(SERVOPIN);
  closeEntrance();
  pumpStatus(PUMP_ONE, PUMP_AVAILABLE);
  pumpStatus(PUMP_TWO, PUMP_AVAILABLE);
  updateLeds();
  showSimple();
}

void showSimple(){
    openEntrance();
    pumpFuelType(PUMP_ONE, FUELTYPE_PETROL);
    pumpStatus(PUMP_ONE, PUMP_RESERVED);
    updateLeds();
    
    delay(5500);
    closeEntrance();
    
    delay(7000);
    pumpStatus(PUMP_ONE, PUMP_NOTAVAILABLE);
    updateLeds();

    delay(8500);
    pumpStatus(PUMP_ONE, PUMP_RESERVED);
    updateLeds();

    delay(14000);
    pumpFuelType(PUMP_ONE, FUELTYPE_OFF);
    pumpStatus(PUMP_ONE, PUMP_AVAILABLE);
    updateLeds();
    openEntrance();
}


void loop() {
  //if(readRFID()){
  //  checkRFID();
  //}
  newCommand = readWiredCommand();
  
  if(newCommand != ""){
    Serial.println(newCommand);
    
    newWiredCommand = parseWiredCommand(newCommand);
    newCommand = "";

    Serial.println(newWiredCommand.CommandType);
    Serial.println(newWiredCommand.LicensePlate);

    if(newWiredCommand.CommandType == WIRED_CMD_FUELTYPE){
      assignToPump(newWiredCommand.LicensePlate, newWiredCommand.FuelType);
      delay(500);
      //arrivedAtPump();
    }

    if(newWiredCommand.CommandType == WIRED_CMD_PAID){
      paidAtPump(newWiredCommand.LicensePlate);
    }

    if(newWiredCommand.CommandType == WIRED_CMD_NOTPAID){
      
    }
  }

  //readWirelessCommand();

  if(rxWirelessCommand.CommandType == WIRELESS_CMD_ARRIVEDATPUMP){
    arrivedAtPump(charToString(rxWirelessCommand.LicensePlate));
  } else if(rxWirelessCommand.CommandType == WIRELESS_CMD_ARRIVEDATEXIT){
    arrivedAtExit(charToString(rxWirelessCommand.LicensePlate));
  } else if(rxWirelessCommand.CommandType == WIRELESS_CMD_FINISHEDFUELING){
    finishedFueling(charToString(rxWirelessCommand.LicensePlate), rxWirelessCommand.Fuel);
  }
  
  delay(50);
}

void assignToPump(String licenseplate, String fueltype){
  int fuelType = 0;
  if(fueltype.equals("DSL")){
    fuelType = FUELTYPE_DIESEL;
  } else if(fueltype.equals("PTL")){
    fuelType = FUELTYPE_PETROL;
  } else if(fueltype.equals("LPG")){
    fuelType = FUELTYPE_LPG;
  }
  
  if(licenseplatePumpOne.equals("")){
    licenseplatePumpOne = licenseplate;
    statusPumpOne = PUMP_RESERVED;
    pumpStatus(PUMP_ONE, statusPumpOne);
    
    openEntrance();
    pumpFuelType(PUMP_ONE, fuelType);
    updateLeds();

// SendWirelessCommand: MoveToPump(kenteken, 1)
  } else if(licenseplatePumpTwo.equals("")){
    licenseplatePumpTwo = licenseplate;
    statusPumpTwo = PUMP_RESERVED;
    pumpStatus(PUMP_TWO, statusPumpTwo);
    
    openEntrance();
    pumpFuelType(PUMP_TWO, fuelType);
    updateLeds();

// SendWirelessCommand: MoveToPump(kenteken, 1)
  }
}

void arrivedAtPump(String licenseplate){
  if(licenseplatePumpOne.equals(licenseplate)){
    pumpStatus(PUMP_ONE, PUMP_NOTAVAILABLE);
    //sendStartFueling(kenteken, stringToChar(licenseplate), 10);
  } else if(licenseplatePumpTwo.equals(licenseplate)){
    pumpStatus(PUMP_TWO, PUMP_NOTAVAILABLE);
    //sendStartFueling(kenteken, stringToChar(licenseplate), 10);
  }
  updateLeds();
}

void finishedFueling(String licenseplate, int fueltanked){
  Serial.println("%");
  //sendWiredCommand(kenteken, fueltanked);
}

void paidAtPump(String licenseplate){
  if(licenseplatePumpOne.equals(licenseplate)){
    pumpStatus(PUMP_ONE, PUMP_RESERVED);
    //sendMoveToExit(kenteken);
  } else if(licenseplatePumpTwo.equals(licenseplate)){
    pumpStatus(PUMP_TWO, PUMP_RESERVED);
    //sendMoveToExit(kenteken);
  }
  updateLeds();
}

void arrivedAtExit(String licenseplate){
  if(licenseplatePumpOne.equals(licenseplate)){
    pumpStatus(PUMP_ONE, PUMP_AVAILABLE);
  } else if(licenseplatePumpTwo.equals(licenseplate)){
    pumpStatus(PUMP_TWO, PUMP_AVAILABLE);
  }
  updateLeds();  
}



String charToString(char* array){
  String output = "";
  for(int i = 0; i < 8; i++){
    output += array[i];
  }
  return output;
}

char* stringToChar(String kenteken){
  char output[9];
  for(int i = 0; i < 8; i++){
    output[i] = kenteken.charAt(i);
  }
  output[8] = '\0';
  return output;
}


