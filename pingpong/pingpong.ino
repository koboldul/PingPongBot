#include <IRremote.h>

//Define + - as increse of speed instead of 1,2
#define PLAY_STOP 16720605 // |>=
#define IR_CMD_1 16718055 //1
#define IR_CMD_2 16724175 //2

#define IR_CMD_INC 16754775
#define IR_CMD_DEC 16748655
#define IR_CMD_MODE 16736925
#define IR_CMD_FW 16761405
#define IR_CMD_BACK 16712445

/*
*Shooter Motor control TODO 
*/

/*
* Ball bucket spinner motor.
*/
int BUCKET_DIR_PIN = 2; //Direction pin
int BUCKET_STEP_PIN = 3; //Stepper pin
int BUCKET_ENABLE_PIN = 7; //Motor enable pin

int BUCKET_SM1_PIN = 11; // These 1 control the step size: HH-1/8 HL-1/4 LH-1/2 LL-1
int BUCKET_SM2_PIN = 12;

float _rotationDelay = 150;
bool _isStopped = false;
int _stepSize = 1 | 2;
int _rotationCount = 1700;
int _cadence = 10;

/*
* Remote Control
*/
int RC_PIN=9; // define input pin on Arduino
IRrecv _irReceiver(RC_PIN);
decode_results _rcResults;

/*
* Lifecycle
*/ 
void setup(){
  pinMode(BUCKET_DIR_PIN, OUTPUT);
  pinMode(BUCKET_STEP_PIN, OUTPUT);
  pinMode(BUCKET_ENABLE_PIN, OUTPUT);
 
  digitalWrite(BUCKET_ENABLE_PIN, LOW); 

  pinMode(BUCKET_SM1_PIN, OUTPUT);
  pinMode(BUCKET_SM2_PIN, OUTPUT);

  //set step to 1/8 - for some reason low low is not working
 toggleStepSize(HIGH, HIGH);
 
  Serial.begin(9600);

  _irReceiver.enableIRIn();
  
  Serial.println("Setup complete...");
}
 
void loop(){
  if (_irReceiver.decode(&_rcResults)){
    Serial.print("IR Command received: ");
    Serial.println(_rcResults.value, DEC);
      
    processCommands(_rcResults.value);
    _irReceiver.resume(); // Receive the next value
  }

  if (_isStopped){
    return;
  }

  rotateBucket(_rotationCount, _rotationDelay); 
  delay(_cadence);
  
}

/*
* IR Code Handling
*/

void processCommands(int cmdVal){
  switch (cmdVal) {
    case PLAY_STOP: 
      _isStopped = !_isStopped;
      toggleBucketMotor();
      break;
    
    //Speed handling  
    case IR_CMD_INC:
      _rotationDelay -= 10; //increase in speed = decrease in delay
      Serial.println(_rotationDelay);
      break;
    case IR_CMD_DEC:
      _rotationDelay += 10;
      Serial.println(_rotationDelay);
      break;
    
    //Step count
    case IR_CMD_FW:
      _rotationCount += 100; 
      Serial.println(_rotationCount);
      break;
    case IR_CMD_BACK:
      _rotationCount -= 100; 
      Serial.println(_rotationCount);
      break;
      
    //Step size  
    case IR_CMD_MODE:
      _stepSize = (_stepSize + 1) % 4;

      int sm1 = (_stepSize & 1) != 0 ? LOW : HIGH;
      int sm2 = (_stepSize & 2) != 0 ? LOW : HIGH;
      
      toggleStepSize(sm1, sm2);
      Serial.print("Step size is: ");
      Serial.println(_stepSize);
      break;
  }
}

/*
* Stepper motor methods
*
*/
void toggleStepSize(int sm1, int sm2){
    digitalWrite(BUCKET_SM1_PIN, sm1); 
    digitalWrite(BUCKET_SM2_PIN, sm2);
}

void toggleBucketMotor() {
  if (_isStopped){
    digitalWrite(BUCKET_ENABLE_PIN, HIGH);
  }
  else{
    digitalWrite(BUCKET_ENABLE_PIN, LOW);
  }
  Serial.print("The bucket motor is:");
  Serial.println(_isStopped);
}
 
void rotateBucket(int steps, float speed){
  int direction = steps > 0 ? HIGH : LOW;
   steps = abs(steps); 
 
  //digitalWrite(BUCKET_DIR_PIN, direction); 
 
  for (int i = 0; i < steps; i++){
    digitalWrite(BUCKET_STEP_PIN, HIGH);
    delayMicroseconds(speed);
    digitalWrite(BUCKET_STEP_PIN, LOW);
    delayMicroseconds(speed);
  }
}
