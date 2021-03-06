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

#define STEP_DELAY 10 /* default step delay */
#define PHASE_DELAY 1000 /* default step delay */
#define QUARTER_TURN_STEP_COUNT 50

int Count = 0; /* Counter to count number of steps made */
boolean Direction = LOW; /* Rotational direction of stepper motors */

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
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
}



//String incomingString = "unset"; // for incoming serial data
int incomingByte = 0; // for incoming serial data

/*
void setup() {
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
}
*/

void loop() {
  if (Serial.available() > 0) {
    incomingByte = Serial.read();


    if (incomingByte == '1') {
      // do things 
      for (int i = 1; i <= QUARTER_TURN_STEP_COUNT; i++) {
        digitalWrite(X_STEP, HIGH);
        delay(STEP_DELAY);
        digitalWrite(X_STEP, LOW);
        delay(STEP_DELAY);
      }
    }

    //Serial.print(incomingByte, DEC);
    Serial.write(incomingByte);
  }
}
