int EB= 5; // Wheel PWM pin (must be a PWM pin)
int I3 = 7; // Wheel direction digital pin 1
int I4 = 6; // Wheel direction digital pin 2

int EA = 3;
int I2 = 4;
int I1 = 2;

void setup()
{
 // Configure digital pins for output
  pinMode(EA, OUTPUT);
  pinMode(I3, OUTPUT);
  pinMode(I4, OUTPUT);
  pinMode(EB, OUTPUT);
  pinMode(I2, OUTPUT);
  pinMode(I1, OUTPUT);

  // part 2
  // turn_clock();

  // delay(1000);

  // digitalWrite(I3, LOW);
  // digitalWrite(I4, HIGH);
  // analogWrite(EA, 255);

  // digitalWrite(I2, LOW);
  // digitalWrite(I1, HIGH);
  // analogWrite(EB, 255);

  // delay(1000);

  // turn_counter_clock();

  turn_circle(1);

  delay(3700);

  turn_circle(-1);

  delay(3700);

  digitalWrite(I3, HIGH);
  digitalWrite(I4, HIGH);
  analogWrite(EA, 255);

  digitalWrite(I2, HIGH);
  digitalWrite(I1, HIGH);
  analogWrite(EB, 255);



}

void loop()
{


}

void drive_for_back_fast(){

  //drive forward
  digitalWrite(I3, HIGH);
  digitalWrite(I4, LOW);
  analogWrite(EA, 255);

  digitalWrite(I2, HIGH);
  digitalWrite(I1, LOW);
  analogWrite(EB, 255);

  delay(1000);

  //drive backward
  digitalWrite(I3, LOW);
  digitalWrite(I4, HIGH);
  analogWrite(EA, 255);

  digitalWrite(I2, LOW);
  digitalWrite(I1, HIGH);
  analogWrite(EB, 255);

  delay(1000);

  analogWrite(EB, 0);
  analogWrite(EA, 0);

}

void drive_for_back_slow(){
  
  //drive forward
  digitalWrite(I3, HIGH);
  digitalWrite(I4, LOW);
  analogWrite(EA, 150);

  digitalWrite(I2, HIGH);
  digitalWrite(I1, LOW);
  analogWrite(EB, 150);

  delay(1000);

  //drive backward
  digitalWrite(I3, LOW);
  digitalWrite(I4, HIGH);
  analogWrite(EA, 150);

  digitalWrite(I2, LOW);
  digitalWrite(I1, HIGH);
  analogWrite(EB, 150);

  delay(1000);
  
  analogWrite(EB, 0);
  analogWrite(EA, 0);
}

void turn_counter_clock()
{

  digitalWrite(I3, HIGH);
  digitalWrite(I4, LOW);
  analogWrite(EA, 255);

  digitalWrite(I2, LOW);
  digitalWrite(I1, HIGH);
  analogWrite(EB, 255);

  delay(3000);

  digitalWrite(I3, HIGH);
  digitalWrite(I4, HIGH);
  analogWrite(EA, 255);

  digitalWrite(I2, HIGH);
  digitalWrite(I1, HIGH);
  analogWrite(EB, 255);


}

void turn_clock()
{

  digitalWrite(I3, LOW);
  digitalWrite(I4, HIGH);
  analogWrite(EA, 255);

  digitalWrite(I2, HIGH);
  digitalWrite(I1, LOW);
  analogWrite(EB, 255);

  delay(3000);

  digitalWrite(I3, HIGH);
  digitalWrite(I4, HIGH);
  analogWrite(EA, 255);

  digitalWrite(I2, HIGH);
  digitalWrite(I1, HIGH);
  analogWrite(EB, 255);


}

void turn_circle(int dir){
  
  if(dir > 0) //clockwise
  {
    digitalWrite(I3, HIGH);
    digitalWrite(I4, LOW);
    analogWrite(EA, 100);

    digitalWrite(I2, LOW);
    digitalWrite(I1, HIGH);
    analogWrite(EB, 255);
  }else //counter clcokwise
  {
    digitalWrite(I3, LOW);
    digitalWrite(I4, HIGH);
    analogWrite(EA, 255);

    digitalWrite(I2, HIGH);
    digitalWrite(I1, LOW);
    analogWrite(EB, 100);
  }
}

void figure_8(){

}