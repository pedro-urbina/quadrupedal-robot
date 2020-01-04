// Pedro Urbina
// ROBO 380 - Mechatronics
// Spring 2019

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <IRremote.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// IR SENSOR //
const int RECV_PIN = 7;
IRrecv irrecv(RECV_PIN);
decode_results results;

// A IS RIGHT-REAR LEG
// B IS RIGHT-FRONT LEG
// C IS LEFT-FRONT LEG
// D IS LEFT-REAR LEG


// SERVO NUMBER ASSIGNMENTS //
const int hipA = 6;
const int kneeA = 8;
const int ankleA = 1;
const int hipB = 5;
const int kneeB = 7;
const int ankleB = 10;

const int hipC = 4;
const int kneeC = 3;
const int ankleC = 2;

const int hipD = 11;
const int kneeD = 0;
const int ankleD = 9;

// SERVO ZERO POSITIONS //
const float hipZeroA = 310;
const float kneeZeroA = 315;
const float ankleZeroA = 307.5; 

const float hipZeroB = 325;
const float kneeZeroB = 310;
const float ankleZeroB = 320; 

const float hipZeroC = 332.5;
const float kneeZeroC = 304.5;
const float ankleZeroC = 312.5; 

const float hipZeroD = 325;
const float kneeZeroD = 327.5;
const float ankleZeroD = 307.5; 

// SERVO pulseCalc/ANGLE FACTORS //
const float hipFactorA = 2.39;
const float kneeFactorA = 2.44;
const float ankleFactorA = 2.42;

const float hipFactorB = 2.44;
const float kneeFactorB = 2.44;
const float ankleFactorB = 2.44;

const float hipFactorC = 2.42;
const float kneeFactorC = 2.38;
const float ankleFactorC = 2.47;

const float hipFactorD = 2.39;
const float kneeFactorD = 2.47;
const float ankleFactorD = 2.42;

// SERVO SETUP ANGLES
int kneeInitial = 40;
int hipInitial =0;
int ankleInitial = 35;

int kneeStretch = -90;
int hipStretch = 60;
int ankleStretch = -90;

int hipOffset1 = 30;
int hipOffset2 = 15;
int hipOffset3 = -15;
int hipOffset4 = -30;

int kneeUp = 30;
int kneeDown = 0;

int ankleOffset1 = 0;
int ankleOffset2 = 0;
int ankleOffset3 = 0;
int ankleOffset4 = 0;

float lastKneeA;
float lastHipA;
float lastAnkleA;
float lastKneeB;
float lastHipB;
float lastAnkleB;
float lastKneeC;
float lastHipC;
float lastAnkleC;
float lastKneeD;
float lastHipD;
float lastAnkleD;

// CONTROL CONSTANTS //
const int servoDelay = 50;
int servoDelay2 = 2;
const int delay2 = 10;
int mode = 0;

char rx_byte = 0;
String rx_str = "";
boolean not_number = false;
int result;

float pulseCalc = 0;

void setup() {
  Serial.begin(9600);
  pwm.begin();
  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates
  irrecv.enableIRIn();
  irrecv.blink13(true);
  stretch();
  delay(500);
}

