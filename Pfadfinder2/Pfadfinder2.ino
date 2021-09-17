/* Title: Pfadpfinder2
   Created: 08.08.2021
   Author: Jonah
   Comment: A programm to control a stepper motor based on analog input
*/

//parameters:

#define test 0                // Test the Motor
#define enableauxoutput  0    //enable auxilary output (0 = OFF, 1 = Active ON, 2 = Active OFF)
#define triggerspeed   100    //min speed the windmill will need to achieve to start the stepper (0-1024)
#define steps          300    //amount of rotations of motor from "flag down" to "flag up" (27cm = 200 steps = 1 rotation) (+/-) 
#define latchtime     5000    //time in ms the flag will stay up after windmill speed is under threshold


//do not change anything beyond this line

#if enableauxoutput == 1
#define aux(x) digitalWrite(7,x);
#elif enableauxoutput == 2
#define aux(x) digitalWrite(7,!x);
#else
#define aux(x)
#endif

#define sensorpin A0
#define auxpin 7
#define dir1 8
#define pwm1 9
#define dir2 10
#define pwm2 11
#define del 18

int step_number;
int number_of_steps = 200;
bool trigger, lasttrigger;
unsigned long previousMillis = 0;

void setup() {
  Serial.begin(115200);

  pinMode(dir1, OUTPUT);
  pinMode(pwm1, OUTPUT);
  pinMode(dir2, OUTPUT);
  pinMode(pwm2, OUTPUT);

  #if enableauxoutput == 1
  pinMode(auxpin,OUTPUT);
  #endif

  pinMode(sensorpin, INPUT);

  if (test) {
    for (uint8_t i = 0; i < 10; i++) { //test motor
      stepperstep(steps);
      delay(1000);
      stepperstep(-(steps));
      delay(2000);
    }
  }

}

void loop() {
  int val = analogRead(sensorpin);
  Serial.print("value: ");
  Serial.println(val);


  if (val >= triggerspeed) {  //motor speed is reached
    previousMillis = millis();
    Serial.println("Motor Spinning");
  }

  if (millis() - previousMillis <= latchtime) {
    Serial.println("Flag set");
    trigger = true;
  } else {
    //Serial.println("Flag reset");
    trigger = false;
  }


  if (trigger != lasttrigger) {
    if (trigger) {
      Serial.println("Moving Up");
      aux(true);
      stepperstep(steps);
    } else {
      Serial.println("Moving Down");
      stepperstep(-(steps));
      aux(false);
      stepperdeactivate();
    }
  }

  lasttrigger = trigger;

}


void stepperstep(long steps_to_move) {
  int steps_left = abs(steps_to_move);
  int dir;
  if (steps_to_move > 0) {
    dir = 1;
  }
  if (steps_to_move < 0) {
    dir = 0;
  }
  while (steps_left > 0)
  {
    // increment or decrement the step number,
    // depending on direction:
    if (dir == 1)
    {
      step_number++;
      if (step_number == number_of_steps) {
        step_number = 0;
      }
    }
    else
    {
      if (step_number == 0) {
        step_number = number_of_steps;
      }
      step_number--;
    }
    // decrement the steps left:
    steps_left--;
    // step the motor to step number 0, 1, ..., {3 or 10}
    stepoutput(step_number % 4);
    delay(del);
  }
}

void stepoutput( int thisStep) {
  switch (thisStep) {
    case 0 :
      digitalWrite(dir1, LOW);
      digitalWrite(pwm1, LOW);
      digitalWrite(dir2, LOW);
      digitalWrite(pwm2, HIGH);
      break;
    case 3 :
      digitalWrite(dir1, LOW);
      digitalWrite(pwm1, HIGH);
      digitalWrite(dir2, LOW);
      digitalWrite(pwm2, LOW);
      break;
    case 2 :
      digitalWrite(dir1, LOW);
      digitalWrite(pwm1, LOW);
      digitalWrite(dir2, HIGH);
      digitalWrite(pwm2, HIGH);
      break;
    case 1 :
      digitalWrite(dir1, HIGH);
      digitalWrite(pwm1, HIGH);
      digitalWrite(dir2, LOW);
      digitalWrite(pwm2, LOW);
      break;
  }
}

void stepperdeactivate() {
  digitalWrite(dir1, LOW);
  digitalWrite(pwm1, LOW);
  digitalWrite(dir2, LOW);
  digitalWrite(pwm2, LOW);
}
