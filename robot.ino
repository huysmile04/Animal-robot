#include "PS2X_lib.h"
#include <Wire.h>
#include <Servo.h>

#define PS2_DAT 6
#define PS2_CMD 5
#define PS2_CLK 4
#define PS2_ATT 3

#define pressures true
#define rumble false

PS2X ps2x_lib;

int error  = 0;
byte type = 0;
byte vibrate = 0;

int lx = 0;
int ly = 0;
int L_Speed = 0;
int R_Speed = 0;

Servo myservo1, myservo2, myservo3, myservo4;
const int SERVOS = 4;

const int ACC = 10;
int value[SERVOS], idle[SERVOS], currentAngle[SERVOS], MIN[SERVOS], MAX[SERVOS], INITANGLE[SERVOS], previousAngle[SERVOS], ANA[SERVOS];
int Left_motor = 8;
int Left_motor_pwm = 9;
int Right_motor_pwm = 10;
int Right_motor = 11;
int enA = A3;
int enB = A4;
int speedy = 255;

void setup() {
  Serial.begin(57600);

  myservo1.attach(2);
  myservo2.attach(7);
  myservo3.attach(12);
  myservo4.attach(13);

  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);

  stop();

  MIN[0] = 90;
  MAX[0] = 25;
  INITANGLE[0] = 90;


  MIN[1] = 25;
  MAX[1] = 90;
  INITANGLE[1] = 90;

  MIN[2] = 0;
  MAX[2] = 90;
  INITANGLE[2] = 90;

  MIN[3] = 0;
  MAX[3] = 90;
  INITANGLE[3] = 90;

  myservo1.write(INITANGLE[0]);
  myservo2.write(INITANGLE[1]);
  myservo3.write(INITANGLE[2]);
  myservo4.write(INITANGLE[3]);

  currentAngle[0] = INITANGLE[0];
  currentAngle[1] = INITANGLE[1];
  currentAngle[2] = INITANGLE[2];
  currentAngle[3] = INITANGLE[3];

  delay(2000);
  Serial.print("Search Controller..");

  do {
    error = ps2x_lib.config_gamepad(PS2_CLK, PS2_CMD, PS2_ATT, PS2_DAT, pressures, rumble);
    if (error == 0) {
      Serial.println("\nConfigured successful ");
      break;
    } else {
      Serial.print(".");
      delay(100);
    }
  } while (1);
  type = ps2x_lib.readType();
  switch (type) {
    case 0:
      Serial.println("Unknown Controller type found ");
      break;
    case 1:
      Serial.println("DualShock Controller found ");
      break;
    case 2:
      Serial.println("GuitarHero Controller found ");
      break;
    case 3:
      Serial.println("Wireless Sony DualShock Controller found ");
      break;
  }
  ps2x_lib.read_gamepad(true, 200);
  delay(500);
  ps2x_lib.read_gamepad(false, 200);
  delay(300);
  ps2x_lib.read_gamepad(true, 200);
  delay(500);
}