void loop() {
  // DECLARE MODE BASED ON IR SENSOR INPUT //
  if (irrecv.decode(&results)){
        Serial.println(results.value, HEX);
        if (results.value == 16753245) {        // DO NOTHING - REMOTE POWER BUTTON
          mode = 0;
          Serial.print("Mode = ");
          Serial.println(mode);
          }
        else if (results.value == 16718055) {   // MOVE FORWARD - REMOTE BUTTON 2
          mode = 1;
          Serial.print("Mode = ");
          Serial.println(mode);
          }
        else if (results.value == 16726215) {   // MOVE BACKWARD - REMOTE BUTTON 5
          mode = 2;
          }
        else if (results.value == 16734885) {   // STRAFE RIGHT - REMOTE BUTTON 6
          mode = 3;
          }
        else if (results.value == 16716015) {   // STRAFE LEFT - REMOTE BUTTON 4
          mode = 4;
          }
        else if (results.value == 16761405) {   // Reduce servoDelay2 - Remote Button FF
          if (servoDelay2 > 0) {
            servoDelay2--; 
          }
        }
        else if (results.value == 16720605) {   // Increase servoDelay2 - Remote Button REW
          if (servoDelay2 < 2) {
            servoDelay2++; 
          }
        }
        irrecv.resume();
          }          
// CALL FUNCTION FOR CURRENT MODE //
  if (mode == 0){
    stand();
  }
  else if (mode == 1){
    forward();
  }
  else if (mode == 2){
    backward();
  }
  else if (mode == 3){
    strafe_right();
  }
  else if (mode == 4){
    strafe_left();
  }
  
  if (Serial.available() > 0) {    // is a character available?
    rx_byte = Serial.read();       // get the character

    if ((rx_byte >= '0') && (rx_byte <= '9')) {
      rx_str += rx_byte;
    }
    else if (rx_byte == '\n') {
      // end of string
      if (not_number) {
        Serial.println("Not a number");
      }
      else {
        // print the result
        if (rx_str.toInt() == 1) {
          mode = 1;          
        }
        if (rx_str.toInt() == 0) {
          mode = 0;
        }
        Serial.print(rx_str);
        Serial.println("");
        Serial.print("Mode = ");
        Serial.println(mode);
        delay(100);
        }
        not_number = false;         // reset flag
        rx_str = "";                // clear the string for reuse
      }
    else {
      // non-number character received
      not_number = true;    // flag a non-number
    }
    if (mode == 1) {
      forward();
    }
}
}

float pulseServo;

