#include "ZumoHardware.h"

void ZumoHardware::readCalibrated(unsigned int sensorWaarden[]) {
}
int ZumoHardware::readLine(unsigned int values[]) {
    return sensors.readLine(values);
}

void ZumoHardware::setMotorSpeeds(int l, int r) {
    motors.setSpeeds(l, r);
}

void ZumoHardware::stopMotors() {
    motors.setSpeeds(0, 0);
}

void ZumoHardware::init() {
    sensors.initFiveSensors();

    unsigned int v[5];
    for (int i = 0; i < 200; i++) {
        sensors.calibrate();
        delay(10);
    }
}

void ZumoHardware::print(String text) {
}

void ZumoHardware::playDoneSound() {
}
