#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;

float ax_offset = 0;
float ay_offset = 0;
float az_offset = 0;
float gx_offset = 0;
float gy_offset = 0;
float gz_offset = 0;


const double accel_threshold = 15; // aceleration threshold for motion detection
const double gyro_threshold = 1.2; // gyroscope threshold for motion detection
const int motion_period = 4000; // set one motion operating time to 4 seconds
sensors_event_t prev_a, prev_g, prev_temp;


// double ax, ay, az;
// double gx, gy, gz;
bool is_moving = false;

void setup() {
  Serial.begin(115200);
  mpuSetup();
  calculateOffsets();
  delay(5000);
  mpu.getEvent(&prev_a, &prev_g, &prev_temp);
}

void loop() {
  getIMUData();

  if (is_moving) {
    // start a round of movement detection if detected a start of move.
    unsigned long start_time = millis();
    while (millis()-start_time < motion_period) {
      sendIMUData();
      delay(25); // sense IMU data and send in frequency 40Hz
    }
  }
  delay(100); // check for motion, frequency 10Hz
}

void mpuSetup() {
  if (!mpu.begin()) {
    while (1) {
      delay(10);
    }
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  mpu.setHighPassFilter(MPU6050_HIGHPASS_0_63_HZ); 

  delay(100);
}

void getIMUData() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  calibrateIMU(&a, &g);

  if (check_motion(&a, &g, &prev_a)) is_moving = true;
  else is_moving = false;
  prev_a = a;
}

void sendIMUData() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // write code to sendIMUData

    /* Print out the values */
  Serial.print("AccelX:");
  Serial.print(a.acceleration.x);
  Serial.print(",");
  Serial.print("AccelY:");
  Serial.print(a.acceleration.y);
  Serial.print(",");
  Serial.print("AccelZ:");
  Serial.print(a.acceleration.z);
  Serial.println("");
  Serial.print("GyroX:");
  Serial.print(g.gyro.x);
  Serial.print(",");
  Serial.print("GyroY:");
  Serial.print(g.gyro.y);
  Serial.print(",");
  Serial.print("GyroZ:");
  Serial.print(g.gyro.z);
  Serial.println("");
}

bool check_motion(sensors_event_t *a, sensors_event_t *g, sensors_event_t *prev_a) {
  double delta_ax = abs(a->acceleration.x - prev_a->acceleration.x);
  double delta_ay = abs(a->acceleration.y - prev_a->acceleration.y);
  double delta_az = abs(a->acceleration.z - prev_a->acceleration.z);
  
  double total_accel = sqrt(delta_ax * delta_ax + delta_ay * delta_ay + delta_az * delta_az);
  double total_gyro = sqrt(g->gyro.x * g->gyro.x + g->gyro.y * g->gyro.y + g->gyro.z * g->gyro.z);
  // Serial.println(total_accel);
  // Serial.println(total_gyro);
  return total_accel > accel_threshold || total_gyro > gyro_threshold;
}

void calculateOffsets() {
  Serial.println("Calculating offsets..");
  float ax_sum = 0, ay_sum = 0, az_sum = 0;
  float gx_sum = 0, gy_sum = 0, gz_sum = 0;
  const int numReadings = 1000;

  for (int i = 0; i < numReadings; i++) {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    
    ax_sum += a.acceleration.x;
    ay_sum += a.acceleration.y;
    az_sum += a.acceleration.z;
    gx_sum += g.gyro.x;
    gy_sum += g.gyro.y;
    gz_sum += g.gyro.z;
    
    delay(5);
  }

  ax_offset = ax_sum / numReadings;
  ay_offset = ay_sum / numReadings;
  az_offset = (az_sum / numReadings);
  gx_offset = gx_sum / numReadings;
  gy_offset = gy_sum / numReadings;
  gz_offset = gz_sum / numReadings;

  Serial.println("Offsets:");
  Serial.print("Accel: ");
  Serial.print(ax_offset); Serial.print(", ");
  Serial.print(ay_offset); Serial.print(", ");
  Serial.println(az_offset);
  Serial.print("Gyro: ");
  Serial.print(gx_offset); Serial.print(", ");
  Serial.print(gy_offset); Serial.print(", ");
  Serial.println(gz_offset);
}

void calibrateIMU(sensors_event_t *a, sensors_event_t *g) {
  a->acceleration.x -= ax_offset;
  a->acceleration.y -= ay_offset;
  a->acceleration.z -= az_offset;

  g->gyro.x -= gx_offset;
  g->gyro.y -= gy_offset;
  g->gyro.z -= gz_offset;
}