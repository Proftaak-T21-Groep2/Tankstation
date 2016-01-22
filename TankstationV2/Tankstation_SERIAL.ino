char receivedChar;
String command = "";

bool readingCommand = false;
bool finishedCommand = false;

/* end of variables */

char readChar(){
  if(Serial.available() > 0){
    return (char) Serial.read();
  }
  else {
    return NULL;
  }
}

String readWiredCommand(){
  receivedChar = readChar();
  if(receivedChar == '%' && !readingCommand){
    command = "";
    readingCommand = true;
  }

  if(receivedChar == '#' && readingCommand){
    readingCommand = false;
    finishedCommand = true;
  }

  if(readingCommand && !finishedCommand){
    if(receivedChar != '%'){
      command += String(receivedChar); 
    }
  }

  if(finishedCommand){
    finishedCommand = false;
    readingCommand = false;
    return command;
  }

  return "";
}

WiredCommand parseWiredCommand(String incommand){
  if(incommand.startsWith("FuelType:")){
    WiredCommand command = {WIRED_CMD_FUELTYPE, 0.0, incommand.substring(9, 12), incommand.substring(13)};
    return command;  
  } else if(incommand.startsWith("Paid:")){
    WiredCommand command = {WIRED_CMD_PAID, 0.0, "", incommand.substring(5)};
    return command;
  } else if(incommand.startsWith("NotPaid:")){
    WiredCommand command = {WIRED_CMD_NOTPAID, 0.0, "", incommand.substring(8)};
    return command;
  } else {
    WiredCommand command = {WIRED_CMD_UNKNOWN, 0.0, "", "unknown"};
    return command;
  }
}

void sendWiredCommand(WiredCommand incommand){
  String command = "%";

  if(incommand.CommandType == WIRED_CMD_CARSCAN){
    command += "LicensePlate:";
    command += incommand.LicensePlate;
  }
  else if(incommand.CommandType == WIRED_CMD_FINISHEDFUELING){
    command += "Fueled:";
    command += incommand.LitersFuel;
    command += ";";
    command += incommand.LicensePlate;
  }
  command += "#";
  Serial.println(command);
}

