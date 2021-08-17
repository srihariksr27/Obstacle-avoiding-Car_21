#include<Servo.h>
#include<Stepper.h>
#include<IRremote.h>
#define pwr   1FE48B7
#define mode  1FE58A7
#define play  1FE807F
#define two   1FED827
#define six   1FE708F
#define eight 1FEF00F
#define four  1FE30CF

int spr=2048;
int stepMotspeed=10;
int driveMode=0;
int servoPin=12;
int trigPin=7;
int echoPin=6;
int irPin=5;
int EN=3;      //DC motor speed pin
int IN1=2;     //direction 1
int IN2=4;     //direction 2

int servoPos;  
int mSpeed=0;
int direct=0;
int dt=50;
int sdt=100;
float distance;


Stepper myStepper(spr,8,10,9,11);
Servo myServo;
IRrecv IR(irPin);
decode_results cmd;



void setup() {
  // put your setup code here, to run once:
myStepper.setSpeed(stepMotspeed);
myServo.attach(servoPin);
IR.enableIRIn();
Serial.begin(9600);
pinMode(trigPin,OUTPUT);
pinMode(echoPin,INPUT);
pinMode(EN,OUTPUT);
pinMode(IN1,OUTPUT);
pinMode(IN2,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  driveMode=IR_read();
  while(driveMode==2){
    move_forward_nonstop();
    distance=read_distance();
    if(distance < 50.0){
      myServo.write(60);
      delay(dt);
      myServo.write(30);
      delay(dt);
      myServo.write(0);
      delay(dt);
      myServo.write(90);
      delay(dt);
    }
    if(distance < 15.0){
      for(int k=1;k<=3;k=k+1){
        reduce_speed();
        if(k==1){
          myStepper.step(512);
          delay(sdt);
          distance=read_distance();
          myStepper.step(-512);
          delay(sdt);
          if(distance > 50.0){
            direct=-90;
            k=3;
          }
        }
        if(k==2){
            myStepper.step(-512);
            delay(sdt);
            distance=read_distance();
            myStepper.step(-512);
          if(distance > 50.0){
            direct=+90;
          }
          
        }
      }
      move_backward();
      myServo.write(90+direct);
      delay(dt);
      move_forward();
      myServo.write(90);
      delay(dt);
      move_forward_nonstop();
      driveMode=IR_read();
    }
    driveMode=IR_read();
  }
  while(driveMode==1){
    driveMode=IR_read();
  }
}


float read_distance(){
  float counter=0;
  float dist;
  int pingTraveltime;
  int j;
  
  for(j=1;j<=10;j=j+1){
    digitalWrite(trigPin,LOW);
    delayMicroseconds(10);
    digitalWrite(trigPin,HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin,LOW);
    delayMicroseconds(10);
    pingTraveltime=pulseIn(echoPin,HIGH);
    counter=counter+(34000.*(pingTraveltime/2000000.));
    }

  dist= counter/10;
  return dist;
}


int IR_read(){
  int drive;
  drive=driveMode;
  if(IR.decode(&cmd)){
    switch(cmd.value){
      case pwr   : drive=0;return drive;break;
      case mode  : drive=1;return drive;break;
      case play  : drive=2;return drive;break;
      case two   : move_forward();return drive;break;
      case six   : turn_right();return drive;break;
      case eight : move_backward();return drive;break;
      case four  : turn_left();return drive;break;
    }
  }
  else{
   return drive;
  }
  delay(50);
}


void move_forward(){
  myServo.write(90);
  delay(dt);
  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,LOW);
  analogWrite(EN,255);
  delay(30);
  analogWrite(EN,200);
  analogWrite(EN,250);

  driveMode=IR_read();
  delay(20);

  analogWrite(EN,200);
  delay(20);
  analogWrite(EN,100);
  delay(20);
  analogWrite(EN,0);
}


void move_backward(){
  myServo.write(90);
  delay(dt);
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,HIGH);
  analogWrite(EN,255);
  delay(30);
  analogWrite(EN,200);
  analogWrite(EN,250);

  driveMode=IR_read();
  delay(20);

  analogWrite(EN,200);
  delay(20);
  analogWrite(EN,100);
  delay(20);
  analogWrite(EN,0);
}


void turn_right(){
  myServo.write(0);
  delay(dt);
  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,LOW);
  analogWrite(EN,255);
  delay(30);
  analogWrite(EN,200);
  analogWrite(EN,250);

  driveMode=IR_read();
  delay(20);

  analogWrite(EN,200);
  delay(20);
  analogWrite(EN,100);
  delay(20);
  analogWrite(EN,0);
  myServo.write(90);
  delay(dt);
}


void turn_left(){
  myServo.write(180);
  delay(dt);
  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,LOW);
  analogWrite(EN,255);
  delay(30);
  analogWrite(EN,200);
  analogWrite(EN,250);

  driveMode=IR_read();
  delay(20);

  analogWrite(EN,200);
  delay(20);
  analogWrite(EN,100);
  delay(20);
  analogWrite(EN,0);
  myServo.write(90);
  delay(dt);
}


void move_forward_nonstop(){
  if(mSpeed==0){
    digitalWrite(IN1,HIGH);
    digitalWrite(IN2,LOW);
    analogWrite(EN,255);
    delay(50);
    analogWrite(EN,150);
    delay(20);
    analogWrite(EN,200);
    delay(20);
    analogWrite(EN,250);
    mSpeed=250;
   }
   else{
    digitalWrite(IN1,HIGH);
    digitalWrite(IN2,LOW);
    analogWrite(EN,250);
    mSpeed=250;
   }
}


void reduce_speed(){
  while(mSpeed > 150){
    mSpeed=mSpeed-50;
    analogWrite(EN,mSpeed);
  }
}
