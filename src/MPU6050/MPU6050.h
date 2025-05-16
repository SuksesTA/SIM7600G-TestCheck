#ifndef MPU6050_H
#define MPU6050_H

#include <Arduino.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

typedef struct {
    struct {
        float x;
        float y;
        float z;
    }gyro,accel;
} mpuReading;

// Function declarations
void mpuBegin();
mpuReading getMPU();

#endif // MPU6050_H