void Set_Servo_Position(char leg, int kneeUp, int hipAngle, int ankleAngle, int kneeDown)  {
  if (leg == 'A') {
    pulseCalc = kneeZeroA+(kneeInitial+kneeUp)*kneeFactorA;
      if (pulseCalc > lastKneeA) {
        for(pulseServo = lastKneeA; pulseServo < pulseCalc; pulseServo++) {
          pwm.setPWM(kneeA,0,pulseServo);
          delay(servoDelay2);
        }
      }
      else if (pulseCalc < lastKneeA) {
        for(pulseServo = lastKneeA; pulseServo > pulseCalc; pulseServo--) {
          pwm.setPWM(kneeA,0,pulseServo);
          delay(servoDelay2);
        }
      }
    Serial.println("Knee up");
    Serial.println(pulseCalc);
    lastKneeA = pulseCalc;
    
    pulseCalc = hipZeroA-(hipInitial+hipAngle)*hipFactorA;
    if (pulseCalc > lastHipA) {
        for(pulseServo = lastHipA; pulseServo < pulseCalc; pulseServo++) {
          pwm.setPWM(hipA,0,pulseServo);
          delay(servoDelay2);
        }
      }
      else if (pulseCalc < lastHipA) {
        for(pulseServo = lastHipA; pulseServo > pulseCalc; pulseServo--) {
          pwm.setPWM(hipA,0,pulseServo);
          delay(servoDelay2);
        }
      }
    Serial.println("Hip up");
    Serial.println(pulseCalc);
    lastHipA = pulseCalc;
    
    pulseCalc = ankleZeroA+(ankleInitial+ankleAngle)*ankleFactorA;
    if (pulseCalc > lastAnkleA) {
        for(pulseServo = lastAnkleA; pulseServo < pulseCalc; pulseServo++) {
          pwm.setPWM(ankleA,0,pulseServo);
          delay(servoDelay2);
        }
      }
      else if (pulseCalc < lastAnkleA) {
        for(pulseServo = lastAnkleA; pulseServo > pulseCalc; pulseServo--) {
          pwm.setPWM(ankleA,0,pulseServo);
          delay(servoDelay2);
        }
      }
    Serial.println("Ankle up");
    Serial.println(pulseCalc);
    lastAnkleA = pulseCalc;
    
    pulseCalc = kneeZeroA+(kneeInitial+kneeDown)*kneeFactorA;
    if (pulseCalc > lastKneeA) {
        for(pulseServo = lastKneeA; pulseServo < pulseCalc; pulseServo++) {
          pwm.setPWM(kneeA,0,pulseServo);
          delay(servoDelay2);
        }
      }
      else if (pulseCalc < lastKneeA) {
        for(pulseServo = lastKneeA; pulseServo > pulseCalc; pulseServo--) {
          pwm.setPWM(kneeA,0,pulseServo);
          delay(servoDelay2);
        }
      }
    Serial.println("Knee up");
    Serial.println(pulseCalc);
    lastKneeA = pulseCalc;
  }
  
  else if (leg == 'B') {
    pulseCalc = kneeZeroB+(kneeInitial+kneeUp)*kneeFactorB;
      if (pulseCalc > lastKneeB) {
        for(pulseServo = lastKneeB; pulseServo < pulseCalc; pulseServo++) {
          pwm.setPWM(kneeB,0,pulseServo);
          delay(servoDelay2);
        }
      }
      else if (pulseCalc < lastKneeB) {
        for(pulseServo = lastKneeB; pulseServo > pulseCalc; pulseServo--) {
          pwm.setPWM(kneeB,0,pulseServo);
          delay(servoDelay2);
        }
      }
    Serial.println("Knee up");
    Serial.println(pulseCalc);
    lastKneeB = pulseCalc;
    
    pulseCalc = hipZeroB-(hipInitial+hipAngle)*hipFactorB;
    if (pulseCalc > lastHipB) {
        for(pulseServo = lastHipB; pulseServo < pulseCalc; pulseServo++) {
          pwm.setPWM(hipB,0,pulseServo);
          delay(servoDelay2);
        }
      }
      else if (pulseCalc < lastHipB) {
        for(pulseServo = lastHipB; pulseServo > pulseCalc; pulseServo--) {
          pwm.setPWM(hipB,0,pulseServo);
          delay(servoDelay2);
        }
      }
    Serial.println("Hip up");
    Serial.println(pulseCalc);
    lastHipB = pulseCalc;
    
    pulseCalc = ankleZeroB+(ankleInitial+ankleAngle)*ankleFactorB;
    if (pulseCalc > lastAnkleB) {
        for(pulseServo = lastAnkleB; pulseServo < pulseCalc; pulseServo++) {
          pwm.setPWM(ankleB,0,pulseServo);
          delay(servoDelay2);
        }
      }
      else if (pulseCalc < lastAnkleB) {
        for(pulseServo = lastAnkleB; pulseServo > pulseCalc; pulseServo--) {
          pwm.setPWM(ankleB,0,pulseServo);
          delay(servoDelay2);
        }
      }
    Serial.println("Ankle up");
    Serial.println(pulseCalc);
    lastAnkleB = pulseCalc;
    
    pulseCalc = kneeZeroB+(kneeInitial+kneeDown)*kneeFactorB;
    if (pulseCalc > lastKneeB) {
        for(pulseServo = lastKneeB; pulseServo < pulseCalc; pulseServo++) {
          pwm.setPWM(kneeB,0,pulseServo);
          delay(servoDelay2);
        }
      }
      else if (pulseCalc < lastKneeB) {
        for(pulseServo = lastKneeB; pulseServo > pulseCalc; pulseServo--) {
          pwm.setPWM(kneeB,0,pulseServo);
          delay(servoDelay2);
        }
      }
    Serial.println("Knee up");
    Serial.println(pulseCalc);
    lastKneeB = pulseCalc;
  }
  else if (leg == 'C') {
    pulseCalc = kneeZeroC+(kneeInitial+kneeUp)*kneeFactorC;
      if (pulseCalc > lastKneeC) {
        for(pulseServo = lastKneeC; pulseServo < pulseCalc; pulseServo++) {
          pwm.setPWM(kneeC,0,pulseServo);
          delay(servoDelay2);
        }
      }
      else if (pulseCalc < lastKneeC) {
        for(pulseServo = lastKneeC; pulseServo > pulseCalc; pulseServo--) {
          pwm.setPWM(kneeC,0,pulseServo);
          delay(servoDelay2);
        }
      }
    Serial.println("Knee up");
    Serial.println(pulseCalc);
    lastKneeC = pulseCalc;
    
    pulseCalc = hipZeroC+(hipInitial+hipAngle)*hipFactorC;
    if (pulseCalc > lastHipC) {
        for(pulseServo = lastHipC; pulseServo < pulseCalc; pulseServo++) {
          pwm.setPWM(hipC,0,pulseServo);
          delay(servoDelay2);
        }
      }
      else if (pulseCalc < lastHipC) {
        for(pulseServo = lastHipC; pulseServo > pulseCalc; pulseServo--) {
          pwm.setPWM(hipC,0,pulseServo);
          delay(servoDelay2);
        }
      }
    Serial.println("Hip up");
    Serial.println(pulseCalc);
    lastHipC = pulseCalc;
    
    pulseCalc = ankleZeroC+(ankleInitial+ankleAngle)*ankleFactorC;
    if (pulseCalc > lastAnkleC) {
        for(pulseServo = lastAnkleC; pulseServo < pulseCalc; pulseServo++) {
          pwm.setPWM(ankleC,0,pulseServo);
          delay(servoDelay2);
        }
      }
      else if (pulseCalc < lastAnkleC) {
        for(pulseServo = lastAnkleC; pulseServo > pulseCalc; pulseServo--) {
          pwm.setPWM(ankleC,0,pulseServo);
          delay(servoDelay2);
        }
      }
    Serial.println("Ankle up");
    Serial.println(pulseCalc);
    lastAnkleC = pulseCalc;
    
    pulseCalc = kneeZeroC+(kneeInitial+kneeDown)*kneeFactorC;
    if (pulseCalc > lastKneeC) {
        for(pulseServo = lastKneeC; pulseServo < pulseCalc; pulseServo++) {
          pwm.setPWM(kneeC,0,pulseServo);
          delay(servoDelay2);
        }
      }
      else if (pulseCalc < lastKneeC) {
        for(pulseServo = lastKneeC; pulseServo > pulseCalc; pulseServo--) {
          pwm.setPWM(kneeC,0,pulseServo);
          delay(servoDelay2);
        }
      }
    Serial.println("Knee up");
    Serial.println(pulseCalc);
    lastKneeC = pulseCalc;
  }
  
  else if (leg == 'D') {
    pulseCalc = kneeZeroD+(kneeInitial+kneeUp)*kneeFactorD;
      if (pulseCalc > lastKneeD) {
        for(pulseServo = lastKneeD; pulseServo < pulseCalc; pulseServo++) {
          pwm.setPWM(kneeD,0,pulseServo);
          delay(servoDelay2);
        }
      }
      else if (pulseCalc < lastKneeD) {
        for(pulseServo = lastKneeD; pulseServo > pulseCalc; pulseServo--) {
          pwm.setPWM(kneeD,0,pulseServo);
          delay(servoDelay2);
        }
      }
    Serial.println("Knee up");
    Serial.println(pulseCalc);
    lastKneeD = pulseCalc;
    
    pulseCalc = hipZeroD+(hipInitial+hipAngle)*hipFactorD;
    if (pulseCalc > lastHipD) {
        for(pulseServo = lastHipD; pulseServo < pulseCalc; pulseServo++) {
          pwm.setPWM(hipD,0,pulseServo);
          delay(servoDelay2);
        }
      }
      else if (pulseCalc < lastHipD) {
        for(pulseServo = lastHipD; pulseServo > pulseCalc; pulseServo--) {
          pwm.setPWM(hipD,0,pulseServo);
          delay(servoDelay2);
        }
      }
    Serial.println("Hip up");
    Serial.println(pulseCalc);
    lastHipD = pulseCalc;
    
    pulseCalc = ankleZeroD+(ankleInitial+ankleAngle)*ankleFactorD;
    if (pulseCalc > lastAnkleD) {
        for(pulseServo = lastAnkleD; pulseServo < pulseCalc; pulseServo++) {
          pwm.setPWM(ankleD,0,pulseServo);
          delay(servoDelay2);
        }
      }
      else if (pulseCalc < lastAnkleD) {
        for(pulseServo = lastAnkleD; pulseServo > pulseCalc; pulseServo--) {
          pwm.setPWM(ankleD,0,pulseServo);
          delay(servoDelay2);
        }
      }
    Serial.println("Ankle up");
    Serial.println(pulseCalc);
    lastAnkleD = pulseCalc;
    
    pulseCalc = kneeZeroD+(kneeInitial+kneeDown)*kneeFactorD;
    if (pulseCalc > lastKneeD) {
        for(pulseServo = lastKneeD; pulseServo < pulseCalc; pulseServo++) {
          pwm.setPWM(kneeD,0,pulseServo);
          delay(servoDelay2);
        }
      }
      else if (pulseCalc < lastKneeD) {
        for(pulseServo = lastKneeD; pulseServo > pulseCalc; pulseServo--) {
          pwm.setPWM(kneeD,0,pulseServo);
          delay(servoDelay2);
        }
      }
    Serial.println("Knee up");
    Serial.println(pulseCalc);
    lastKneeD = pulseCalc;
  }
}

