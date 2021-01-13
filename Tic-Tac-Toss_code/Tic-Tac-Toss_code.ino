/*
 * To add:
 * - change color of current player on board
 * - winner celebration
 * - tie celebration
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
    //UPDATE LIGHT AND GAMESTATE
    digitalWrite(lights[row][column][player], HIGH);     //turn on corresponding light the color of the current player
    gamestate[row][column] = player;            //changes element to current player's color
    
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
  }
  else{           //if player missed a shot (vibration sensor) or if they made a shot that has already been made
    turnofflights();
    missedshot();
    resetlights();
  }  
  
  //SWITCH PLAYERS (CHANGES COLOR OF LIGHTS)
  switchplayers();
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
          hangingdelay();
          return;            //return (row and column remain the row and column of the activated sensor for use outside of the loop)
        }
        vibration = analogRead(9);   //get reading from vibration sensor
        if(vibration > 60){    //if vibration is lower than threshold of 60
          Serial.println("Vibration detected");
          miss = true;
          vibrationdelay();
          return;
        }
      }
    }
  }
}


//WAITS AFTER VIBRATION IS DETECTED TO SEE IF THE BAG FALLS IN A HOLE
void vibrationdelay(){
  for(int i=0; i<100; i++){          //might want to change how many times this loops based on trial and error
    for(row = 0; row < 3; row++){     //check each of the sensors
      for(column = 0; column < 3; column++){
        reading = analogRead(sensors[row][column]);      //get reading
        if(reading < 20){     //if reading is below threshold of 20
          Serial.print("Got reading below 20 from row ");
          Serial.println(row);
          make = true;      //bag fell in the hole
          miss = false;     //ignore the miss
          return;            //return (row and column remain the row and column of the activated sensor for use outside of the loop)
        }
      }
    }
  }
}


//MAKES SURE THE BAG WENT COMPLETELY THROUGH THE HOLE (NOT JUST HANGING)
void hangingdelay(){
  delay(100);       //may want to alter the delay based on trial and error
  if(sensors[row][column]<20){    //executes if still getting reading from sensor (bag is hanging over edge)
    make = false;                 //change values of make and miss, since it doesn't count if the bag doesn't go all the way through
    miss = true;
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


//CHECKS IF CURRENT PLAYER WON ALONG A HORIZONTAL
bool horizontalwin(){
  for(int i=0; i<3; i++){
    for(int k=0; k<3; k++){
      if(gamestate[i][k]!=player){    //must get all three in order to win, so if one is not the current player's piece then check the next row
        break;
      }
      if(k==2){     //if got this far without breaking inner for loop, then the current player won
        Serial.println("Horizontal win by player ");
        Serial.println(player);
        horizontalcelebration(i);
        return true;
      }
    }
  }
  return false;
}


//BLINKS WHITE AND WINNER'S COLOR IN WINNING ROW THREE TIMES
void horizontalcelebration(int row){
  for(int i=0; i<3; i++){       //blinks three times
    for(int k=0; k<3; k++){     //turn off lights in row
      digitalWrite(lights[row][k][player], LOW);
    }
    for(int k=0; k<3; k++){   //turn on white in entire row
      digitalWrite(lights[row][k][0], HIGH);
      digitalWrite(lights[row][k][1], HIGH);
      digitalWrite(lights[row][k][2], HIGH);
    }
    delay(100);             //delay
    for(int k=0; k<3; k++){   //turn off lights in row
      digitalWrite(lights[row][k][0], LOW);
      digitalWrite(lights[row][k][1], LOW);
      digitalWrite(lights[row][k][2], LOW);
    }
    for(int k=0; k<3; k++){   //turn on player's color in row
      digitalWrite(lights[row][k][player], HIGH);
    }
    delay(100);
  }
}


//CHECKS IF CURRENT PLAYER WON ALONG A VERTICAL
bool verticalwin(){
  for(int i=0; i<3; i++){
    for(int k=0; k<3; k++){
      if(gamestate[k][i]!=player){    //must get all three in order to win, so if one is not the current player's piece then check the next row
        break;
      }
      if(k==2){     //if got this far without breaking inner for loop, then the current player won
        Serial.println("Vertical win by player ");
        Serial.println(player);
        verticalcelebration(i);
        return true;
      }
    }
  }
  return false;
}


//BLINKS WHITE AND WINNER'S COLOR IN WINNING COLUMN THREE TIMES
void verticalcelebration(int col){
  for(int i=0; i<3; i++){       //blinks three times
    for(int k=0; k<3; k++){     //turn off lights in column
      digitalWrite(lights[k][col][player], LOW);
    }
    for(int k=0; k<3; k++){   //turn on white in entire column
      digitalWrite(lights[k][col][0], HIGH);
      digitalWrite(lights[k][col][1], HIGH);
      digitalWrite(lights[k][col][2], HIGH);
    }
    delay(100);             //delay
    for(int k=0; k<3; k++){   //turn off lights in column
      digitalWrite(lights[k][col][0], LOW);
      digitalWrite(lights[k][col][1], LOW);
      digitalWrite(lights[k][col][2], LOW);
    }
    for(int k=0; k<3; k++){   //turn on player's color in column
      digitalWrite(lights[k][col][player], HIGH);
    }
    delay(100);           //delay
  }
}


//CHECKS IF CURRENT PLAYER WON ALONG THE DIAGONAL FROM TOP LEFT TO BOTTOM RIGHT
bool leftdiagonalwin(){
  for(int i=0; i<3; i++){
    if(gamestate[i][i]!=player){    //must have all three of the diagonal, so if find one that is not the current player's piece, then false
      return false;
    }
  }
  Serial.println("Left diagonal win by player ");
  Serial.println(player);
  leftdiagcelebration();
  return true;
}


//BLINKS WHITE AND WINNER'S COLOR IN WINNING LEFT DIAGONAL THREE TIMES
void leftdiagcelebration(){
  for(int i=0; i<3; i++){       //blinks three times
    for(int k=0; k<3; k++){     //turn off lights in diag
      digitalWrite(lights[k][k][player], LOW);
    }
    for(int k=0; k<3; k++){   //turn on white lights
      digitalWrite(lights[k][k][0], HIGH);
      digitalWrite(lights[k][k][1], HIGH);
      digitalWrite(lights[k][k][2], HIGH);
    }
    delay(100);             //delay
    for(int k=0; k<3; k++){   //turn off lights
      digitalWrite(lights[k][k][0], LOW);
      digitalWrite(lights[k][k][1], LOW);
      digitalWrite(lights[k][k][2], LOW);
    }
    for(int k=0; k<3; k++){   //turn on player's color
      digitalWrite(lights[k][k][player], HIGH);
    }
    delay(100);           //delay
  }
}


//CHECKS IF CURRENT PLAYER WON ALONG THE DIAGONAL FROM TOP RIGHT TO BOTTOM LEFT
bool rightdiagonalwin(){
  for(int i=0; i<3; i++){
    if(gamestate[i][2-i]!=player){    //must have all three of the diagonal, so if find one that is not the current player's piece, then false
      return false;
    }
  }
  Serial.println("Right diagonal win by player ");
  Serial.println(player);
  rightdiagcelebration();
  return true;
}


//BLINKS WHITE AND WINNER'S COLOR IN WINNING RIGHT DIAGONAL THREE TIMES
void rightdiagcelebration(){
  for(int i=0; i<3; i++){       //blinks three times
    for(int k=0; k<3; k++){     //turn off lights in diag
      digitalWrite(lights[k][2-k][player], LOW);
    }
    for(int k=0; k<3; k++){   //turn on white lights
      digitalWrite(lights[k][2-k][0], HIGH);
      digitalWrite(lights[k][2-k][1], HIGH);
      digitalWrite(lights[k][2-k][2], HIGH);
    }
    delay(100);             //delay
    for(int k=0; k<3; k++){   //turn off lights
      digitalWrite(lights[k][2-k][0], LOW);
      digitalWrite(lights[k][2-k][1], LOW);
      digitalWrite(lights[k][2-k][2], LOW);
    }
    for(int k=0; k<3; k++){   //turn on player's color
      digitalWrite(lights[k][2-k][player], HIGH);
    }
    delay(100);           //delay
  }
}


//SNAKES COLOR AROUND BOARD ASSUMES ALL LIGHTS ARE OFF
void show(int color){
  for(int i=0; i<3; i++){
    for(int k=0; k<3; k++){
      digitalWrite(lights[i][k][color], HIGH);
      if(!(i==0 && k==0)){
        delay(100);
        if(k==0){
          digitalWrite(lights[i-1][2][color], LOW);
        }
        else{
          digitalWrite(lights[i][k-1][color], LOW);
        }
      }
    }
  }
  digitalWrite(lights[2][2][color], LOW);
}
