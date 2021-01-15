void setup()
{
    Serial.begin(115200);    

    Serial.println("Hello, world!");
}

void loop()
{
   Serial.println(millis());
   delay(1000);
}
