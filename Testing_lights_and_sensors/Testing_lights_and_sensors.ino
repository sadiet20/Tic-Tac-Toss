int turns = 0;
char player = 'b';
char winner = 't';

int index;
int reading;
int vibration;
bool found;
bool miss;

//char gamestate[3][3] = {{'0', '0', '0'}, {'0', '0', '0'}, {'0', '0', '0'}};
//int pins[3][3] = {{A0, A1, A2}, {A3, A4, A5}, {A6, A7, A8}};    //call with value = analogRead(pins[i][k])

char gamestate[3] = {'0', '0', '0'};
int pins[3] = {A0, A1, A2};
int lights[3][3] = {{22, 23, 24}, {25, 26, 27}, {28, 29, 30}}; //digitalWrite(lights[i])


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}
/*
void loop() {
  // put your main code here, to run repeatedly:


   //TESTING EACH LIGHT AND EACH COLOR
   for(int i=0; i<3; i++){
    for(int k=0; k<3; k++){
      digitalWrite(lights[i][k], HIGH);
      delay(800);
      digitalWrite(lights[i][k], LOW);
    }
   }
/*
   //TESTING THREE WHITE LIGHTS
   for(int i=0; i<3; i++){
    for(int k=0; k<6; k++){
      if(k<3){
        digitalWrite(lights[i][k], HIGH); //turn on all three colors
      }
      else if(k==3){  //once all are on
        delay(1000);  //wait one second
      }
      else{   //if k>3
        digitalWrite(lights[i][k], LOW);  //turn off all three colors
      }
    }
   }
}

*/
void loop(){
  found = false;
  miss = false;
  waitforinput();
  
  Serial.println("Out of while loop");
  
  if(found){
    shotmade();   //turn on corresponding light for one second
  }
  else{
    missedshot();   //blink lights three times
  }
}


//LOOP UNTIL RECEIVE INPUT FROM SENSOR (LIGHT OR VIBRATION)
void waitforinput(){
 while(!found && !miss){     //loop waiting for input
  for(index=0; index<3; index++){     //check each of the sensors
   reading = analogRead(pins[index]);      //get reading
   if(reading < 20){     //if reading is below threshold of 20
    Serial.print("Got reading below 20 from index ");
    Serial.println(index);
    found = true;     //break while loop
    return;            //return (index remains the index of the activated sensor for use outside of the loop)
   }
   vibration = analogRead(9);   //get reading from vibration sensor
   if(vibration > 60){    //if vibration is lower than threshold of 60
     Serial.println("Vibration detected");
     miss = true;
     return;
   }
  }
 }
}


//TURN ON CORRESPONDING LIGHT FOR THE SENSOR THAT TURNED ON 
void shotmade(){
    digitalWrite(lights[index][0], HIGH);
    delay(1000);
    digitalWrite(lights[index][0], LOW);
}


//BLINK ALL (THREE) LIGHTS RED TO SIGNIFY A MISS
void missedshot(){
  for(int i=0; i<3; i++){
    for(int k=0; k<7; k++){
      if(k<3){
        digitalWrite(lights[k][0], HIGH); //turn on all red lights
      }
      else if(k==3){    //once all are on
        delay(500);   //wait half a second once all are on
      }
      else{   //if k>3
        digitalWrite(lights[k-4][0], LOW);    //turn off all lights
      }
      if(k==6){
        delay(500);   //wait half a second once all are off
      }
    }
  } 
}