void stretch() {
  pulseCalc = kneeZeroA+kneeStretch*kneeFactorA;
    pwm.setPWM(kneeA,0,pulseCalc);
    Serial.println("Knee A ready");
    Serial.println(pulseCalc);
    lastKneeA = pulseCalc;
    delay(servoDelay);
    pulseCalc = ankleZeroA+ankleStretch*ankleFactorA;
    pwm.setPWM(ankleA,0,pulseCalc);
    Serial.println("Ankle A ready");
    Serial.println(pulseCalc);
    lastAnkleA = pulseCalc;
    delay(servoDelay);

    pulseCalc = kneeZeroB+kneeStretch*kneeFactorB;
    pwm.setPWM(kneeB,0,pulseCalc);
    Serial.println("Knee B ready");
    Serial.println(pulseCalc);
    lastKneeB = pulseCalc;
    delay(servoDelay);
    pulseCalc = ankleZeroB+ankleStretch*ankleFactorB;
    pwm.setPWM(ankleB,0,pulseCalc);
    Serial.println("Ankle B ready");
    Serial.println(pulseCalc);
    lastAnkleB = pulseCalc;
    delay(servoDelay);

    pulseCalc = kneeZeroC+kneeStretch*kneeFactorC;
    pwm.setPWM(kneeC,0,pulseCalc);
    Serial.println("Knee C ready");
    Serial.println(pulseCalc);
    lastKneeB = pulseCalc;
    delay(servoDelay);
    pulseCalc = ankleZeroC+ankleStretch*ankleFactorC;
    pwm.setPWM(ankleC,0,pulseCalc);
    Serial.println("Ankle C ready");
    Serial.println(pulseCalc);
    lastAnkleC = pulseCalc;
    delay(servoDelay);

    pulseCalc = kneeZeroD+kneeStretch*kneeFactorD;
    pwm.setPWM(kneeD,0,pulseCalc);
    Serial.println("Knee D ready");
    Serial.println(pulseCalc);
    lastKneeB = pulseCalc;
    delay(servoDelay);
    pulseCalc = ankleZeroD+ankleStretch*ankleFactorD;
    pwm.setPWM(ankleD,0,pulseCalc);
    Serial.println("Ankle D ready");
    Serial.println(pulseCalc);
    lastAnkleD = pulseCalc;
    delay(servoDelay);

    delay(500);

    pulseCalc = hipZeroA-hipStretch*hipFactorA;
    pwm.setPWM(hipA,0,pulseCalc);
    Serial.println("Hip A ready");
    Serial.println(pulseCalc);
    lastHipA = pulseCalc;
    delay(servoDelay);

    pulseCalc = hipZeroB-hipStretch*hipFactorB;
    pwm.setPWM(hipB,0,pulseCalc);
    Serial.println("Hip B ready");
    Serial.println(pulseCalc);
    lastHipB = pulseCalc;
    delay(servoDelay);

    pulseCalc = hipZeroC-hipStretch*hipFactorC;
    pwm.setPWM(hipC,0,pulseCalc);
    Serial.println("Hip C ready");
    Serial.println(pulseCalc);
    lastHipC = pulseCalc;
    delay(servoDelay);

    pulseCalc = hipZeroD-hipStretch*hipFactorD;
    pwm.setPWM(hipD,0,pulseCalc);
    Serial.println("Hip D ready");
    Serial.println(pulseCalc);
    lastHipD = pulseCalc;
    delay(servoDelay);

    delay(500);

    pulseCalc = hipZeroA+hipStretch*hipFactorA;
    pwm.setPWM(hipA,0,pulseCalc);
    Serial.println("Hip A ready");
    Serial.println(pulseCalc);
    lastHipA = pulseCalc;
    delay(servoDelay);

    pulseCalc = hipZeroB+hipStretch*hipFactorB;
    pwm.setPWM(hipB,0,pulseCalc);
    Serial.println("Hip B ready");
    Serial.println(pulseCalc);
    lastHipB = pulseCalc;
    delay(servoDelay);

    pulseCalc = hipZeroC+hipStretch*hipFactorC;
    pwm.setPWM(hipC,0,pulseCalc);
    Serial.println("Hip C ready");
    Serial.println(pulseCalc);
    lastHipC = pulseCalc;
    delay(servoDelay);

    pulseCalc = hipZeroD+hipStretch*hipFactorD;
    pwm.setPWM(hipD,0,pulseCalc);
    Serial.println("Hip D ready");
    Serial.println(pulseCalc);
    lastHipD = pulseCalc;
    delay(servoDelay);
}
void stand() {
    pulseCalc = kneeZeroA+kneeInitial*kneeFactorA;
    pwm.setPWM(kneeA,0,pulseCalc);
    Serial.println("Knee A ready");
    Serial.println(pulseCalc);
    lastKneeA = pulseCalc;
    delay(servoDelay);
    pulseCalc = hipZeroA-hipInitial*hipFactorA;
    pwm.setPWM(hipA,0,pulseCalc);
    Serial.println("Hip A ready");
    Serial.println(pulseCalc);
    lastHipA = pulseCalc;
    delay(servoDelay);
    pulseCalc = ankleZeroA+ankleInitial*ankleFactorA;
    pwm.setPWM(ankleA,0,pulseCalc);
    Serial.println("Ankle A ready");
    Serial.println(pulseCalc);
    lastAnkleA = pulseCalc;
    delay(servoDelay);

    pulseCalc = kneeZeroB+kneeInitial*kneeFactorB;
    pwm.setPWM(kneeB,0,pulseCalc);
    Serial.println("Knee B ready");
    Serial.println(pulseCalc);
    lastKneeB = pulseCalc;
    delay(servoDelay);
    pulseCalc = hipZeroB-hipInitial*hipFactorB;
    pwm.setPWM(hipB,0,pulseCalc);
    Serial.println("Hip B ready");
    Serial.println(pulseCalc);
    lastHipB = pulseCalc;
    delay(servoDelay);
    pulseCalc = ankleZeroB+ankleInitial*ankleFactorB;
    pwm.setPWM(ankleB,0,pulseCalc);
    Serial.println("Ankle B ready");
    Serial.println(pulseCalc);
    lastAnkleB = pulseCalc;
    delay(servoDelay);

    pulseCalc = kneeZeroC+kneeInitial*kneeFactorC;
    pwm.setPWM(kneeC,0,pulseCalc);
    Serial.println("Knee C ready");
    Serial.println(pulseCalc);
    lastKneeC = pulseCalc;
    delay(servoDelay);
    pulseCalc = hipZeroC+hipInitial*hipFactorC;
    pwm.setPWM(hipC,0,pulseCalc);
    Serial.println("Hip C ready");
    Serial.println(pulseCalc);
    lastHipC = pulseCalc;
    delay(servoDelay);
    pulseCalc = ankleZeroC+ankleInitial*ankleFactorC;
    pwm.setPWM(ankleC,0,pulseCalc);
    Serial.println("Ankle C ready");
    Serial.println(pulseCalc);
    lastAnkleC = pulseCalc;
    delay(servoDelay);

    pulseCalc = kneeZeroD+kneeInitial*kneeFactorD;
    pwm.setPWM(kneeD,0,pulseCalc);
    Serial.println("Knee D ready");
    Serial.println(pulseCalc);
    lastKneeD = pulseCalc;
    delay(servoDelay);
    pulseCalc = hipZeroD+hipInitial*hipFactorD;
    pwm.setPWM(hipD,0,pulseCalc);
    Serial.println("Hip D ready");
    Serial.println(pulseCalc);
    lastHipD = pulseCalc;
    delay(servoDelay);
    pulseCalc = ankleZeroD+ankleInitial*ankleFactorD;
    pwm.setPWM(ankleD,0,pulseCalc);
    Serial.println("Ankle D ready");
    Serial.println(pulseCalc);
    lastAnkleD = pulseCalc;
    delay(servoDelay);
}
void forward() {
//STARTING POSITION
Set_Servo_Position('A',kneeDown,hipOffset1,ankleOffset1,kneeDown);   
Set_Servo_Position('B',kneeDown,hipOffset4,ankleOffset1,kneeDown);   
Set_Servo_Position('C',kneeDown,hipOffset2,ankleOffset1,kneeDown);  
Set_Servo_Position('D',kneeDown,hipOffset3,ankleOffset1,kneeDown);    

//STEP 1
Set_Servo_Position('B',kneeUp,hipOffset1,ankleOffset1,kneeDown);

//SHIFT1
Set_Servo_Position('A',kneeDown,hipOffset2,ankleOffset1,kneeDown);
//Set_Servo_Position('B',kneeDown,hipOffset1,ankleOffset1,kneeDown);
Set_Servo_Position('C',kneeDown,hipOffset3,ankleOffset1,kneeDown);
Set_Servo_Position('D',kneeDown,hipOffset4,ankleOffset1,kneeDown);

//STEP 2
Set_Servo_Position('D',kneeUp,hipOffset1,ankleOffset1,kneeDown);

//SHIFT2
Set_Servo_Position('A',kneeDown,hipOffset3,ankleOffset1,kneeDown);
Set_Servo_Position('B',kneeDown,hipOffset2,ankleOffset1,kneeDown);
Set_Servo_Position('C',kneeDown,hipOffset4,ankleOffset1,kneeDown);
//Set_Servo_Position('D',kneeDown,hipOffset1,ankleOffset1,kneeDown);

//STEP 3
Set_Servo_Position('C',kneeUp,hipOffset1,ankleOffset1,kneeDown);

//SHIFT3
Set_Servo_Position('A',kneeDown,hipOffset4,ankleOffset1,kneeDown);
Set_Servo_Position('B',kneeDown,hipOffset3,ankleOffset1,kneeDown);
//Set_Servo_Position('C',kneeDown,hipOffset1,ankleOffset1,kneeDown);
Set_Servo_Position('D',kneeDown,hipOffset2,ankleOffset1,kneeDown);

//STEP 4
Set_Servo_Position('A',kneeUp,hipOffset1,ankleOffset1,kneeDown);
}

