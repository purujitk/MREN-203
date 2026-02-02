/*
PI controller - MREN 203

Purujit, Yan, Kaustubh

transational speed controller and rotational rate controller

*/

//left wheel connections
int EB = 6;
int I3 = 7;
int I4 = 5;

double omega_L = 0;
double omega_R = 0;
double vel_L = 0;
double vel_R = 0;

//encoder
const byte SIGNAL_A_L = 8;
const byte SIGNAL_B_L = 9;
const byte SIGNAL_A_R = 10;
const byte SIGNAL_B_R = 11;

volatile long encoder_ticks_a = 0;
volatile long encoder_ticks_b = 0;

//right wheel connections
int EA = 3;
int I1 = 2;
int I2 = 4;

//desired values
double v_d = 1;
double omega = 0;

//wheel and robot charactersistics
const int TPR = 3000;
const double RHO = 0.0625;
const double L = 0.2775;

//output to wheels
bool dir_l = 0;
bool dir_r = 0;

//wheel errors
double e_now_l = 0;
double e_now_r = 0;
double e_sum_l = 0;
double e_sum_r = 0;

//time
long t_last = 0;
long t_now = 0;
double T = 1000;

//proportional
long u_l = 0;
long u_r = 0;


////////////////////////////////////////// Wheel Speed Estimation //////////////////////////////////////////////////
void decodeEncoderTicks()
{
    if (digitalRead(SIGNAL_B_L) == LOW)
    {
        // SIGNAL_A leads SIGNAL_B, so count one way
        encoder_ticks_a--;
    }
    else
    {
        // SIGNAL_B leads SIGNAL_A, so count the other way
        encoder_ticks_a++;
    }

    if (digitalRead(SIGNAL_B_R) == LOW)
    {
        encoder_ticks_b--;
    }
    else
    {
        encoder_ticks_b++;
    }
}


////////////////////////////////////////////////////////////error wheel///////////////////////////////////////////////////////
void error_wheel_l(double v_d, double v_l, double omega, double l)
{

  e_now_l = (v_d - (l * omega) * 0.5) - v_l;
  e_sum_l += (v_d - (l * omega) * 0.5) - v_l;


}

void error_wheel_r(double v_d, double v_r, double omega, double l)
{

  e_now_r = (v_d + (l * omega) * 0.5) - v_r;
  e_sum_r += (v_d + (l * omega) * 0.5) - v_r;

}

/////////////////////////////////////////////////////////controller//////////////////////////////////////////////////////////
void pi_control(int k_p, int k_i)
{

  u_l = k_p*e_now_l + k_i*e_sum_l;
  u_r = k_p*e_now_r + k_i*e_sum_r;

// u_l control ouput constraint
  if (u_l > 255 )
  {
    u_l = 255;
  }
  else if (u_r < -255)
  {
    u_l = -255;
  }
  
// u_r control output constraint
  if (u_r > 255 )
  {
    u_r = 255;
  }
  else if (u_r < -255)
  {
    u_r = -255;
  }

//direction flag
  //left
  if (u_l > 0 )
  {
    dir_l = 1;
  }else 
  {
    dir_l = 0;
  }

  //right
  if (u_r > 0 )
  {
    dir_r = 1;
  }else 
  {
    dir_r = 0;
  }
}

//////////////////////////////////////////////////////////////////////////setup + loop//////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(9600);
  //wheel setup
  pinMode(EB, OUTPUT);
  pinMode(I2, OUTPUT);
  pinMode(I1, OUTPUT);
  pinMode(EA, OUTPUT);
  pinMode(I3, OUTPUT);
  pinMode(I4, OUTPUT);

  // Set the pin modes for the encoders
  pinMode(SIGNAL_A_L, INPUT);
  pinMode(SIGNAL_B_L, INPUT);
  pinMode(SIGNAL_A_R, INPUT);
  pinMode(SIGNAL_B_R, INPUT);

  //interupt when there is a change in the encoder signals (detects direction of spin)
  attachInterrupt(digitalPinToInterrupt(SIGNAL_A_L), decodeEncoderTicks, RISING);
  attachInterrupt(digitalPinToInterrupt(SIGNAL_A_R), decodeEncoderTicks, RISING);
}

void loop() {
// Get the elapsed time [ms]
    t_now = millis();

    // v_d = Serial.read();
    if (t_now - t_last >= T)
    {
      // Estimate the rotational speed [rad/s]
      omega_L = 2.0 * PI * ((double)encoder_ticks_a / (double)TPR) * 1000.0 / (double)(t_now - t_last);
      omega_R = 2.0 * PI * (-1 *(double)encoder_ticks_b / (double)TPR) * 1000.0 / (double)(t_now - t_last);

      vel_L = RHO * omega_L;
      vel_R = RHO * omega_R;
      
      error_wheel_l(v_d, vel_L, omega, L);
      error_wheel_r(v_d, vel_R, omega, L);
      pi_control(0,5);

      // Print some stuff to the serial monitor 
      // Serial.print("Estimated left wheel speed Left: ");
      // Serial.print(omega_L);
      // Serial.print(" rad/s");        
      // Serial.print("Estimated left wheel speed Left: ");
      // Serial.print(omega_R);
      // Serial.print(" rad/s");
      // Serial.print("\n");

      Serial.print("left PWM: ");
      Serial.print(u_l);

      Serial.print(" right PWM: ");
      Serial.println(u_r);


      // Record the current time [ms]
      t_last = t_now;

      // Reset the encoder ticks counter
      encoder_ticks_a = 0;
      encoder_ticks_b = 0;
    }

  
  if (dir_l = 0)
  {
    digitalWrite(I1, LOW);
    digitalWrite(I2, HIGH);
  }else{
    digitalWrite(I1, HIGH);
    digitalWrite(I2, LOW);
  }

  if (dir_r = 1)
  {
    digitalWrite(I3, LOW);
    digitalWrite(I4, HIGH);
  }else
  {
    digitalWrite(I3, HIGH);
    digitalWrite(I4, LOW);
  }

  analogWrite(EA, u_l);
  analogWrite(EB, u_r);

}

