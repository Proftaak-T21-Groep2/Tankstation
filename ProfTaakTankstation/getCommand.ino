String unfinished;
bool reading = false;
bool done = false;

String getCommand()
{
  char receivedCharacter = readSerialLine();
  if (receivedCharacter == '%' && !reading)
  {
    unfinished = "";
    reading = true;
  }

  if (receivedCharacter == '#' && reading)
  {
    reading = false;
    done = true;
  }


  if (reading && !done)
  {
    if (receivedCharacter != '%'){
      unfinished += String(receivedCharacter);
    }
  }


  if (done)
  {
    done = false;
    reading = false;
    return unfinished;
  }
  return " ";
}