void loop() {
  ps2x_lib.read_gamepad(false, vibrate);

  if (ps2x_lib.Button(PSB_START)) {
    Serial.println("Start is being held");
    ps2x_lib.read_gamepad(true, 200);
  }
  if (ps2x_lib.Button(PSB_SELECT))
    Serial.println("Select is being held");
  if (ps2x_lib.Button(PSB_PAD_UP))
  {
    Serial.print("Up held this hard: ");
    Serial.println(ps2x_lib.Analog(PSAB_PAD_UP), DEC);
    forward();

  } else if (ps2x_lib.ButtonReleased(PSB_PAD_UP)) {
    stop();
  }
  if (ps2x_lib.Button(PSB_PAD_DOWN))
  {
    Serial.print("DOWN held this hard: ");
    Serial.println(ps2x_lib.Analog(PSAB_PAD_DOWN), DEC);
    back();
  } else if (ps2x_lib.ButtonReleased(PSB_PAD_DOWN))
  {
    stop();
  }
  if (ps2x_lib.Button(PSB_R1))
     {
     Serial.println("R2 pressed");
      myservo1.write(90);
     }
  if (ps2x_lib.Button(PSB_L1)) {
    myservo4.write(0);
  }
  if (ps2x_lib.Button(PSB_PAD_LEFT)){
    Serial.println(ps2x_lib.Analog(PSB_PAD_LEFT));
    right();
  } else if (ps2x_lib.ButtonReleased(PSB_PAD_LEFT))
  {
    stop();
  }
  if (ps2x_lib.Button(PSB_PAD_RIGHT)) {
    Serial.println(ps2x_lib.Analog(PSB_PAD_RIGHT));
    left();
  } else if (ps2x_lib.ButtonReleased(PSB_PAD_RIGHT)) {

    stop();
  }
  if (ps2x_lib.Button(PSB_R3)) {
    Serial.println(ps2x_lib.Analog(PSB_R3));
    full();
  } else if (ps2x_lib.ButtonReleased(PSB_R3)) {

    stop();
  }
  vibrate = ps2x_lib.Analog(PSAB_CROSS);

  if (ps2x_lib.NewButtonState()) {
    if (ps2x_lib.Button(PSB_L3))
      Serial.println("L3 pressed");
    if (ps2x_lib.Button(PSB_L2)) 
    {
      Serial.println("L2 pressed");
      myservo4.write(90);
    }
    if (ps2x_lib.Button(PSB_R2))
     {
     Serial.println("R2 pressed");
      myservo1.write(25);
     }
    if (ps2x_lib.Button(PSB_TRIANGLE)) 
    {
      Serial.println("Triangle pressed");
      myservo1.write(90);    
    }
  }
  if (ps2x_lib.ButtonPressed(PSB_CIRCLE)) 
  {
    Serial.println("Circle just pressed");
    myservo4.write(90);          
  }
  if (ps2x_lib.NewButtonState(PSB_CROSS)) 
  {
    Serial.println("X just changed");
    myservo1.write(25);   
  }
  if (ps2x_lib.ButtonPressed(PSB_SQUARE)) 
  {
    Serial.println("Square just released");
    myservo4.write(0);              
  }
  if (ps2x_lib.Button(PSB_L1) || ps2x_lib.Button(PSB_R1)) {
    Serial.print("Stick Values:");
    Serial.print(ps2x_lib.Analog(PSS_LY), DEC);
    Serial.print(",");
    Serial.print(ps2x_lib.Analog(PSS_LX), DEC);
    Serial.print(",");
    Serial.print(ps2x_lib.Analog(PSS_RY), DEC);
    Serial.print(",");
    Serial.println(ps2x_lib.Analog(PSS_RX), DEC);
    Serial.print(",");
  }

  value[0] = ps2x_lib.Analog(PSS_RY);
  value[1] = ps2x_lib.Analog(PSS_LX);
  value[2] = ps2x_lib.Analog(PSS_LY);
  value[3] = ps2x_lib.Analog(PSS_RX);

  for (int i = 0; i < SERVOS; i++) {
    if (value[i] > 130) {
      if (currentAngle[i] < MAX[i])
        currentAngle[i] += 1;
      switch (i) {
        case 0: myservo1.write(currentAngle[i]); break;
        case 1: myservo2.write(currentAngle[i]); break;
        case 2: myservo3.write(currentAngle[i]); break;
        case 3: myservo4.write(currentAngle[i]); break;
      }
    } else if (value[i] < 120) {
      if (currentAngle[i] > MIN[i]) currentAngle[i] -= 1;
      switch (i) {
        case 0: myservo1.write(currentAngle[i]); break;
        case 1: myservo2.write(currentAngle[i]); break;
        case 2: myservo3.write(currentAngle[i]); break;
        case 3: myservo4.write(currentAngle[i]); break;
      }
    }
  }
  delay(10);
}

void openGripper() {
  myservo1.write(MIN[0]);
  delay(300);
}
void closeGripper() {
  myservo1.write(MAX[90]);
  delay(300);
}

void forward() {
  analogWrite(A3, 220);
  digitalWrite(8, HIGH);
  digitalWrite(9, LOW);
  analogWrite(A4, 220);
  digitalWrite(11, LOW);
  digitalWrite(10, HIGH);
}
void right() {
  analogWrite(A3, 200);
  digitalWrite(8, LOW);
  digitalWrite(9, HIGH);
  analogWrite(A4, 130);
  digitalWrite(11, LOW);
  digitalWrite(10, HIGH);
}
void back() {
  digitalWrite(8, LOW);
  digitalWrite(9, HIGH);

  digitalWrite(11, HIGH);
  digitalWrite(10, LOW);
}

void left() {
  analogWrite(A3, 130);
  digitalWrite(8, HIGH);
  digitalWrite(9, LOW);
  analogWrite(A4, 200);
  digitalWrite(11, HIGH);
  digitalWrite(10, LOW);
}
void full() {
  analogWrite(A3, 255);
  digitalWrite(8, HIGH);
  digitalWrite(9, LOW);
  analogWrite(A4, 255);
  digitalWrite(11, LOW);
  digitalWrite(10, HIGH);
}
void stop() {
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);
  digitalWrite(11, LOW);
  digitalWrite(10, LOW);
}