void backward() {
//STARTING POSITION
Set_Servo_Position('A',0, 30,0,0);    //RIGHT-REAR
Set_Servo_Position('B',0,-30,0,0);    //RIGHT-FRONT
Set_Servo_Position('C',0, 15,0,0);    //LEFT-FRONT
Set_Servo_Position('D',0,-15,0,0);    //LEFT-REAR

//STEP 4
Set_Servo_Position('A',30,-30,0,0);

//SHIFT3
//Set_Servo_Position('A',0,-30,0,0);
Set_Servo_Position('B',0,-15,0,0);
Set_Servo_Position('C',0, 30,0,0);
Set_Servo_Position('D',0, 15,0,0);

//STEP 3
Set_Servo_Position('C',30,-30,0,0);

//SHIFT2
Set_Servo_Position('A',0,-15,0,0);
Set_Servo_Position('B',0, 15,0,0);
//Set_Servo_Position('C',0,-30,0,0);
Set_Servo_Position('D',0,30,0,0);

//STEP 2
Set_Servo_Position('D',30,-30,0,0);

//SHIFT1
Set_Servo_Position('A',0, 15,0,0);
Set_Servo_Position('B',0,30,0,0);
Set_Servo_Position('C',0,-15,0,0);
//Set_Servo_Position('D',0,-30,0,0);

//STEP 1
Set_Servo_Position('B',30,-30,0,0);
}

