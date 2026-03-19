/*
PI controller - MREN 203

Purujit, Yan, Kaustubh

transational speed controller and rotational rate controller

*/

//left wheel connections
// int EB = 6;
// int I3 = 7;
// int I4 = 5;
int EA = 6;
int I1 = 5;
int I2 = 7;

double omega_L = 0;
double omega_R = 0;
double vel_L = 0;
double vel_R = 0;

//encoder
const byte SIGNAL_B_R = 8;
const byte SIGNAL_A_R = 9;
const byte SIGNAL_B_L = 10;
const byte SIGNAL_A_L = 11;

volatile long encoder_ticks_a = 0;
volatile long encoder_ticks_b = 0;

//right wheel connections
int EB = 3;
int I3 = 4;
int I4 = 2;

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

float v_desired = 0;
float omega = 0;

float scan_v_d = 0;
float scan_omega = 0;

//time
long t_last = 0;
long t_now = 0;

//proportional
long u_l = 0;
long u_r = 0;


////////////////////////////////////////// Wheel Speed Estimation //////////////////////////////////////////////////

// SEPARATE ISR for left encoder only
void decodeEncoderTicks_L()
{
    if (digitalRead(SIGNAL_B_L) == LOW)
    {
        encoder_ticks_a--;
    }
    else
    {
        encoder_ticks_a++;
    }
}

// SEPARATE ISR for right encoder only
void decodeEncoderTicks_R()
{
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
  e_sum_l += e_now_l;
}

void error_wheel_r(double v_d, double v_r, double omega, double l)
{
  e_now_r = (v_d + (l * omega) * 0.5) - v_r;
  e_sum_r += e_now_r;
}

/////////////////////////////////////////////////////////controller//////////////////////////////////////////////////////////
void pi_control(int k_p, int k_i)
{
  u_l = k_p * e_now_l + k_i * e_sum_l;
  u_r = k_p * e_now_r + k_i * e_sum_r;

  // u_l control output constraint
  if (u_l > 255) {
      u_l = 255;
      e_sum_l -= e_now_l;  // Anti-windup
  }
  else if (u_l < -255) {
      u_l = -255;
      e_sum_l -= e_now_l;  // Anti-windup
  }
  
  // u_r control output constraint
  if (u_r > 255) {
      u_r = 255;
      e_sum_r -= e_now_r;  // Anti-windup
  }
  else if (u_r < -255) {
      u_r = -255;
      e_sum_r -= e_now_r;  // Anti-windup
  }

  // direction flag
  dir_l = (u_l > 0) ? 1 : 0;
  dir_r = (u_r > 0) ? 1 : 0;
}

//////////////////////////////////////////////////////////////////////////setup//////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(115200);

  // wheel setup
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

  // Attach SEPARATE ISRs for each encoder
  attachInterrupt(digitalPinToInterrupt(SIGNAL_A_L), decodeEncoderTicks_L, RISING);
  attachInterrupt(digitalPinToInterrupt(SIGNAL_A_R), decodeEncoderTicks_R, RISING);


  t_last = millis();
}

//////////////////////////////////////////////////////////////////////////loop//////////////////////////////////////////////////////////////////////

unsigned long last_publish = 0;

void loop() {
  t_now = millis();

  // Publish encoder ticks every 50ms
  if (t_now - last_publish >= 50) {
    Serial.print("ENC ");
    Serial.print(encoder_ticks_a);
    Serial.print(" ");
    Serial.println(encoder_ticks_b);
    last_publish = t_now;
  }

  long dt = t_now - t_last;

  if (Serial.available()) {
      String line = Serial.readStringUntil('\n');
      line.trim();
      if (line.startsWith("CMD")) {
          // Find the positions of the spaces
          int firstSpace = line.indexOf(' ');
          int secondSpace = line.indexOf(' ', firstSpace + 1);

          if (firstSpace != -1 && secondSpace != -1) {
            // Extract substrings and convert to float
            scan_v_d = line.substring(firstSpace + 1, secondSpace).toFloat();
            scan_omega = line.substring(secondSpace + 1).toFloat();

            v_desired = scan_v_d / 100.0;
            omega = scan_omega / 100.0;

            // Serial.println(v_desired);
            // Serial.println(omega);
          }
        }
      }

    // --- 2. Run PI control on a fixed time step (e.g. every 50ms) ---
  if (dt >= 50) {
    long dt = t_now - t_last;

    omega_L = 2.0 * PI * ((double)encoder_ticks_a / (double)TPR) * 1000.0 / (double)dt;
    omega_R = 2.0 * PI * (-1.0 * (double)encoder_ticks_b / (double)TPR) * 1000.0 / (double)dt;

    vel_L = RHO * omega_L;
    vel_R = RHO * omega_R;

    // Serial.print("left wheel speed: ");
    // Serial.println(vel_L);
    // Serial.print("Right wheel speed: ");
    // Serial.println(vel_R);
    

    error_wheel_l(v_desired, vel_L, omega, L);
    error_wheel_r(v_desired, vel_R, omega, L);
    pi_control(800, 30);

    encoder_ticks_a = 0;
    encoder_ticks_b = 0;
    t_last = t_now;
  }

  // Setting motor spin direction
  if (dir_l == 0) {
    digitalWrite(I1, LOW);
    digitalWrite(I2, HIGH);
  } else {
    digitalWrite(I1, HIGH);
    digitalWrite(I2, LOW);
  }

  if (dir_r == 1) {
    digitalWrite(I3, LOW);
    digitalWrite(I4, HIGH);
  } else {
    digitalWrite(I3, HIGH);
    digitalWrite(I4, LOW);
  }

  analogWrite(EA, abs(u_l));
  analogWrite(EB, abs(u_r));
}