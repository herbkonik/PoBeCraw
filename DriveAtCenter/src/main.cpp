/*
Sketch is made for a LGT8F328P-LQFP48 Board
Last Update 24.07.2024

works through moveBackward
"nextRow" needs improvment, but most foults are based at the behavior of the HC-SR04 sensors.
*/

#include <Arduino.h>
#include "pins.h"

constexpr uint8_t maxSpeed = 240;
constexpr uint8_t minSpeed = 60;
constexpr uint8_t turnSpeed = 220;
constexpr uint8_t minDistanceSide = 10;
constexpr uint8_t maxDistanceSide = 50;
constexpr uint8_t minDistanceFront = 15;
constexpr uint8_t startBrakingAt = 120;
constexpr uint8_t stepSpeedUp = 2;
constexpr uint8_t stepSlowdown = 10;

uint8_t speedMotorLeft = 0;
uint8_t speedMotorRight = 0;

bool endOfRow = false;
bool leftForward = true;
bool rightForward = false;
bool leftReverse = false;
bool rightReverse = true;
bool clockwise = true;
bool endOfRowCheck = false;
bool turningBack = false;
bool braking = false;

#include "distances.h"

void forward()
{
  digitalWrite(motorLeftDirection, leftForward);
  digitalWrite(motorRightDirection, rightForward);
  delay(20);
}

void backward()
{
  digitalWrite(motorLeftDirection, leftReverse);
  digitalWrite(motorRightDirection, rightReverse);
  delay(20);
}

void setMotor()
{
  analogWrite(motorRight, speedMotorRight);
  analogWrite(motorLeft, speedMotorLeft);
}

void stay()
{
  speedMotorLeft = 0;
  speedMotorRight = 0;
  setMotor();
  braking = false;
  delay(1000);
}

void slowdown()
{
  speedMotorRight -= stepSlowdown;
  if (speedMotorRight < minSpeed)
  {
    speedMotorRight = minSpeed;
  }
  speedMotorLeft -= stepSlowdown;
  if (speedMotorLeft < minSpeed)
  {
    speedMotorLeft = minSpeed;
  }
  setMotor();
}

void checkCenter()
{
  frontDistanceInside();

  if (distanceFrontLeftInside > distanceFrontRightInside)
  {
    speedMotorLeft += stepSpeedUp;
  }
  else if (distanceFrontRightInside > distanceFrontLeftInside)
  {
    speedMotorRight += stepSpeedUp;
  }
  else if (distanceFrontRightInside == distanceFrontLeftInside)
  {
    speedMotorLeft = speedMotorRight;
  }
}

void checkSpeed()
{
  if (speedMotorLeft >= maxSpeed || speedMotorRight >= maxSpeed)
  {
    speedMotorRight -= stepSpeedUp;
    speedMotorLeft -= stepSpeedUp;
  }
  else if (speedMotorLeft < minSpeed && speedMotorRight < minSpeed)
  {
    speedMotorLeft = minSpeed;
    speedMotorRight = minSpeed;
  }
  else if ((speedMotorLeft + stepSpeedUp) < maxSpeed || (speedMotorRight + stepSpeedUp) < maxSpeed)
  {
    if (endOfRowCheck == false && braking == false)
    {
      speedMotorRight += stepSpeedUp;
      speedMotorLeft += stepSpeedUp;
    }
  }
  setMotor();
}

void checkEndOfRow()
{
  slowdown();
  rearDistanceInside();
  if (distanceRearLeftInside >= maxDistanceSide && distanceRearRightInside >= maxDistanceSide)
  {
    if (speedMotorLeft <= minSpeed || speedMotorRight <= minSpeed)
    {
      stay();
      endOfRow = true;
      endOfRowCheck = false;
      delay(500);
    }
  }
}

void moveForward()
{
  forward();

  checkForObstacleAtFront();
  if (distanceFrontRight <= minDistanceFront || distanceFrontLeft <= minDistanceFront)
  {
    stay();
  }
  else if (distanceFrontRight <= startBrakingAt || distanceFrontLeft <= startBrakingAt)
  {
    slowdown();
    braking = true;
  }
  else
  {
    braking = false;
  }

  checkCenter();

  if (distanceFrontLeftInside > maxDistanceSide && distanceFrontRightInside > maxDistanceSide)
  {
    checkEndOfRow();
    endOfRowCheck = true;
  }
  else
  {
    endOfRowCheck = false;
  }
  checkSpeed();
}

void setParallel()
{
  frontDistanceInside();
  rearDistanceInside();

  if (distanceFrontLeftInside > distanceRearLeftInside)
  {
    while (distanceFrontLeftInside > distanceRearLeftInside)
    {
      digitalWrite(motorLeftDirection, leftForward);
      speedMotorLeft = minSpeed + 30;
      setMotor();
      delay(20);
      frontDistanceInside();
      rearDistanceInside();
    }
  }
  else if (distanceFrontLeftInside < distanceRearLeftInside)
  {
    while (distanceFrontLeftInside < distanceRearLeftInside)
    {
      digitalWrite(motorLeftDirection, leftReverse);
      speedMotorLeft = minSpeed + 30;
      setMotor();
      delay(20);
      frontDistanceInside();
      rearDistanceInside();
    }
  }
  stay();
}

