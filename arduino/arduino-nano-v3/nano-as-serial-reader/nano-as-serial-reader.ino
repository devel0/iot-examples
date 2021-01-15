void setup()
{
  Serial.begin(115200);
}

void loop()
{
  while (Serial.available() > 0)
  {
    char c = Serial.read();
    Serial.write(c);
  }
}
