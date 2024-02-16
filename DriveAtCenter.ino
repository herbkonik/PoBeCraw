/*
  Last Update: 07.01.2024, last stable working version is _05.

  With _06 i start to include to change to the next row.
  For testing there is the need of 3 rows at least


*/

#include <Arduino.h>
#include <HCSR04_herbk.h>
#define SONAR_NUM 10    // Number of sensors.
#define MAX_DISTANCE 50 // Maximum distance (in cm) to ping.

HCSR04_herbk sonar[SONAR_NUM] = {     // Sensor object array.

  HCSR04_herbk(38, 37, 200),          // Sensor 0 = front left
  HCSR04_herbk(35, 31, 200),          // Sensor 1 = front right
  HCSR04_herbk(12, 11, MAX_DISTANCE), // Sensor 2 = left outside front
  HCSR04_herbk(2, 34, MAX_DISTANCE),  // Sensor 3 = left outside back
  HCSR04_herbk(8, 39, MAX_DISTANCE),  // Sensor 4 = left inside front
  HCSR04_herbk(32, 36, MAX_DISTANCE), // Sensor 5 = left inside back
  HCSR04_herbk(15, 16, MAX_DISTANCE), // Sensor 6 = right outside front
  HCSR04_herbk(21, 14, MAX_DISTANCE), // Sensor 7 = right outside back
  HCSR04_herbk(17, 23, MAX_DISTANCE), // Sensor 8 = right inside front
  HCSR04_herbk(13, 26, MAX_DISTANCE), // Sensor 9 = right inside back
};

constexpr uint8_t max_speed = 250; // adjust values here
constexpr uint8_t min_speed = 60;
constexpr uint8_t min_space_side = 10;
constexpr uint8_t max_space_side = 35; // must be less or equal MAX_DISTANCE
constexpr uint8_t min_space_front = 15;
constexpr uint8_t start_braking_at = 120;
constexpr uint8_t step_speed_up = 5;
constexpr uint8_t step_slowdown = 10;
// constexpr uint8_t max_different = 90; // max differnt between the 2 motors
constexpr uint8_t sensordelay = 30;

constexpr uint8_t motor_left = 3; // pins where the motors conected to
constexpr uint8_t motor_left_direction = 4;
constexpr uint8_t motor_right = 9;
constexpr uint8_t motor_right_direction = 10;

uint8_t speed_motor_left = 0;
uint8_t speed_motor_right = 0;

uint16_t distance_front_left = 0;
uint16_t distance_front_right = 0;
uint16_t distance_outside_left_front = 0;
uint16_t distance_outside_left_back = 0;
uint16_t distance_outside_right_front = 0;
uint16_t distance_outside_right_back = 0;
uint16_t distance_inside_left_front = 0;
uint16_t distance_inside_left_back = 0;
uint16_t distance_inside_right_front = 0;
uint16_t distance_inside_right_back = 0;

bool end_of_row = false;
bool BLDC_left_forward = true; // turning direction for the BLDCs
bool BLDC_right_forward = false;
bool BLDC_left_reverse = false;
bool BLDC_right_reverse = true;
bool clockwise = true;
bool eor_check = false;
bool turning_back = false;


//  void set_parallel();


void set_motor()
{
  analogWrite(motor_right, speed_motor_right);
  analogWrite(motor_left, speed_motor_left);
}

void stay()
{

  speed_motor_left = 0;
  speed_motor_right = 0;
  set_motor();

  delay(1000);
}

void slowdown()
{
  speed_motor_right -= step_slowdown;
  if (speed_motor_right < min_speed)
  {
    speed_motor_right = min_speed;
  }
  speed_motor_left -= step_slowdown;
  if (speed_motor_left < min_speed)
  {
    speed_motor_left = min_speed;
  }
  set_motor();
}

void check_speed()
{

  if (speed_motor_left >= max_speed || speed_motor_right >= max_speed)
  {
    speed_motor_right -= step_speed_up;
    speed_motor_left -= step_speed_up;
  }
  else if (speed_motor_left < min_speed && speed_motor_right < min_speed)
  {
    speed_motor_left = min_speed;
    speed_motor_right = min_speed;
  }
  else if ((speed_motor_left + step_speed_up) < max_speed && (speed_motor_right + step_speed_up) < max_speed)
  {
    if (eor_check == false)
    {
      speed_motor_right += step_speed_up;
      speed_motor_left += step_speed_up;
    }
  }

  set_motor();
}

