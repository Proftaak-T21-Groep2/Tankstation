char readSerialLine()
{
  
  // send data only when you receive data:
  if (Serial.available() > 0)
  {
    // read the incoming byte:
    int incomingChar = Serial.read();

    // say what you got:
    char receivedCharacter = (char) incomingChar;
    return receivedCharacter;
  } 
  return NULL;
}