void strafe_right() {
//STARTING POSITION
Set_Servo_Position('D',0, 30*-1,0,0);    //RIGHT-REAR  (reverse directions)
Set_Servo_Position('A',0,-30,0,0);    //RIGHT-FRONT 
Set_Servo_Position('B',0, 15*-1,0,0);    //LEFT-FRONT  (reverse directions)
Set_Servo_Position('C',0,-15,0,0);    //LEFT-REAR   

//STEP 1
Set_Servo_Position('A',30,30,0,0);

//SHIFT1
Set_Servo_Position('D',0, 15*-1,0,0);
//Set_Servo_Position('A',0, 30,0,0);
Set_Servo_Position('B',0,-15*-1,0,0);
Set_Servo_Position('C',0,-30,0,0);

//STEP 2
Set_Servo_Position('C',30,30,0,0);

//SHIFT2
Set_Servo_Position('D',0,-15*-1,0,0);
Set_Servo_Position('A',0, 15,0,0);
Set_Servo_Position('B',0,-30*-1,0,0);
//Set_Servo_Position('C',0,30,0,0);

//STEP 3
Set_Servo_Position('B',30,30*-1,0,0);

//SHIFT3
Set_Servo_Position('D',0,-30*-1,0,0);
Set_Servo_Position('A',0,-15,0,0);
//Set_Servo_Position('B',0,30*-1,0,0);
Set_Servo_Position('C',0, 15,0,0);

//STEP 4
Set_Servo_Position('D',30,30*-1,0,0);

//STEP 1
Set_Servo_Position('A',30,30,0,0);
}