void check_end_of_row()
{

  delay(sensordelay);
  //  distance_outside_left_front = sonar[2].ping_cm();
  //  delay(sensordelay);
  //  distance_outside_right_front = sonar[6].ping_cm();
  //  delay(sensordelay);
  //  distance_outside_left_back = sonar[3].ping_cm();
  //  delay(sensordelay);
  //  distance_outside_right_back = sonar[7].ping_cm();
  distance_inside_left_back = sonar[5].ping_cm();
  distance_inside_right_back = sonar[9].ping_cm();
  slowdown();

  if (distance_inside_left_back >= max_space_side && distance_inside_right_back >= max_space_side)
  {
    if (speed_motor_left <= min_speed || speed_motor_right <= min_speed)
    {
      stay();
      end_of_row = true;
      delay(500);
    }
  }
}

void checkcenter()
{

  delay(sensordelay);
  distance_inside_left_front = sonar[4].ping_cm();
  //  delay(sensordelay);
  //  distance_inside_left_back = sonar[5].ping_cm();
  //  delay(sensordelay);
  distance_inside_right_front = sonar[8].ping_cm();
  //  delay(sensordelay);
  //  distance_inside_right_back = sonar[9].ping_cm();

  if (distance_inside_left_front < distance_inside_right_front)
  {
    if ((speed_motor_right + step_speed_up) < max_speed)
    {
      speed_motor_right += step_speed_up;
      check_speed();
    }
    else if ((speed_motor_right - step_speed_up) > min_speed)
    {
      speed_motor_left -= step_speed_up;
      check_speed();
    }
  }
  else if (distance_inside_right_front < distance_inside_left_front)
  {
    if ((speed_motor_left + step_speed_up) < max_speed)
    {
      speed_motor_left += step_speed_up;
      check_speed();
    }
    else if ((speed_motor_left - step_speed_up) > min_speed)
    {
      speed_motor_right -= step_speed_up;
      check_speed();
    }
  }
  else if (distance_inside_right_front == distance_inside_left_front)
  {
    if (speed_motor_left > speed_motor_right)
    {
      speed_motor_left = speed_motor_right;
      set_motor();
    }
    else if (speed_motor_right > speed_motor_left)
    {
      speed_motor_right = speed_motor_left;
      set_motor();
    }
  }

}

void forward()
{
  digitalWrite(motor_left_direction, BLDC_left_forward);
  digitalWrite(motor_right_direction, BLDC_right_forward);

  delay(sensordelay);
  distance_front_left = sonar[0].ping_cm();
  distance_front_right = sonar[1].ping_cm();

  if (distance_front_right <= min_space_front || distance_front_left <= min_space_front)
  {
    stay();
  }
  else if (distance_front_right <= start_braking_at || distance_front_left <= start_braking_at)
  {
    slowdown();
  }
  else
  {
    check_speed();
  }


  if (distance_inside_left_front > max_space_side && distance_inside_right_front > max_space_side)
  {
    check_end_of_row();
    eor_check = true;
  }
  else
  {
    eor_check = false;
  }

  checkcenter();

}

void backward()
{

  digitalWrite(motor_left_direction, BLDC_left_reverse);
  digitalWrite(motor_right_direction, BLDC_right_reverse);
  delay(200);
  check_speed();

  if (clockwise == true)  {
    delay(sensordelay);
    distance_outside_right_front = sonar[6].ping_cm();
    while (distance_outside_right_front > max_space_side) {
      delay(sensordelay);
      distance_outside_right_front = sonar[6].ping_cm();
    }
    stay();
  }
  else if (clockwise == false)  {
    delay(sensordelay);
    distance_outside_left_front = sonar[2].ping_cm();
    while (distance_outside_left_front > max_space_side) {
      delay(sensordelay);
      distance_outside_left_front = sonar[2].ping_cm();
    }
    stay();
  }

  delay(4000);
  next_row();
}


