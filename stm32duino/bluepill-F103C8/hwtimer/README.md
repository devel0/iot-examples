# iot-stm32-ledblink-interrupt-debug

## description

![](sshot.png)

- sample application led blink
- uses TIM2 counter 1Hz
- configured for vscode debugging

## notes on vscode debugging

- prerequisites
    - https://github.com/stm32duino/Arduino_Core_STM32
    - https://www.st.com/en/development-tools/stm32cubeprog.html
    - https://github.com/xpack-dev-tools/arm-none-eabi-gcc-xpack/releases
- update `.vscode` json files paths accordingly to your tools path
- connect STLINK V2 ( 4 wires 3.3-swdio-swclk-gnd )

## troubleshoot debugging

if receive error like `Unable to start debugging. Debug server process failed to initialize.` it could related to the wrong cpuid in the openocd script file, then try to run openocd from console

```sh
openocd -f interface/stlink.cfg -f target/stm32f1x.cfg
```

and you way find

```
Warn : UNEXPECTED idcode: 0x2ba01477
Error: expected 1 of 1: 0x1ba01477
```

this case fix launch.json changing from

```json
"debugServerArgs": "-f interface/stlink-v2.cfg -f target/stm32f1x.cfg",            
```

to

```json
"debugServerArgs": "-f interface/stlink-v2.cfg -c \"set CPUTAPID 0x2ba01477\" -f target/stm32f1x.cfg",            
```