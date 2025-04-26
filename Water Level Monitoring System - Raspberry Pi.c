#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <lcd.h>


#define LED_LOW 6       
#define LED_MEDIUM 5    
#define LED_HIGH 4      
#define BUZZER_PIN 8    
#define BUTTON_PIN 7    


#define TANK_HEIGHT 50.0   
#define WATER_DENSITY 0.997 

#define LCD_ADDR 0x27   
#define LCD_COLS 16    
#define LCD_ROWS 2    
int lcdHandle;        

int pressureSensor;      
#define MS5837_ADDR 0x76 

bool alarmEnabled = true; 


bool initPressureSensor() {
    pressureSensor = wiringPiI2CSetup(MS5837_ADDR);
    if (pressureSensor == -1) {
        printf("Failed to initialize pressure sensor!\n");
        return false;
    }
    
    
    return true;
}


float readPressure() {
    unsigned char data[3];
    wiringPiI2CWrite(pressureSensor, 0x00);
    read(pressureSensor, data, 3);
    
    long pressure = (data[0] << 16) | (data[1] << 8) | data[2];
    return (float)pressure / 100.0;
}


float calculateWaterLevel(float pressure) {
    float pressureInCm = pressure / 100.0;
    
    if (pressureInCm < 0) return 0;
    if (pressureInCm > TANK_HEIGHT) return TANK_HEIGHT;
    return pressureInCm;
}


void setLEDs(bool low, bool medium, bool high) {
    digitalWrite(LED_LOW, low ? HIGH : LOW);
    digitalWrite(LED_MEDIUM, medium ? HIGH : LOW);
    digitalWrite(LED_HIGH, high ? HIGH : LOW);
}


void triggerAlarm() {
    for (int i = 0; i < 3; i++) {
        digitalWrite(BUZZER_PIN, HIGH);
        delay(500);
        digitalWrite(BUZZER_PIN, LOW);
        delay(500);
    }
}


int main() {
    if (wiringPiSetup() == -1) {
        printf("Failed to initialize wiringPi!\n");
        return 1;
    }
    
    lcdHandle = lcdInit(LCD_ROWS, LCD_COLS, 4, LCD_ADDR, 0);
    if (lcdHandle == -1) {
        printf("Failed to initialize LCD!\n");
        return 1;
    }
    lcdHome(lcdHandle);
    lcdPrintf(lcdHandle, "Water Level:");
    
    if (!initPressureSensor()) {
        return 1;
    }
    
    pinMode(LED_LOW, OUTPUT);
    pinMode(LED_MEDIUM, OUTPUT);
    pinMode(LED_HIGH, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT);
    pullUpDnControl(BUTTON_PIN, PUD_UP);
    
    while (1) {
        if (digitalRead(BUTTON_PIN) == LOW) {
            alarmEnabled = !alarmEnabled;
            delay(300);
            lcdPosition(lcdHandle, 0, 1);
            lcdPrintf(lcdHandle, alarmEnabled ? "Alarm:ON  " : "Alarm:OFF ");
        }
        
        float pressure = readPressure();
        float waterLevel = calculateWaterLevel(pressure);
        float waterPercentage = (waterLevel / TANK_HEIGHT) * 100;
        
        lcdPosition(lcdHandle, 0, 1);
        lcdPrintf(lcdHandle, "Level:%.1f%% ", waterPercentage);
        
        if (waterLevel <= 0) {
            setLEDs(false, false, false); 
        } 
        else if (waterPercentage < 30) {
            setLEDs(true, false, false); 
        } 
        else if (waterPercentage < 70) {
            setLEDs(false, true, false); 
        } 
        else {
            setLEDs(false, false, true); 
            if (waterPercentage > 90 && alarmEnabled) {
                triggerAlarm(); 
            }
        }
        
        delay(1000); 
    }
    
    return 0;
}