void where_to_go()
{
  delay(sensordelay);
  distance_outside_left_front = sonar[2].ping_cm();
  delay(sensordelay);
  distance_outside_right_front = sonar[6].ping_cm();
  delay(sensordelay);
  distance_outside_left_back = sonar[3].ping_cm();
  delay(sensordelay);
  distance_outside_right_back = sonar[7].ping_cm();

  if (distance_outside_right_front <= max_space_side && distance_outside_right_back <= max_space_side)
  {
    clockwise = true;
  }
  else if (distance_outside_left_front <= max_space_side && distance_outside_left_back <= max_space_side)
  {
    clockwise = false;
  }
  else
  {
    clockwise = true;
  }
}

void set_parallel()
{

  delay(sensordelay);
  distance_inside_left_front = sonar[4].ping_cm();
  delay(sensordelay);
  distance_inside_left_back = sonar[5].ping_cm();

  if (distance_inside_left_front > distance_inside_left_back)
  {
    while (distance_inside_left_front > distance_inside_left_back)
    {
      digitalWrite(motor_left_direction, BLDC_left_forward);
      speed_motor_left = min_speed;
      set_motor();

      delay(sensordelay);
      distance_inside_left_front = sonar[4].ping_cm();
      delay(sensordelay);
      distance_inside_left_back = sonar[5].ping_cm();
    }
  }
  else if (distance_inside_left_front < distance_inside_left_back)
  {
    while (distance_inside_left_front < distance_inside_left_back)
    {
      digitalWrite(motor_left_direction, BLDC_left_reverse);
      speed_motor_left = min_speed;
      set_motor();

      delay(sensordelay);
      distance_inside_left_front = sonar[4].ping_cm();
      delay(sensordelay);
      distance_inside_left_back = sonar[5].ping_cm();
    }
  }
  stay();
}

void loop()
{
  if (end_of_row == false)
  {
    forward();
  }
  else if (end_of_row == true)
  {
    backward();
  }
}

void setup()
{

  //      Serial.begin(115200);

  pinMode(motor_left, OUTPUT);
  pinMode(motor_left_direction, OUTPUT);
  pinMode(motor_right, OUTPUT);
  pinMode(motor_right_direction, OUTPUT);

  digitalWrite(motor_left_direction, BLDC_left_forward);
  digitalWrite(motor_right_direction, BLDC_right_forward);

  stay();
  set_parallel();
  where_to_go();
  delay(3000);
}



void next_row() {

  if (speed_motor_left == 0 && speed_motor_right == 0) {
    digitalWrite(motor_left_direction, BLDC_left_forward);
    digitalWrite(motor_right_direction, BLDC_right_forward);
    delay(200);
  }

  //  turning_back = true;
  set_parallel();
  delay(1000);

  if (clockwise == true)  {
    speed_motor_right = min_speed;
    speed_motor_left = (min_speed * 2);
    set_motor();
    clockwise = false;
  }
  else if (clockwise == false)  {
    speed_motor_right = (min_speed * 2);
    speed_motor_left = min_speed;
    set_motor();
    clockwise = true;
  }

  /*                                      Wird wahrscheinlich nicht benötigt
    delay(sensordelay);
    distance_inside_left_front = sonar[4].ping_cm();
    delay(sensordelay);
    distance_inside_right_front = sonar[8].ping_cm();

    while (distance_inside_left_front >= max_space_side && distance_inside_right_front >= max_space_side) {
      delay(sensordelay);
      distance_inside_left_front = sonar[4].ping_cm();
      delay(sensordelay);
      distance_inside_right_front = sonar[8].ping_cm();
    }
  */

  delay(sensordelay);
  distance_inside_left_back = sonar[4].ping_cm();
  delay(sensordelay);
  distance_inside_right_back = sonar[8].ping_cm();

  while (distance_inside_left_back >= max_space_side && distance_inside_right_back >= max_space_side) {
    delay(sensordelay);
    distance_inside_left_back = sonar[4].ping_cm();
    delay(sensordelay);
    distance_inside_right_back = sonar[8].ping_cm();
  }

  speed_motor_right = min_speed ;
  speed_motor_left = min_speed;
  set_motor();
  delay(500);
  stay();
  set_parallel();
  end_of_row = false;

}
