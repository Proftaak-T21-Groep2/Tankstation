void sendWirelessCommand()
{
  radio.stopListening();
  radio.write(&txWirelessCommand, sizeof(WirelessCommand));
  radio.startListening();
}

void readWirelessCommand()
{
  if (radio.available()) {
    radio.read(&rxWirelessCommand, sizeof(WirelessCommand));  
  }
}
