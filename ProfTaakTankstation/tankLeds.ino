void setServo(String fuelType){
  if(fuelType == "Petrol"){
      servo.write(1);
  } else if(fuelType == "LPG"){
      servo.write(90);
  } else if(fuelType == "Diesel"){
      servo.write(179);
  }
}


void setPumpLed(int whichPump, String fuelType){
  if(fuelType == "Petrol"){
    green_led[whichPump] = 50;
    red_led[whichPump] = 50;  
  } else if(fuelType == "LPG"){
        red_led[whichPump] = 50;  
  } else if(fuelType == "Diesel"){
      blue_led[whichPump] = 50;
  } else if(fuelType == "Off"){
     red_led[whichPump] = 0;
     green_led[whichPump] = 0;
     blue_led[whichPump] = 0; 
  }
  
  writeBuffer();
}

void writeBuffer(){
 for(int i = 0; i < 4; i++){
  strip.setPixelColor(i, red_led[i], green_led[i], blue_led[i]);
 } 
 strip.show();
}
