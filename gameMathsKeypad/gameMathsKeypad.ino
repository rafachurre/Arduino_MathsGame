/*
 * Mathematics game
 * 
 * This is a maths game with different levels. It uses a 4x4 keypad.
 * 
 * Game:  It shows an operation ('+', '-' or '*') and wait for the result
 *        There are diferent levels:
 *        Level 1 to 5: Only sums of 1 digit numbers
 *        Level 5 to 10: Only subtractions of 1 digit numbers
 *        Level 10 to 15: Only sums of 1 or 2 digit numbers
 *        Level 15 to 20: Only subtraction of 1 or 2 digit numbers
 *        Level 20 to 25: Only multiplication of 1 digit numbers
 *        Level 25 to 30: Only multiplication of 1 or 2 digit numbers
 *        Level 30 or greater: Random operation of 1 or 2 digit numbers
 *        
 *        'A', 'B', 'C', 'D', '#', '*' keys enter the player result
 * 
 * Wiring:
 *    Keypad rows: pins 2 to 5;
 *    Keypad columns: pins 6 to 9;
 *    
 * 
 * @author Rafael Lopez Martinez
 * @date 19 March 2016
 */

#include <Keypad.h>

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {5, 4, 3, 2}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {9, 8, 7, 6}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

//vars
bool waitResult = false; //flag: when new result submitted -> true
bool pressKey = false; //flag: player pressed a random key -> true
int numberOfErrors = 0; //counter: number of fails
int level = 1; //counter: current level
int number1 = 0; //operation: firts number
int number2 = 0; //operation: second number
int operation = 1; //1='+'; 2='-'; 3='x';
String resultString = ""; //stores the pressed numbers in a String

void setup(){
  Serial.begin(9600);

  Serial.println("----------------------------");
  Serial.println("Bienvenido al SumaCaramelo");
  Serial.println("----------------------------");
  Serial.println();

  randomSeed(analogRead(0));
}

void loop(){
  if (!waitResult && !pressKey){ //If I'm not waiting for a result or a random key
    if(numberOfErrors < 3){ // if I still have lifes. Max 3 failures
      
      //Generate numbers and the operation
      generateOperation();
      generateNumbers();
      
      //Print level
      Serial.println("--------");
      Serial.print("Nivel ");
      Serial.println(String(level));
      Serial.println("--------");
      
      //print operation
      Serial.print(String(number1) + " ");
      switch(operation){
        case 1: 
          Serial.print("+");
          break;
        case 2: 
          Serial.print("-");
          break;
        case 3: 
          Serial.print("x");
          break;
        default:
          Serial.print("*");
          break;
      }
      Serial.print(" " + String(number2));
      Serial.println("=");
      
      //set flags: waiting for result
      waitResult = true;
      pressKey = false;
    }
    else{  //if no more lifes
      Serial.println("Ya has fallado 3 veces. Tomate un descanso");
      waitResult = false;
      pressKey = true;
    }
  }
  else{ //if waiting for any result or a random key
    char key = keypad.getKey();
    if (key != NO_KEY){//if key pressed
      
      // if I was waiting for a random key
      if (pressKey){
        pressKey = false; //reset flag: a key was already pressed
      }

      // if waiting for a result
      else if (key == '*' ||key == '#' ||key == 'A' ||key == 'B' ||key == 'C' ||key == 'D'){ //if I press a key different than a number: submit the result
        Serial.println();
        //Serial.println(resultString); //debugging
        checkResult(resultString);
        resultString = ""; //clear String for the next operation
      }

      else{ // While introducing numbers: store and print
        resultString += key;
        Serial.print(key);
      }
    }
  }
}

/*
 * Check if the result is correct
 */
void checkResult(String resultString){  
  int resultInteger = resultString.toInt();
  int trueResult = 0;

  //calculate trueResult
  switch(operation){ // calculate the correct result depending on the operation
    case 1: 
      trueResult = number1+number2;
      break;
    case 2: 
      trueResult = number1-number2;
      break;
    case 3:
      trueResult = number1*number2;
      break;
    default:
      trueResult = number1*number2;
      break;
  }
  if(trueResult == resultInteger){ //if correct
    Serial.println("Acertaste!!!!"); //print greetings message
    waitResult = false; //set flag: not waiting for result
    pressKey = true; //set flag: press a random key for the next operation
    level += 1; //level up
  }
  else{ //if incorrect
    Serial.println("OOOHH!! Fallaste :("); //print failure message
    Serial.println("El resultado era: " + String(trueResult)); // print correct result
    Serial.println("Prueba otra vez");
    waitResult = false; //set flag: not waiting for result
    pressKey = true; //set flag: press a random key for the next operation
    numberOfErrors += 1; //increase the errors counter
  }
}

/*
 * Generates random numbers depending on the level
 */
void generateNumbers(){
  int maxRandom = 10; //First 10 levels: only 1 digit operations (add and substract)
  if (level>=10 && level<20){ //level 10 to 20: 1 or 2 digits operations (add and substract)
    maxRandom = 100;
  }
  else if (level>=20 && level<25){ //level 20 to 25: only 1 digit operations (multiply)
    maxRandom = 10;
  }
  else if (level>=25){ //level >=25: 1 or 2 digits number (25 to 30 multiply; >30 random operation)
    maxRandom = 100;
  }
  number1 = random(maxRandom); //generates number 1
  number2 = random(maxRandom); //generates number 2
  
  if(operation == 2){ // if substraction, avoid negative resulsts (number2 always < number1)
    int temp = min(number1,number2);
    number1 = max(number1, number2);
    number2 = temp;
  }
}

/*
 * Generates operation depending on level
 */
void generateOperation(){
  if(level<5){ //level 1 to 5: add (1 digit)
    operation = 1;
  }else if (level>=5 && level<10){ //level 5 to 10: substract (1 digit)
    operation = 2;
  }
  else if (level>=10 && level<15){ //level 10 to 15: add (1 or 2 digits)
    operation = 1;
  }
  else if (level>=15 && level<20){ //level 15 to 20: substract (1 or 2 digits)
    operation = 2;
  }
  else if (level>=20 && level<30){ //level 20 to 30: multiply (20 to 25: 1 digit; 25 to 30: 1 or 2 digits)
    operation = 3;
  }
  else if (level > 30){// level 30+: random operation
    operation = random(1,3);
  }
}
