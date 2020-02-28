// Derived from Andrew Davies sketch on hobby components
// https://forum.hobbycomponents.com/viewtopic.php?t=1747

#define EN 8      /* Enable pin for all stepper outputs */

#define X_DIR 5   /* Direction pin for X axis */
#define X_STEP 2  /* Step pin for X axis */

#define Y_DIR 6   /* Direction pin for Y axis */
#define Y_STEP 3  /* Step pin for Y axis */

#define Z_DIR 7   /* Direction pin for Z axis */
#define Z_STEP 4  /* Step pin for Z axis */

#define A_DIR 13  /* Direction pin for Aux driver. Requires D13 and A-DIR pins to be shorted */
#define A_STEP 12 /* Direction pin for Aux driver. Requires D12 and A-STEP pins to be shorted */

#define X_ENDSTOP 9   /* X axis endstop input pin */
#define Y_ENDSTOP 10  /* Y axis endstop input pin */
#define Z_ENDSTOP 11  /* Z axis endstop input pin */
#define ABORT A0  /* Abort input pin */
#define HOLD A1   /* Hold input pin */
#define RESUME A2 /* Resume input pin */

#define STEP_DELAY 2 /* default step delay */
#define QUARTER_TURN_STEP_COUNT 50

int Count = 0; /* Counter to count number of steps made */
boolean DIRECTION = LOW; /* Rotational direction of stepper motors */
boolean OPEN_DIRECTION = LOW;
boolean CLOSE_DIRECTION = HIGH;
int STEP_CHANNEL = 0; // which channel is currently active

//int incomingByte = 0; // for incoming serial data
char receivedChar;


/* The setup routine runs once when you press reset: */
void setup() 
{                
  /* Initialize serial */
  Serial.begin(9600);
  
  /* Configure the steper drive pins as outputs */ 
  pinMode(EN, OUTPUT); 
  pinMode(X_DIR, OUTPUT); 
  pinMode(X_STEP, OUTPUT); 
  pinMode(Y_DIR, OUTPUT); 
  pinMode(Y_STEP, OUTPUT); 
  pinMode(Z_DIR, OUTPUT); 
  pinMode(Z_STEP, OUTPUT); 
  pinMode(A_DIR, OUTPUT); 
  pinMode(A_STEP, OUTPUT); 
  
  /* Configure the control pins as inputs with pullups */
  pinMode(X_ENDSTOP, INPUT_PULLUP);
  pinMode(Y_ENDSTOP, INPUT_PULLUP);
  pinMode(Z_ENDSTOP, INPUT_PULLUP);

  pinMode(ABORT, INPUT_PULLUP);
  pinMode(HOLD, INPUT_PULLUP);
  pinMode(RESUME, INPUT_PULLUP);

  /* Enable the X, Y, Z & Aux stepper outputs */
  digitalWrite(EN, LOW); //Low to enable
}

void loop() {
  if (Serial.available() > 0) {
    //incomingByte = Serial.read();
    receivedChar= Serial.read();

    // echo out the received char for logging/debugging
    Serial.write(receivedChar);


    //switch(incomingByte) {
    switch(receivedChar) {

      case '1': // open X
        // set the rotation direction
      	digitalWrite(X_DIR, OPEN_DIRECTION);
        // define the stepper that will be activated below
        STEP_CHANNEL = X_STEP;
      break;

      case '2': // close X
        // set the rotation direction
      	digitalWrite(X_DIR, CLOSE_DIRECTION); 
        // define the stepper that will be activated below
        STEP_CHANNEL = X_STEP;
      break;

      case '3': // open Y
        // set the rotation direction
      	digitalWrite(Y_DIR, OPEN_DIRECTION); 
        // define the stepper that will be activated below
        STEP_CHANNEL = Y_STEP;
      break;

      case '4': // close Y
        // set the rotation direction
      	digitalWrite(Y_DIR, CLOSE_DIRECTION); 
        // define the stepper that will be activated below
        STEP_CHANNEL = Y_STEP;
      break;

      case '5': // open Z
        // set the rotation direction
      	digitalWrite(Z_DIR, OPEN_DIRECTION); 
        // define the stepper that will be activated below
        STEP_CHANNEL = Z_STEP;
      break;

      case '6': // close Z
        // set the rotation direction
      	digitalWrite(Z_DIR, CLOSE_DIRECTION); 
        // define the stepper that will be activated below
        STEP_CHANNEL = Z_STEP;
      break;

      case '7': // open A
        // set the rotation direction
      	digitalWrite(A_DIR, OPEN_DIRECTION); 
        // define the stepper that will be activated below
        STEP_CHANNEL = A_STEP;
      break;

      case '8': // close A
        // set the rotation direction
      	digitalWrite(A_DIR, CLOSE_DIRECTION); 
        // define the stepper that will be activated below
        STEP_CHANNEL = A_STEP;
      break;

      default:
        Serial.write('F');
    }

    Serial.write(STEP_CHANNEL);

    for (int i = 1; i <= QUARTER_TURN_STEP_COUNT; i++) {
      //digitalWrite(STEP_CHANNEL, HIGH);
      digitalWrite(X_STEP, HIGH);
      delay(STEP_DELAY);
      //digitalWrite(STEP_CHANNEL, LOW);
      digitalWrite(X_STEP, LOW);
      delay(STEP_DELAY);
    }
  }
}

