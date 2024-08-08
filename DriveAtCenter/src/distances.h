uint8_t distanceFrontLeftInside;
uint8_t distanceFrontRightInside;
uint8_t distanceRearRightInside;
uint8_t distanceRearLeftInside;
uint8_t distanceFrontRightOutside;
uint8_t distanceFrontLeftOutside;
uint8_t distanceRearRightOutside;
uint8_t distanceRearLeftOutside;
uint8_t distanceFrontLeft;
uint8_t distanceFrontRight;

uint16_t distance;

unsigned long duration;
unsigned long maxDuration;

void takeDistance()
{
  delay(20);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(15);
  digitalWrite(triggerPin, LOW);
  duration = pulseIn(echoPin, HIGH, maxDuration);
  if (duration == 0)
  {
    duration = maxDuration + 100;
  }
  distance = duration * 0.03432 / 2;
}

void checkForObstacleAtFront()
{
  maxDuration = startBrakingAt * 2 / 0.03432;

  triggerPin = frontLeftTriggerPin;
  echoPin = frontLeftEchoPin;
  takeDistance();
  distanceFrontLeft = distance;

  triggerPin = frontRightTriggerPin;
  echoPin = frontRightEchoPin;
  takeDistance();
  distanceFrontRight = distance;
}

void frontDistanceInside()
{
  maxDuration = maxDistanceSide * 2 / 0.03432;

  triggerPin = frontRightInsideTriggerPin;
  echoPin = frontRightInsideEchoPin;
  takeDistance();
  distanceFrontRightInside = distance;

  triggerPin = frontLeftInsideTriggerPin;
  echoPin = frontLeftInsideEchoPin;
  takeDistance();
  distanceFrontLeftInside = distance;
}

void rearDistanceInside()
{
  maxDuration = maxDistanceSide * 2 / 0.03432;

  triggerPin = rearRightInsideTriggerPin;
  echoPin = rearRightInsideEchoPin;
  takeDistance();
  distanceRearRightInside = distance;

  triggerPin = rearLeftInsideTriggerPin;
  echoPin = rearLeftInsideEchoPin;
  takeDistance();
  distanceRearLeftInside = distance;
}

void frontDistanceOutside()
{
  maxDuration = maxDistanceSide * 2 / 0.03432;

  triggerPin = frontRightOutsideTriggerPin;
  echoPin = frontRightOutsideEchoPin;
  takeDistance();
  distanceFrontRightOutside = distance;

  triggerPin = frontLeftOutsideTriggerPin;
  echoPin = frontLeftOutsideEchoPin;
  takeDistance();
  distanceFrontLeftOutside = distance;
}

void rearDistanceOutside()
{
  maxDuration = maxDistanceSide * 2 / 0.03432;

  triggerPin = rearRightOutsideTriggerPin;
  echoPin = rearRightOutsideEchoPin;
  takeDistance();
  distanceRearRightOutside = distance;

  triggerPin = rearLeftOutsideTriggerPin;
  echoPin = rearLeftOutsideEchoPin;
  takeDistance();
  distanceRearLeftOutside = distance;
}
