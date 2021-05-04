#include <IRremote.h>

//Define + - as increse of speed instead of 1,2
#define PLAY_STOP 16720605 // |>=
#define IR_CMD_1 16718055 //1
#define IR_CMD_2 16724175 //2

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

float _speed = 70;
bool _isStopped=false;

/*
* Remote Control
*/
int rcPin=9; // define input pin on Arduino
IRrecv _irReceiver(rcPin);
decode_results results;

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
  digitalWrite(BUCKET_SM1_PIN, HIGH); 
  digitalWrite(BUCKET_SM2_PIN, HIGH);
 
  Serial.begin(9600);

  _irReceiver.enableIRIn();
  
  Serial.println("Setup complete...");
}
 
void loop(){
  if (_irReceiver.decode(&results)){
    Serial.print("IR Command received: ");
    Serial.println(results.value, DEC);
      
    processCommands(results.value);
    _irReceiver.resume(); // Receive the next value
  }

  if (_isStopped){
    return;
  }
  
  rotate(1700, _speed); 
  delay(200);
  
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
    case IR_CMD_1:
      _speed += 10;
      Serial.println(_speed);
      break;
    case IR_CMD_2:
      _speed -= 10;
      Serial.println(_speed);
      break;
  }
}

/*
* Stepper motor methods
*
*/
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
 
void rotate(int steps, float speed){
  int direction = steps > 0 ? HIGH : LOW;
 
  speed = speed; //Calculating speed
  steps = abs(steps); //Stores the absolute value of the content in 'steps' back into the 'steps' variable
 
  //digitalWrite(BUCKET_DIR_PIN, direction); 
 
  for (int i = 0; i < steps; i++){
    digitalWrite(BUCKET_STEP_PIN, HIGH);
    delayMicroseconds(speed);
    digitalWrite(BUCKET_STEP_PIN, LOW);
    delayMicroseconds(speed);
  }
}
