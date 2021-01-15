HardwareTimer* tim2;

volatile int state = LOW;

void tim2_cb()
{
    digitalWrite(PB3, state);
    state = !state;
}

void setup()
{
    uint32_t freq = 10000;    
    float period = 1.0 / freq; 
    uint32_t half_period_us = round(period * 1e6 / 2);

    pinMode(PB3, OUTPUT);

    {
        tim2 = new HardwareTimer(TIM2);
        
        tim2->setOverflow(half_period_us, MICROSEC_FORMAT);
        //tim2->setInterruptPriority(0, 0); // highest prio
        tim2->attachInterrupt(tim2_cb);
        tim2->resume();
    }      

}

void loop()
{
}
