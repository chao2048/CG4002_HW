// modified from https://github.com/adafruit/Adafruit_MPU6050/tree/master/examples/motion_detection

#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;
double ax, ay, az;
double gx, gy, gz;

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  while (!Serial) {
    delay(10); // Wait for serial connection
  }

  // Initialize the MPU6050
  setup_imu();

  // Configure the MPU6050
  mpu.setAccelerometerRange(MPU6050_RANGE_4_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_184_HZ);

  delay(100); // Allow the sensor to stabilize
}

void loop() {
  get_motion_readings();
}

void setup_imu() {
  
  Serial.println("Adafruit MPU6050 test!");

  // Try to initialize!
  while (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    delay(3000);
  }
  Serial.println("MPU6050 Found!");

  //setupt motion detection
  mpu.setHighPassFilter(MPU6050_HIGHPASS_2_5_HZ); // set high pass filter for accelerometer to remove the constant effect of gravity
  mpu.setMotionDetectionThreshold(1); // set the motion detection threshold register of MPU6050
  mpu.setMotionDetectionDuration(10); // set the detection duration to 10ms
  mpu.setInterruptPinLatch(true);	// Keep it latched.  Will turn off when reinitialized.
  mpu.setInterruptPinPolarity(true);
  mpu.setMotionInterrupt(true); // motion interrupt will be activated based on threshold and duration.

  Serial.println("");
  delay(100);
}

void get_motion_readings() {

  if(mpu.getMotionInterruptStatus()) {
    /* Get new sensor events with the readings */
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    ax = a.acceleration.x;
    ay = a.acceleration.y;
    az = a.acceleration.z;
    gx = g.acceleration.x;
    gy = g.acceleration.y;
    gz = g.acceleration.z;

    /* Print out the values */
    Serial.print("AccelX:");
    Serial.print(a.acceleration.x);
    Serial.print(",");
    Serial.print("AccelY:");
    Serial.print(a.acceleration.y);
    Serial.print(",");
    Serial.print("AccelZ:");
    Serial.print(a.acceleration.z);
    Serial.println(".");
    Serial.print("GyroX:");
    Serial.print(g.gyro.x);
    Serial.print(",");
    Serial.print("GyroY:");
    Serial.print(g.gyro.y);
    Serial.print(",");
    Serial.print("GyroZ:");
    Serial.print(g.gyro.z);
    Serial.println(".");
  }

}
