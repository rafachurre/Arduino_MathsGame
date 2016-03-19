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
const byte COLS = 4; //three columns
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
bool waitResult = false; //flag: el jugador tiene que introducir el resultado
bool pressKey = false; //flag: el jugador tiene que pulsar una tecla cualquiera
int numberOfErrors = 0; //contador: errores cometidos
int level = 1; //contador: nivel alcanzado
int number1 = 0; //primer numero de la operacion
int number2 = 0; //segundo numero de la operacion
int operation = 1; //1='+'; 2='-'; 3='x';
String resultString = ""; //variable que va almacenando los numero que va pulsando

void setup(){
  Serial.begin(9600);

  Serial.println("----------------------------");
  Serial.println("Bienvenido al SumaCaramelo");
  Serial.println("----------------------------");
  Serial.println();

  randomSeed(analogRead(0));
}

void loop(){
  if (!waitResult && !pressKey){ //si no estoy esperando por el resultado o por una tecla cualquiera
    if(numberOfErrors < 3){ // si aun me quedan vidas
      
      //Calculo los numeros y la operacion
      generateOperation();
      generateNumbers();
      
      //imprimo el nivel
      Serial.println("--------");
      Serial.print("Nivel ");
      Serial.println(String(level));
      Serial.println("--------");
      
      //imprimo la operacion
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
      
      //fijo los flags: esperando por resultado
      waitResult = true;
      pressKey = false;
    }
    else{  //si no me quedan vidas
      Serial.println("Ya has fallado 3 veces. Tomate un descanso");
      waitResult = false;
      pressKey = true;
    }
  }
  else{ //si estoy esperando por resultado o por una tecla cualquiera
    char key = keypad.getKey();
    if (key != NO_KEY){//si presiono una tecla
      
      // Si estoy esperando por una tecla cualquiera
      if (pressKey){
        pressKey = false; //fijo el flag: la tecla ya fue pulsada
      }

      // Si estoy esperando el resultado
      else if (key == '*' ||key == '#' ||key == 'A' ||key == 'B' ||key == 'C' ||key == 'D'){ //si presiono una tecla que no sea numero, confirmo el resultado introducido
        Serial.println();
        //Serial.println(resultString); //debugging
        checkResult(resultString);
        resultString = ""; //limpio la variable para la siguiente operacion
      }

      else{ // Mientras introduzca numeros, los almaceno e imprimo
        resultString += key;
        Serial.print(key);
      }
    }
  }
}

/*
 * Comprueba si el resultado es correcto
 */
void checkResult(String resultString){  
  int resultInteger = resultString.toInt();
  int trueResult = 0;

  //calculate trueResult
  switch(operation){ // calcula el resultado correcto dependiendo de la operacion
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
  if(trueResult == resultInteger){ //si es correcto
    Serial.println("Acertaste!!!!"); //lo comunica por pantalla
    waitResult = false; //fija el flag: no esta esperando un resultado
    pressKey = true; //fija el flag: presiona una tecla cualquiera para avanzar
    level += 1; //sube un nivel
  }
  else{ //si es incorrecto
    Serial.println("OOOHH!! Fallaste :(");
    Serial.println("El resultado era: " + String(trueResult));
    Serial.println("Prueba otra vez");
    waitResult = false; //fija el flag: no esta esperando un resultado
    pressKey = true; //fija el flag: presiona una tecla cualquiera para avanzar
    numberOfErrors += 1; //incrementa el contador de errores
  }
}

/*
 * Genera los numeros segun el nivel alcanzado
 */
void generateNumbers(){
  int maxRandom = 10; //en los 10 primeros niveles: solo numeros de 1 digito (sumas y restas)
  if (level>=10 && level<20){ //del nivel 10 al 20: numeros de 1 o 2 digitos (sumas y restas)
    maxRandom = 100;
  }
  else if (level>=20 && level<25){ //del nivel 20 al 25: solo numeros de 1 (multiplicacion)
    maxRandom = 10;
  }
  else if (level>=25){ //del nivel 25 en adelante: numeros de 1 o 2 digitos (del 25 al 30 multiplicaciones; del 30 en adelante, cualquier operacion)
    maxRandom = 100;
  }
  number1 = random(maxRandom); //genera el numero 1
  number2 = random(maxRandom); //genera el numero 2
  
  if(operation == 2){ // si es una resta, evitamos resultados negativos obligando a que number2 < number1
    int temp = min(number1,number2);
    number1 = max(number1, number2);
    number2 = temp;
  }
}

/*
 * Genera la operacion segun el nivel alcanzado
 */
void generateOperation(){
  if(level<5){ //del nivel 1 al 5: suma (1 digito)
    operation = 1;
  }else if (level>=5 && level<10){ //del nivel 5 al 10: resta (1 digito)
    operation = 2;
  }
  else if (level>=10 && level<15){ //del nivel 10 al 15: suma (1 o 2 digitos)
    operation = 1;
  }
  else if (level>=15 && level<20){ //del nivel 15 al 20: resta (1 o 2 digitos)
    operation = 2;
  }
  else if (level>=20 && level<30){ //del nivel 20 al 30: multiplicacion (del 20 al 25: 1 digito; del 25 al 30: 1 o 2 digitos)
    operation = 3;
  }
  else if (level > 30){// a partir del nivel 30: operacion random
    operation = random(1,3);
  }
}
