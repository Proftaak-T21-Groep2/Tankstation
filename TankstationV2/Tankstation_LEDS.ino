void updateLeds(){
  for(int i = 0; i < 9; i++){
   pump_one.setPixelColor(i, leds_red[i], leds_green[i], leds_blue[i]);
   pump_two.setPixelColor(i, leds_red[(9+i)], leds_green[(9+i)], leds_blue[(9+i)]);
  }
  pump_one.show();
  pump_two.show();
}

void setLed(int whichLed, int whichPump, int red, int green, int blue){
  if(whichPump == PUMP_ONE){
    leds_red[whichLed] = red;
    leds_green[whichLed] = green;
    leds_blue[whichLed] = blue;
  } else if(whichPump == PUMP_TWO){
    leds_red[(whichLed + 9)] = red;
    leds_green[(whichLed + 9)] = green;
    leds_blue[(whichLed + 9)] = blue;
  } 
}

void pumpStatus(int whichPump, int pumpStatus){
  int r = 0;
  int g = 0;
  int b = 0;

  if(pumpStatus == PUMP_AVAILABLE){
    g = 150;
  } else if(pumpStatus == PUMP_RESERVED){
    r = 150;
    g = 75;
  } else if(pumpStatus == PUMP_NOTAVAILABLE){
    r = 150;
  }
  
  if(whichPump == PUMP_ONE){
    leds_red[0] = r;
    leds_green[0] = g;
    leds_blue[0] = b;  
  }
  else if(whichPump == PUMP_TWO){
    leds_red[9] = r;
    leds_green[9] = g;
    leds_blue[9] = b;
  }
}

void pumpFuelType(int whichPump, int fuelType){
  int r = 0;
  int g = 0;
  int b = 0;

  if(fuelType == FUELTYPE_LPG){
    b = 150;
  } else if(fuelType == FUELTYPE_DIESEL){
    r = 150;
    g = 150;  
  } else if(fuelType == FUELTYPE_PETROL){
    r = 150;
  } else if(fuelType == FUELTYPE_GREEN){
    g = 150;
  }

  for(int i = 1; i < 9; i++){
    if(whichPump == PUMP_ONE){
      leds_red[i] = r;
      leds_green[i] = g;
      leds_blue[i] = b;  
    }
    else if(whichPump == PUMP_TWO){
      leds_red[(9+i)] = r;
      leds_green[(9+i)] = g;
      leds_blue[(9+i)] = b;
    }
  }
}

