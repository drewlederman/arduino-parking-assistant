
/*
* Parking Assistant
* 
* Uses an ultrasonic sensor to control a traffic signal which assists you when 
* pulling your car into the garage.
*
* Copyright © Drew Lederman
*/

/*
* Pin constants
*/
const int pinLight = A5;
const int pinPing  = 7;
const int pinPower = 2;
const int pinR = 3;
const int pinA = 4;
const int pinG = 5;

/*
* If the value read from the light sensor is less than or equal to this value, the
* main power to the signal will be turned on, otherwise it will be turned off.
*/
const int thresholdLight = 100;

/*
* Thresholds for controlling the red and amber lights. The green light will be switched 
* on if the ping value exceeds the amber threshold. Tweak these as necessary.
*/
const int thresholdPingR = 7000;
const int thresholdPingA = 15000;

/*
* Timeout in microseconds to be used for the ping sensor
*/
const int pingTimeout = 17000;

/*
* Represents the state of the main power to the traffic signal.
*/
boolean powerOn = false;

/*
* Represents the states of each colored light.
*/
boolean rOn = false;
boolean aOn = false;
boolean gOn = false;

/*
* Initial setup
*/
void setup() 
{
  Serial.begin(9600);
  
  pinMode(pinLight, INPUT);
  pinMode(pinPower, OUTPUT);
  pinMode(pinR, OUTPUT);
  pinMode(pinA, OUTPUT);
  pinMode(pinG, OUTPUT);
}

/*
* Main loop
*/
void loop() 
{
  // Read the value from the light sensor; turn the main power to the signal
  // on or off accordingly
  boolean lightOn = checkLightSensor();
  if (lightOn && !powerOn) {
    setPower(true);
    setLights(false, false, true);
  }
  else if (!lightOn && powerOn) {
    setPower(false);
    setLights(false, false, false);
  } 
  
  // If power is off, sleep
  if (powerOn == false) {
    delay(1000);
    return;
  }
  
  // Power is on, ping!
  long pingValue = ping();
  if (pingValue == pingTimeout) { 
    // Timed-out, bail
    delay(100);
    return;
  }
  
  // Switch on the appropriate light
  if (pingValue <= thresholdPingR && !rOn) {
    setLights(true, false, false); // Red on
  }
  else if (pingValue > thresholdPingR && pingValue <= thresholdPingA && !aOn) {
    setLights(false, true, false); // Amber on
  }
  else if (pingValue > thresholdPingA && !gOn) {
    setLights(false, false, true); // Green on
  }
    
  // Don't need to go so fast
  delay(100);
}

/*
* Turns the main power on or off.
*/
void setPower(boolean power)
{
  digitalWrite(pinPower, powerOn = power ? HIGH : LOW);
}

/*
* Reads the value from the light sensor; returns true if light exceeds the threshold,
* false otherwise.
*/
boolean checkLightSensor()
{
  int lightValue = analogRead(pinLight);
  return lightValue <= thresholdLight;
}

/*
* Signals the ping sensor to run a cycle and returns the duration in microseconds.
*/
long ping() 
{
  // Send a short pulse to the ping sensor; this will signal the sensor to emit
  // a high frequency sound wave and wait for the echo to return.
  pinMode(pinPing, OUTPUT);
  digitalWrite(pinPing, LOW);
  delayMicroseconds(2);
  digitalWrite(pinPing, HIGH);
  delayMicroseconds(5);
  digitalWrite(pinPing, LOW);

  // Read from the sensor; the duration of the pulse represents the number 
  // of microseconds it took for the sound wave to return to the sensor.
  pinMode(pinPing, INPUT);
  long ping = pulseIn(pinPing, HIGH, pingTimeout);
  if (ping == 0) {
    return pingTimeout;
  }
  return ping;
}

/*
* Turns each colored light on or off
*/
void setLights(boolean r, boolean a, boolean g) 
{
  digitalWrite(pinR, rOn = r ? HIGH : LOW);
  digitalWrite(pinA, aOn = a ? HIGH : LOW);
  digitalWrite(pinG, gOn = g ? HIGH : LOW);
}

