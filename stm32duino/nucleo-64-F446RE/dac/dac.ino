#define DA_PWM_1 PA4
#define DA_PWM_2 PA5

// the setup function runs once when you press reset or power the board
void setup()
{
    Serial.begin(115200);
    Serial.println("START");
    pinMode(DA_PWM_1, OUTPUT);
    pinMode(DA_PWM_2, OUTPUT);
    analogWriteResolution(12);
    analogWrite(DA_PWM_1, 0);
    analogWrite(DA_PWM_2, 0);
    delay(1000);
    Serial.println("SET");
    analogWrite(DA_PWM_1, 4095);
    analogWrite(DA_PWM_2, 2047);
}

// the loop function runs over and over again forever
void loop()
{
}