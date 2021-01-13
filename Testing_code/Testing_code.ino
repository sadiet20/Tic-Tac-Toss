/*
 * Testing: new game setup, infrared, vibration sensor, proper lights turn on, missed shot show, switching players (need to add in index)
 * Commented out: delay after sensor input (infrared and vibration), tie game, 
 * Add in code for... checking winner, light shows
 */


int turns;      //number of turns completed (after 9 then the board is full)
int player;     //player represents the color of the current player (starts out red)

int row;
int column;
int reading;
int vibration;
bool make;
bool miss;

int gamestate[3][3] = {{-1, -1, -1}, {-1, -1, -1}, {-1, -1, -1}};   //when pocket is made, the corresponding element in gamestate is changed to the current player (represented by their color so 0, 1, or 2 for red, blue, or green)
int sensors[3][3] = {{A0, A1, A2}, {A3, A4, A5}, {A6, A7, A8}};    //call with value = analogRead(sensors[i][k])
int lights[3][3][3] = {                                            //lights are ordered by row, column, color (red, green, blue)
  {{22, 23, 24}, {25, 26, 27}, {28, 29, 30}}, 
  {{31, 32, 33}, {34, 35, 36}, {37, 38, 39}}, 
  {{40, 41, 42}, {43, 44, 45}, {46, 47, 48}}
};

int diaglights[9][3] = {{22, 23, 24}, {31, 32, 33}, {25, 26, 27}, {40, 41, 42}, {34, 35, 36}, {28, 29, 30}, {43, 44, 45}, {37, 38, 39}, {46, 47, 48}};



void setup() {
  Serial.begin(9600);
  newgame(2);     //gives input of player 2 (blue) which switches so that the first player is player 1 (red)
}

void loop() {
  Serial.print("Current player: ");
  Serial.println(player);
  
  //LOOP UNTIL RECIEVES INPUT FROM EITHER LIGHT OR VIBRATION SENSOR
  waitforinput();     //changes either make or miss to true based on which sensor was activated
  
  Serial.println("Got input");
  
  if(make && gamestate[row][column]==-1){       //if player made a shot (light sensor) and that spot has not already been taken (shot already made)
    Serial.println("Made a shot!");
    //UPDATE LIGHT AND GAMESTATE
    digitalWrite(lights[row][column][player], HIGH);     //turn on corresponding light the color of the current player
    gamestate[row][column] = player;            //changes element to current player's color

    /*
    //CHECK FOR WINNER
    if(horizontalwin() || verticalwin() || leftdiagonalwin() || rightdiagonalwin()){
      //winner light show
      delay(5000);        //wait 5 seconds for celebration
      newgame(player);          //start new game (player will switch so that the loser goes first)
    }
    
    //CHECKS IF BOARD IS FULL
    if(++turns==9){               //adds one to turns then checks if it equals 9
      //tie game celebration
      delay(5000);      //wait 5 seconds
      newgame(player);        //start new game (player will switch so that the person who played last goes second)
    }

    */
  }
  else{           //if player missed a shot (vibration sensor) or if they made a shot that has already been made
    Serial.println("Close but no cigar");
    turnofflights();
    missedshot();
    resetlights();
  }  
  
  //SWITCH PLAYERS (CHANGES COLOR OF LIGHTS)
  switchplayers();
}


//TURNS OFF ALL LIGHTS AND RESETS VARIABLES READY FOR A NEW GAME
void newgame(int previousplayer){
  turnofflights();    //make sure all lights are switched to off
  turns = 0;          //number of completed turns
  player = previousplayer;         //color index of previous player to take a turn
  switchplayers();                 //switches current player so that whoever took the last turn now goes second
  for(int i=0; i<3; i++){
    for(int k=0; k<3; k++){
      gamestate[i][k] = -1;   //-1 signifies that location has not been taken
    }
  }
}


//SWAPS CURRENT PLAYER
void switchplayers(){
  Serial.print("Switching players from ");
  Serial.print(player);
  Serial.print(" to ");
  if(player==0){
    player = 2;
  }
  else{
    player = 0;
  } 
  Serial.println(player);
  //ADD CHANGE COLOR OF CURRENT PLAYER ON BOARD!!!!!!!!!!!!
}


//LOOP UNTIL RECEIVE INPUT FROM SENSOR (LIGHT OR VIBRATION)
void waitforinput(){
  //RESET VALUES THAT INDICATE A SENSOR HAS RECEIVED INPUT
  make = false;
  miss = false;
  
  Serial.println("Waiting for sensor input");
  while(!make && !miss){     //loop waiting for input
    for(row = 0; row < 3; row++){     //check each of the sensors
      for(column = 0; column < 3; column++){
        reading = analogRead(sensors[row][column]);      //get reading
        if(reading < 20){     //if reading is below threshold of 20
          Serial.print("Got reading below 20 from row ");
          Serial.println(row);
          make = true;     //break while loop
          //hangingdelay();
          return;            //return (row and column remain the row and column of the activated sensor for use outside of the loop)
        }
        vibration = analogRead(9);   //get reading from vibration sensor
        if(vibration > 60){    //if vibration is lower than threshold of 60
          Serial.println("Vibration detected");
          miss = true;
          //vibrationdelay();
          return;
        }
      }
    }
  }
}

//RESETS LIGHTS SO THAT THEY MATCH THE CURRENT GAMESTATE 
void resetlights(){
  Serial.println("Reseting lights based on gamestate");
  for(int i=0; i<3; i++){
    for(int k=0; k<3; k++){
      if(gamestate[i][k]!=-1){    //-1 signifies the element is not taken, so only turn on light if element is not taken
        digitalWrite(lights[i][k][gamestate[i][k]], HIGH);    //the element of gamestate[i][k] is the color index of the player who has that spot
      }
    }
  }
}


//TURNS OFF ALL LIGHTS OF ALL COLORS
void turnofflights(){
  Serial.println("Turning off all lights");
  for(int i=0; i<3; i++){
    for(int k=0; k<3; k++){
      for(int j=0; j<3; j++){
        digitalWrite(lights[i][k][j], LOW);
      }
    }
  }
}


//BLINK ALL LIGHTS RED TO SIGNIFY A MISS
//PRE CONDITIONS: all lights must be off when this is called
//POST CONDITIONS: all lights are off
void missedshot(){
  Serial.println("Missed shot. Blinking red three times");
  for(int i=0; i<3; i++){         //blinks three times
    for(int k=0; k<7; k++){
      if(k<3){      //turn lights on
        for(int j=0; j<3; j++){
          digitalWrite(lights[k][j][0], HIGH);    //turns each red light on
        }
      }
      else if(k==3){      //once all lights are on
        delay(500);       //wait half a second
      }
      else{     //if k>3    //turn lights off
        for(int j=0; j<3; j++){
          digitalWrite(lights[k][j][0], LOW);   //turns each red light off
        }
      }
      if(k==6){         //once all lights are off again
        delay(500);     //wait half a second
      }
    }
  }
}
