
/**
 * @file motor-angular-rate.ino
 * @author Joshua Marshall (joshua.marshall@queensu.ca)
 * @brief Arduino program to estimate motor speed from encoder.
 * @version 2.1
 * @date 2022-12-09
 *
 * @copyright Copyright (c) 2021-2022
 *
 */

// Wheel PWM pin (must be a PWM pin)
int EA = 3;

// Wheel direction digital pins
int I1 = 2;
int I2 = 4;

// Wheel PWM pin (must be a PWM pin)
int EB = 6;

// Wheel direction digital pins
int I3 = 7;
int I4 = 5;

// Motor PWM command variable [0-255]
byte u = 0;

// Left wheel encoder digital pins
const byte SIGNAL_A_L = 8;
const byte SIGNAL_B_L = 9;
const byte SIGNAL_A_R = 10;
const byte SIGNAL_B_R = 11;

// Encoder ticks per (motor) revolution (TPR)
const int TPR = 3000;

// Wheel radius [m]
const double RHO = 0.0625;

//track length
const double L = 0.2775;


// Counter to keep track of encoder ticks [integer]
volatile long encoder_ticks_a = 0;
volatile long encoder_ticks_b = 0;

// Variable to store estimated angular rate of left wheel [rad/s]
double omega_L = 0.0;
double omega_R = 0.0;
double omega = 0.0;


//velocity variables
double vel_L = 0.0;
double vel_R = 0.0;
double vel = 0.0;

// Sampling interval for measurements in milliseconds
const int T = 5000;

// Counters for milliseconds during interval
long t_now = 0;
long t_last = 0;

// This function is called when SIGNAL_A goes HIGH
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

void setup()
{
    // Open the serial port at 9600 bps
    Serial.begin(9600);

    // Set the pin modes for the motor driver
    pinMode(EA, OUTPUT);
    pinMode(I1, OUTPUT);
    pinMode(I2, OUTPUT);
    pinMode(EB, OUTPUT);
    pinMode(I3, OUTPUT);
    pinMode(I4, OUTPUT);


    // Set the pin modes for the encoders
    pinMode(SIGNAL_A_L, INPUT);
    pinMode(SIGNAL_B_L, INPUT);
    pinMode(SIGNAL_A_R, INPUT);
    pinMode(SIGNAL_B_R, INPUT);

    // Every time the pin goes high, this is a pulse
    attachInterrupt(digitalPinToInterrupt(SIGNAL_A_L), decodeEncoderTicks, RISING);
    attachInterrupt(digitalPinToInterrupt(SIGNAL_A_R), decodeEncoderTicks, RISING);

    // Print a message
    Serial.print("Program initialized.");
    Serial.print("\n");
}

void loop()
{
    // Get the elapsed time [ms]
    t_now = millis();

    if (t_now - t_last >= T)
    {
        // Estimate the rotational speed [rad/s]
        omega_L = 2.0 * PI * ((double)encoder_ticks_a / (double)TPR) * 1000.0 / (double)(t_now - t_last);
        omega_R = 2.0 * PI * (-1 *(double)encoder_ticks_b / (double)TPR) * 1000.0 / (double)(t_now - t_last);

        vel_L = RHO * omega_L;
        vel_R = RHO * omega_R;

        vel = 0.5*(vel_L + vel_R);

        omega = (1/L) * (vel_R - vel_L);

        Serial.print("Velocity: ");
        Serial.print(vel);
        Serial.print(" Angular Velocity: ");
        Serial.print(omega);
        Serial.print("\n");

        // Record the current time [ms]
        t_last = t_now;

        // Reset the encoder ticks counter
        encoder_ticks_a = 0;
        encoder_ticks_b = 0;
    }

    // Set the wheel motor PWM command [0-255]
    u = 128;

    // Select a direction
    digitalWrite(I1, LOW);
    digitalWrite(I2, HIGH);

    digitalWrite(I3, HIGH);
    digitalWrite(I4, LOW);

    // PWM command to the motor driver
    analogWrite(EA, u);
    analogWrite(EB, u);
}
