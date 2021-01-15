volatile bool state = false;

void tim2_cb(void)
{
    digitalWrite(LED_BUILTIN, state ? HIGH : LOW);
    state = !state;
}

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);

    TIM_TypeDef *Instance = TIM2;

    HardwareTimer *MyTim = new HardwareTimer(Instance);

    MyTim->setOverflow(1, HERTZ_FORMAT);
    //MyTim->setInterruptPriority(0, 0); // highest prio
    MyTim->attachInterrupt(tim2_cb);
    MyTim->resume();
}

void loop()
{
   
}