void strafe_left() {
//STARTING POSITION
Set_Servo_Position('B',0, 30,0,0);        //RIGHT-REAR
Set_Servo_Position('C',0,-30*-1,0,0);     //RIGHT-FRONT
Set_Servo_Position('D',0, 15,0,0);        //LEFT-FRONT
Set_Servo_Position('A',0,-15*-1,0,0);     //LEFT-REAR

//STEP 1
Set_Servo_Position('C',30,30*-1,0,0);

//SHIFT1
Set_Servo_Position('B',0, 15,0,0);
//Set_Servo_Position('C',0, 30*-1,0,0);
Set_Servo_Position('D',0,-15,0,0);
Set_Servo_Position('A',0,-30*-1,0,0);

//STEP 2
Set_Servo_Position('A',30,30*-1,0,0);

//SHIFT2
Set_Servo_Position('B',0,-15,0,0);
Set_Servo_Position('C',0, 15*-1,0,0);
Set_Servo_Position('D',0,-30,0,0);
//Set_Servo_Position('A',0,30*-1,0,0);

//STEP 3
Set_Servo_Position('D',30,30,0,0);

//SHIFT3
Set_Servo_Position('B',0,-30,0,0);
Set_Servo_Position('C',0,-15*-1,0,0);
//Set_Servo_Position('D',0,30,0,0);
Set_Servo_Position('A',0, 15*-1,0,0);

//STEP 4
Set_Servo_Position('B',30,30,0,0);

//STEP 1
Set_Servo_Position('C',30,30*-1,0,0);
}
