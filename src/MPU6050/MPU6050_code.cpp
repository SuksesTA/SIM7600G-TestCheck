#include "MPU6050.h"

Adafruit_MPU6050 mpu;

void mpuBegin() {
    if (!mpu.begin()) {
        Serial.println("Failed to find MPU6050 chip");
        while (1) { delay(10); }
    }
    
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
}

mpuReading getMPU() {

    static unsigned long lastTime = millis();

    mpuReading reading;
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    unsigned long currentTime = millis();
    float deltaTime = (currentTime - lastTime) / 1000.0;
    lastTime = currentTime;

    reading.accel.x = abs(a.acceleration.x * deltaTime);
    reading.accel.y = abs((a.acceleration.y * deltaTime) - 2.4);
    reading.accel.z = abs(a.acceleration.z * deltaTime);

    return reading;
}
