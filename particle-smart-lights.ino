#define LIGHTS_OUT D2

enum LightStatus {
  OFF = 0,
  ON = 1
};

struct LightSettings {
  LightStatus lightStatus;
  time32_t timeOn;
  time32_t timeOff;
  int brightness;
};

// Defaults
struct LightSettings lightSettings = {
    ON,
    NULL,
    NULL,
    100
};

time32_t convertToTime(String timeString) {
    struct tm tm;
    strptime(timeString, "%H:%M:%S", &tm);
    time32_t convertedTime = mktime(&tm);
    return convertedTime;
}

int setLightBrightness(String lightBrightness) {
    lightSettings.brightness = atoi(lightBrightness);
    updateLightBrightness();
    return 1;
}

int turnOnLight(String lightStatus) {
    lightSettings.lightStatus = ON;
    updateLightStatus();
    return 1;
}

int turnOffLight(String lightStatus) {
    lightSettings.lightStatus = OFF;
    updateLightStatus();
    return 1;
}

int setTimeOn(String timeOn) {
    lightSettings.timeOn = convertToTime(timeOn);
    updateLightStatus();
    return 1;
}

int setTimeOff(String timeOff) {
    lightSettings.timeOff = convertToTime(timeOff);
    updateLightStatus();
    return 1;
}

void updateLightStatus() {
    if (lightSettings.lightStatus == ON) {
        digitalWrite(LIGHTS_OUT, HIGH);
        Serial.println("ON");
    } else {
        digitalWrite(LIGHTS_OUT, LOW);
        Serial.println("OFF");
    }
}

void updateLightBrightness() {
    // 0 (always off) and 255 (always on)
    const int OFF_OUTPUT = 0;
    const int ON_OUTPUT = 255;
    
    const float brightnessPercentage = (lightSettings.brightness/100)*ON_OUTPUT;
    analogWrite(LIGHTS_OUT, brightnessPercentage);
    Serial.println("DIMMER");
}

void setup()
{
    Serial.begin(9600);
    pinMode(LIGHTS_OUT, OUTPUT);
    
    // Defining cloud functions
    Particle.function("setLightBrightness", setLightBrightness);
    Particle.function("turnOnLight", turnOnLight);
    Particle.function("turnOffLight", turnOffLight);
    Particle.function("setTimeOn", setTimeOn);
    Particle.function("setTimeOff", setTimeOff);
    
    // Defining cloud variables
    // Particle.variable("lightSettings", lightSettings);
    // Particle.variable("lightStatus", lightSettings.lightStatus);
    // Particle.variable("lightBrightness", lightSettings.brightness);
    
    updateLightStatus();
}

void loop()
{
    if (lightSettings.timeOn != NULL
    && lightSettings.timeOff != NULL) {
        time32_t currentTime = Time.now();
        
        if (currentTime >= lightSettings.timeOn
        && currentTime <= lightSettings.timeOff) {
            Serial.println("timeOn");
            lightSettings.lightStatus = ON;
            lightSettings.brightness = 100;
            updateLightStatus();
        }
        
        if (currentTime <= lightSettings.timeOn
        || currentTime >= lightSettings.timeOff) {
            Serial.println("timeOff");
            lightSettings.lightStatus = OFF;
            updateLightStatus();
        }
    }
}
