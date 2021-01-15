#define TIMER_INTERRUPT_DEBUG 0

#define USE_TIMER_1 true

#include <ISR_Timer.h>
#include <TimerInterrupt.h>

#define PWM_PIN 6
#define PWN_FREQ_HZ 995

int port_state = LOW;

void TimerHandler1()
{
    port_state = port_state == LOW ? HIGH : LOW;
    digitalWrite(PWM_PIN, port_state);
}

#define TIMER1_INTERVAL_MS 1

void setup()
{
    Serial.begin(115200);
    Serial.println("SETUP");

    pinMode(PWM_PIN, OUTPUT);
    digitalWrite(PWM_PIN, port_state);

    ITimer1.init();

    if (ITimer1.attachInterrupt(PWN_FREQ_HZ*4, TimerHandler1, 0))
    {
        Serial.println("timer init ok");
    }
    else
    {
        Serial.println("error init timer");
    }
}

void loop()
{
}
