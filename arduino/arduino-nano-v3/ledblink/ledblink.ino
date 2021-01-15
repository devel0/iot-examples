int led = LOW;

void setup()
{
    Serial.begin(115200);
    Serial.println("SETUP");

    pinMode(LED_BUILTIN, OUTPUT);

    digitalWrite(LED_BUILTIN, led);
}

unsigned long start = millis();

void loop()
{
    unsigned long m = millis();

    if (m - start > 500)
    {
        digitalWrite(LED_BUILTIN, led);
        led = led == HIGH ? LOW : HIGH;

        start = m;
    }
}
