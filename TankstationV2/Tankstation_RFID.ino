#define TAG_LENGTH 16
#define ID_LENGTH 13

bool readtag = false;
byte readByte = 0;

char knownTags[5][ID_LENGTH] =
{
  "0100026F402C",
  "3D00A8972321",
  "3D0045BE64A2",
  "41003D882EDA",
  "41003D45B68F"
};

String nummerplaten[5] = {
  "GL-07-KL",
  "RD-08-MN",
  "BL-09-PQ",
  "BZ-XW-62",
  "CX-TV-46",
};

String newNummerplaat = "";

char newTag[ID_LENGTH];

String RAW_RFID_TAG = "";

bool readRFID(){
  if(rfid.available() == TAG_LENGTH || rfid.available() == (TAG_LENGTH + 1)){
    readtag = true;
    int i = 0;
    for(int a = 0; a < TAG_LENGTH; a++)
    {
      readByte = rfid.read();
      if (readByte != 2 && readByte != 13 && readByte != 10 && readByte != 3)
      {
        newTag[i] = readByte;
        i++;
      }

      // If we see ASCII 3, ETX, the tag is over
      if (readByte == 3)
      {
        readtag = false;
        return true;
      }
    }
    return false;
  }
}

String checkRFID(){
  for(int i = 0; i < 5; i++){
    bool matching = true;
    for(int j = 0; j < ID_LENGTH; j++){
      if(newTag[j] != knownTags[i][j]){
        matching = false;
      }
    }
    if(matching){
      newTag[0] = '0';
      newNummerplaat = nummerplaten[i];
      Serial.println(i);
      //Serial.println(newNummerplaat);
    }
  }
}

void printRFID(){
  for(int i = 0; i < ID_LENGTH; i++){
    Serial.print(newTag[i]);
  }
  Serial.println("");
}

