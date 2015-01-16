
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
const int thresholdLight = 50;

/*
* Thresholds for controlling the red and amber lights. The green light will be switched 
* on if the ping value exceeds the amber threshold. Tweak these as necessary.
*/
const int thresholdPingR = 7500;
const int thresholdPingA = 12500;

/*
* We use a running average of the values read from the ping sensor in order to obtain a 
* more stable value in case of bad readings.
*/
const int pingBufferSize = 5;
long pingBuffer[pingBufferSize];

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
  pinMode(pinLight, INPUT);
  pinMode(pinPower, OUTPUT);
  pinMode(pinR, OUTPUT);
  pinMode(pinA, OUTPUT);
  pinMode(pinG, OUTPUT);
  
  // Initialize the ping buffer with sane values.
  for (int i = 0; i < pingBufferSize; i++) {
    pingBuffer[i] = 15000;
  }
}

/*
* Main loop
*/
void loop() 
{
  // Read the value from the light sensor; turn the power to the signal
  // on or off accordingly
  int lightValue = analogRead(pinLight);
  if (lightValue <= thresholdLight && powerOn == false) {
    digitalWrite(pinPower, HIGH);
    powerOn = true;
  }
  else if (lightValue > thresholdLight && powerOn == true) {
    digitalWrite(pinPower, LOW);
    setLights(false, false, false);
    powerOn = false;
  } 
  
  // If power is off, delay 1 second and bail so we don't unnecessarily spin
  if (powerOn == false) {
    delay(1000);
    return;
  }
  
  // Power is on, perform a ping
  ping();
  
  // Get the average ping value and switch on the appropriate light
  long averagePing = getAveragePing();  
  if (averagePing <= thresholdPingR && !rOn) {
    setLights(true, false, false); // Red on
  }
  else if (averagePing > thresholdPingR && averagePing <= thresholdPingA && !aOn) {
    setLights(false, true, false); // Amber on
  }
  else if (averagePing > thresholdPingA && !gOn) {
    setLights(false, false, true); // Green on
  }
  
  // Don't need to go so fast
  delay(100);
}

/*
* Signals the ping sensor to run a cycle and adds the result to the buffer.
*/
void ping() 
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
  long ping = pulseIn(pinPing, HIGH);
  
  // Push the result into our buffer
  for (int i = pingBufferSize-1; i >= 0; i--) {
    pingBuffer[i] = pingBuffer[i-1];
  }
  pingBuffer[0] = ping;
}

/*
* Returns the average of the values in the ping buffer.
*/
long getAveragePing()
{
  long total = 0;
  for (int i = 0; i < pingBufferSize; i++) {
    total += pingBuffer[i];
  }
  return total / pingBufferSize;
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

