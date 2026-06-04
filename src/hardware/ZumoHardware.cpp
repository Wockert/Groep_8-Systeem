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

    // Kalibreren terwijl de robot op zijn plek heen en weer draait,
    // zodat de sensoren vanzelf over zwart EN wit vegen.
    // Zet hem met de sensoren boven de lijn neer voordat hij aangaat.
    for (uint16_t i = 0; i < 120; i++) {
        if (i > 30 && i <= 90)
            motors.setSpeeds(-200, 200);   // naar de ene kant draaien
        else
            motors.setSpeeds(200, -200);   // naar de andere kant terug
        sensors.calibrate();
    }
    motors.setSpeeds(0, 0);                 // weer stilstaan na kalibratie
}

void ZumoHardware::print(String text) {
}

void ZumoHardware::playDoneSound() {
}
