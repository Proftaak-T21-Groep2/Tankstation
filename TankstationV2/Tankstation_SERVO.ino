void openEntrance(){
  if(entranceStatus == ENTRANCE_CLOSED){
    entrance.write(90);
    entranceStatus = ENTRANCE_OPEN;
  }
}

void closeEntrance(){
  if(entranceStatus == ENTRANCE_OPEN){
    entrance.write(179);
    entranceStatus = ENTRANCE_CLOSED;  
  }
}

