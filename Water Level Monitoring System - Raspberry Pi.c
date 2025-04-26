#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <lcd.h>

// تعريف أرقام المنافذ (Pin Definitions)
#define LED_LOW 6       // LED للمستوى المنخفض
#define LED_MEDIUM 5    // LED للمستوى المتوسط
#define LED_HIGH 4      // LED للمستوى العالي
#define BUZZER_PIN 8    // منفذ البيزر للإنذار
#define BUTTON_PIN 7    // منفذ الزر

// إعدادات النظام (System Constants)
#define TANK_HEIGHT 50.0    // ارتفاع الخزان بالسنتيمتر
#define WATER_DENSITY 0.997 // كثافة الماء (جم/سم³)

// إعدادات شاشة LCD (LCD Settings)
#define LCD_ADDR 0x27   // عنوان I2C للشاشة
#define LCD_COLS 16     // عدد أعمدة الشاشة
#define LCD_ROWS 2      // عدد صفوف الشاشة
int lcdHandle;         // مؤشر للشاشة

// متغيرات مستشعر الضغط (Pressure Sensor Variables)
int pressureSensor;    // مؤشر لمستشعر الضغط
#define MS5837_ADDR 0x76 // عنوان I2C للمستشعر

// متغيرات النظام (System Variables)
bool alarmEnabled = true; // حالة تفعيل الإنذار

/*******************************************************
* وظيفة: تهيئة مستشعر الضغط
* Input: لا يوجد
* Output: true إذا نجحت التهيئة، false إذا فشلت
*******************************************************/
bool initPressureSensor() {
    pressureSensor = wiringPiI2CSetup(MS5837_ADDR);
    if (pressureSensor == -1) {
        printf("Failed to initialize pressure sensor!\n");
        return false;
    }
    
    // كود تهيئة المستشعر حسب النموذج
    // (يجب استبداله بالكود الصحيح للمستشعر المستخدم)
    
    return true;
}

/*******************************************************
* وظيفة: قراءة قيمة الضغط من المستشعر
* Input: لا يوجد
* Output: قيمة الضغط بالمللي بار
*******************************************************/
float readPressure() {
    // كود قراءة الضغط من المستشعر
    // (يجب استبداله بالكود الصحيح للمستشعر المستخدم)
    unsigned char data[3];
    wiringPiI2CWrite(pressureSensor, 0x00);
    read(pressureSensor, data, 3);
    
    long pressure = (data[0] << 16) | (data[1] << 8) | data[2];
    return (float)pressure / 100.0; // تحويل إلى مللي بار
}

/*******************************************************
* وظيفة: حساب مستوى الماء من قيمة الضغط
* Input: قيمة الضغط بالمللي بار
* Output: مستوى الماء بالسنتيمتر
*******************************************************/
float calculateWaterLevel(float pressure) {
    // تحويل الضغط إلى ارتفاع عمود الماء
    float pressureInCm = pressure / 100.0;
    
    // التأكد من أن القيمة بين صفر وارتفاع الخزان
    if (pressureInCm < 0) return 0;
    if (pressureInCm > TANK_HEIGHT) return TANK_HEIGHT;
    return pressureInCm;
}

/*******************************************************
* وظيفة: التحكم في مصابيح LED
* Input: حالة كل LED (تشغيل/إطفاء)
* Output: لا يوجد
*******************************************************/
void setLEDs(bool low, bool medium, bool high) {
    digitalWrite(LED_LOW, low ? HIGH : LOW);
    digitalWrite(LED_MEDIUM, medium ? HIGH : LOW);
    digitalWrite(LED_HIGH, high ? HIGH : LOW);
}

/*******************************************************
* وظيفة: تشغيل الإنذار الصوتي
* Input: لا يوجد
* Output: لا يوجد
*******************************************************/
void triggerAlarm() {
    for (int i = 0; i < 3; i++) {
        digitalWrite(BUZZER_PIN, HIGH);
        delay(500);
        digitalWrite(BUZZER_PIN, LOW);
        delay(500);
    }
}

/*******************************************************
* الوظيفة الرئيسية
*******************************************************/
int main() {
    // تهيئة wiringPi
    if (wiringPiSetup() == -1) {
        printf("Failed to initialize wiringPi!\n");
        return 1;
    }
    
    // تهيئة شاشة LCD
    lcdHandle = lcdInit(LCD_ROWS, LCD_COLS, 4, LCD_ADDR, 0);
    if (lcdHandle == -1) {
        printf("Failed to initialize LCD!\n");
        return 1;
    }
    lcdHome(lcdHandle);
    lcdPrintf(lcdHandle, "Water Level:");
    
    // تهيئة مستشعر الضغط
    if (!initPressureSensor()) {
        return 1;
    }
    
    // تهيئة المنافذ
    pinMode(LED_LOW, OUTPUT);
    pinMode(LED_MEDIUM, OUTPUT);
    pinMode(LED_HIGH, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT);
    pullUpDnControl(BUTTON_PIN, PUD_UP);
    
    // الحلقة الرئيسية
    while (1) {
        // قراءة حالة الزر
        if (digitalRead(BUTTON_PIN) == LOW) {
            alarmEnabled = !alarmEnabled;
            delay(300); // لمنع ارتداد الزر
            lcdPosition(lcdHandle, 0, 1);
            lcdPrintf(lcdHandle, alarmEnabled ? "Alarm:ON  " : "Alarm:OFF ");
        }
        
        // قراءة الضغط وحساب مستوى الماء
        float pressure = readPressure();
        float waterLevel = calculateWaterLevel(pressure);
        float waterPercentage = (waterLevel / TANK_HEIGHT) * 100;
        
        // عرض مستوى الماء على الشاشة
        lcdPosition(lcdHandle, 0, 1);
        lcdPrintf(lcdHandle, "Level:%.1f%% ", waterPercentage);
        
        // التحكم في المصابيح حسب مستوى الماء
        if (waterLevel <= 0) {
            setLEDs(false, false, false); // كل المصابيح مطفاة
        } 
        else if (waterPercentage < 30) {
            setLEDs(true, false, false); // LED الأحمر فقط
        } 
        else if (waterPercentage < 70) {
            setLEDs(false, true, false); // LED الأصفر فقط
        } 
        else {
            setLEDs(false, false, true); // LED الأخضر فقط
            if (waterPercentage > 90 && alarmEnabled) {
                triggerAlarm(); // تشغيل الإنذار إذا تجاوز 90%
            }
        }
        
        delay(1000); // انتظر ثانية قبل القياس التالي
    }
    
    return 0;
}