void whereToGo()
{
  frontDistanceOutside();
  rearDistanceOutside();

  if (distanceFrontLeftOutside <= maxDistanceSide && distanceRearLeftOutside <= maxDistanceSide)
  {
    clockwise = false;
  }
  else
  {
    clockwise = true;
  }
}

void nextRow()
{
  forward();
  setParallel();
  speedMotorRight = minSpeed;
  speedMotorLeft = minSpeed;
  setMotor();
  delay(1000);

  if (clockwise == true)
  {
    for (int i = 0; i < ((turnSpeed - minSpeed) / stepSpeedUp); i++)
    {
      speedMotorRight = minSpeed;
      speedMotorLeft += stepSpeedUp;
      setMotor();
      delay(10);
    }
    delay(3000);
    frontDistanceInside();
    while (distanceFrontLeftInside > maxDistanceSide)
    {
      frontDistanceInside();
    }
    while (distanceFrontRightInside < distanceFrontLeftInside)
    {
      frontDistanceInside();
      speedMotorLeft -= stepSlowdown;
      if (speedMotorLeft < minSpeed)
        speedMotorLeft = minSpeed;
      speedMotorRight = minSpeed;
      setMotor();
    }
    clockwise = false;
  }

  else if (clockwise == false)
  {
    for (int i = 0; i < ((turnSpeed - minSpeed) / stepSpeedUp); i++)
    {
      speedMotorRight += stepSpeedUp;
      speedMotorLeft = minSpeed;
      setMotor();
      delay(10);
    }
    delay(3000);
    frontDistanceInside();
    while (distanceFrontRightInside > maxDistanceSide)
    {
      frontDistanceInside();
    }
    while (distanceFrontLeftInside < distanceFrontRightInside)
    {
      frontDistanceInside();
      speedMotorRight -= stepSlowdown;
      if (speedMotorRight < minSpeed)
        speedMotorRight = minSpeed;
      speedMotorLeft = minSpeed;
      setMotor();
    }
    clockwise = true;
  }

  rearDistanceInside();
  while (distanceRearRightInside > maxDistanceSide)
  {
    rearDistanceInside();
  }
  delay(2000);
  stay();
  setParallel();
  delay(20000);
  endOfRow = false;
}

void moveBackward()
{
  backward();
  checkSpeed();

  frontDistanceOutside();

  if (clockwise == true)
  {
    while (distanceFrontRightOutside > maxDistanceSide)
    {
      frontDistanceOutside();
    }
  }
  else if (clockwise == false)
  {
    while (distanceFrontLeftOutside > maxDistanceSide)
    {
      frontDistanceOutside();
    }
  }
  delay(300);
  stay();
  nextRow();
}

void setup()
{

  pinMode(frontRightInsideTriggerPin, OUTPUT);
  pinMode(frontLeftInsideTriggerPin, OUTPUT);
  pinMode(rearRightInsideTriggerPin, OUTPUT);
  pinMode(rearLeftInsideTriggerPin, OUTPUT);
  pinMode(frontRightOutsideTriggerPin, OUTPUT);
  pinMode(frontLeftOutsideTriggerPin, OUTPUT);
  pinMode(rearRightOutsideTriggerPin, OUTPUT);
  pinMode(rearLeftOutsideTriggerPin, OUTPUT);
  pinMode(frontRightTriggerPin, OUTPUT);
  pinMode(frontLeftTriggerPin, OUTPUT);

  pinMode(frontRightInsideEchoPin, INPUT);
  pinMode(frontLeftInsideEchoPin, INPUT);
  pinMode(rearRightInsideEchoPin, INPUT);
  pinMode(rearLeftInsideEchoPin, INPUT);
  pinMode(frontRightOutsideEchoPin, INPUT);
  pinMode(frontLeftOutsideEchoPin, INPUT);
  pinMode(rearRightOutsideEchoPin, INPUT);
  pinMode(rearLeftOutsideEchoPin, INPUT);
  pinMode(frontRightEchoPin, INPUT);
  pinMode(frontLeftEchoPin, INPUT);

  pinMode(motorLeft, OUTPUT);
  pinMode(motorLeftDirection, OUTPUT);
  pinMode(motorRight, OUTPUT);
  pinMode(motorRightDirection, OUTPUT);

  forward();
  stay();
  delay(10000);
  endOfRow = false;
  setParallel();
  whereToGo();
  delay(1000);
}

void loop()
{
  if (!endOfRow)
  {
    moveForward();
  }
  else if (endOfRow)
  {
    moveBackward();
  }
}
