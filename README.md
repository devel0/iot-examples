# iot-examples

- [description](#description)
- [quickstart](#quickstart)
- [arduino](#arduino)
  - [examples](#examples)
  - [development](#development)
- [platformio](#platformio)
  - [examples (mbed)](#examples-mbed)
  - [examples (arduino)](#examples-arduino)
  - [development](#development-1)
  - [troubleshoot debug](#troubleshoot-debug)
- [stm32duino](#stm32duino)
  - [examples](#examples-1)
  - [development](#development-2)
- [hardware](#hardware)
  - [arduino nano V3](#arduino-nano-v3)
  - [nucleo-64 F446RE](#nucleo-64-f446re)
    - [troubleshoot](#troubleshoot)
  - [nucleo-144 F767ZI](#nucleo-144-f767zi)
  - [raspberry pi 4](#raspberry-pi-4)

## description

Examples for various

- boards ( arduino nano, stm32 )
- frameworks ( arduino, stm32duino, mbed )
- development environments ( vscode arduino, platformio )

## quickstart

```sh
git clone https://github.com/devel0/iot-examples
cd iot-examples
code arduino/ledblink
```

## arduino

### examples

- arduino nano v3
  - [ds18b20](arduino/arduino-nano-v3/ds18b20)
  - [ledblink](arduino/arduino-nano-v3/ledblink)
  - [nano-as-serial-reader](arduino/arduino-nano-v3/nano-as-serial-reader)
  - [rotary-encoder](arduino/arduino-nano-v3/rotary-encoder)
  - [signal-gen](arduino/arduino-nano-v3/signal-gen)

### development

- prerequisites
  - [vscode](https://code.visualstudio.com/)
  - [arduino ide](https://www.arduino.cc/en/software)
  - [vscode arduino](https://marketplace.visualstudio.com/items?itemName=vsciot-vscode.vscode-arduino)    
- troubleshoot
  - `avrdude: stk500_getsync() attempt 1 of 10: not in sync` uploading on arduino nano ; set `"configuration": "cpu=atmega328old",` on `.vscode/arduino.json`
  - usb permission error on linux: add your user to `dialout` group and set `/etc/udev/rules.d/99-arduino-usb.rules` to automodify arduino serial permission with follow ( check your idVendor:idProduct through `lsusb` ) `SUBSYSTEMS=="usb", ATTRS{idVendor}=="1a86", ATTRS{idProduct}=="7523", MODE="0666"`
- debug
  - using `Serial.print` through USB cable
  - C-S-p `Arduino: Upload` then C-S-p `Arduino: Open Serial Monitor`
- increase build speed setting `.vscode/arduino.json` with `{ "output": "./build" }`

## platformio

### examples (mbed)

- bluepill F103C8
  - [hello-world](platformio/bluepill-F103C8/mbed/hello-world)
- nucleo-64-F446RE
  - [hello-world](platformio/nucleo-64-F446RE/mbed/hello-world)
- nucleo-144-F767ZI
  - [hello-world](platformio/nucleo-144-F767ZI/mbed/hello-world)

### examples (arduino)

- bluepill F103C8
  - [temp-sensors2](platformio/bluepill-F103C8/arduino/temp-sensors2)
- ds18b20
  - [ds18b20](platformio/arduino-nano-v3/ds18b20-nano)

### development

- prerequisites
  - [vscode](https://code.visualstudio.com/)
  - [platformio](https://marketplace.visualstudio.com/items?itemName=platformio.platformio-ide)
- troubleshoot
  - receive `conflicted extension arduino` go to vscode extensions and from arduino ext select from dropdown `Disable (Workspace)`
  - receive `Error: init mode failed (unable to connect to the target)` on upload ; solve by hit RESET btn until yellow `srst_only separate srst_nogate srst_open_drain connect_deassert_srst` not appears on terminal
  - receive `[upload] Error` even `Verified OK` ; watch at monitor C-A-s and press RESET to retry start program
  - receive `Warn : UNEXPECTED idcode: 0x2ba01477 ; Error: expected 1 of 1: 0x1ba01477` ; can set `upload_flags = -c set CPUTAPID 0x2ba01477` on platformio.ini
- debug
  - using `printf` output go to monitor ( C-S-p `PlatformIO: Serial Monitor` ) ; it goes over usb cable used for programming to stlink connector
  - through C-S-p `PlatformIO: Start Debugging` ( working for platformio 5.0.4 )

### troubleshoot debug

- do a platformio/cleanup before

- with platformio/upload receive

```
xPack OpenOCD, x86_64 Open On-Chip Debugger 0.10.0+dev-00378-ge5be992df (2020-06-26-09:27)
Licensed under GNU GPL v2
For bug reports, read
        http://openocd.org/doc/doxygen/bugs.html
debug_level: 1

srst_only separate srst_nogate srst_open_drain connect_deassert_srst

Error: init mode failed (unable to connect to the target)
in procedure 'program'
** OpenOCD init failed **
shutdown command invoked

*** [upload] Error 1
```

- solve that by hitting RESET btn until `srst_only separate srst_nogate srst_open_drain connect_deassert_srst` then release and upload goes fine

- hit F5 to debug ( set a breakpoint on an instruction to check )

- if use `DigitalOut` on alternate function pins SWDIO/SWCLK may have problem during reset just after upload ( for example, avoid to use PA13, PA14 on STM32F446RE as DigitalOut )

## stm32duino

### examples

- bluepill F103C8
  - [hello-world](stm32duino/bluepill-F103C8/hello-world)
- nucleo-64-F446RE
  - [hello-world](stm32duino/nucleo-64-F446RE/hello-world)
  - [dac](stm32duino/nucleo-64-F446RE/dac)
- nucleo-144-F767ZI
  - [hello-world](stm32duino/nucleo-144-F767ZI/hello-world)

### development

- prerequisites
  - [vscode](https://code.visualstudio.com/)
  - [arduino ide](https://www.arduino.cc/en/software)
  - [vscode arduino](https://marketplace.visualstudio.com/items?itemName=vsciot-vscode.vscode-arduino)  
  - https://github.com/stm32duino/Arduino_Core_STM32
  - https://www.st.com/en/development-tools/stm32cubeprog.html
  - arm gcc `xpm install --global @xpack-dev-tools/arm-none-eabi-gcc@latest`
  - openocd `xpm install --global @xpack-dev-tools/openocd@latest`
- connect STLINK V2 ( 4 wires 3.3-swdio-swclk-gnd )
- debug
  - using `Serial.print`
    - bluepill: through USB-TTL connected to serial (eg. stm32f103c8 pins A9,A10)
    - nucleo64/144: through usb cable ( `/dev/ttyACM0` )
    - then C-S-p `Arduino: Upload` then C-S-p `Arduino: Open Serial Monitor`
  - with active debugger setting `.vscode/launch.json` ( see [hello-world](stm32duino/hello-world/.vscode/launch.json) example ) then press F5
- troubleshoot
  - if receive `Send Event AD7MessageEvent` on debug console try to run corresponding `openocd` with arguments given from debug console to test board connection; may you got `Warn : UNEXPECTED idcode: 0x2ba01477` and `Error: expected 1 of 1: 0x1ba01477` that you can fix in `.vscode/launch.json` by setting `"debugServerArgs": "-f interface/stlink-v2.cfg -c \"set CPUTAPID 0x2ba01477\" -f target/stm32f1x.cfg",`
  - if debugger start but breakpoint still grayed double check `.vscode/arduino.json` and see if `"configuration"` contains `opt=ogstd` that is Optimize for Debug (-g) while default is Optimize for Smallest (-Os default) on `opt=osstd`.
  - board:arduino nano
    - if upload fails check on `.vscode/arduino.json` if `"configuration": "cpu=atmega328old"`

## hardware

### arduino nano V3

![pinout](https://raw.githubusercontent.com/devel0/iot-examples/main/wirings/arduino-nano-v3-rot.svg)

- CPU 16Mhz / SRAM 2KB / EEPROM 1KB / FLASH 32KB
- power supply types
  - USB cable
  - `VIN` pin (unregulated 7-12V) to AMS1117-5.0
  - `5V` pin (regulated 5V)
- max current
  - I/O pins (40mA)
  - `3.3V` pin (50mA) from CH340
- 2 x hardware interrupt
- 8 x 10bit ADC
- 6 x PWM

### nucleo-64 F446RE

![pinout](https://raw.githubusercontent.com/devel0/iot-examples/main/wirings/nucleo64-F446RE.svg)

- CPU 180Mhz / SRAM 128KB / EEPROM 0KB / FLASH 512KB
- power supply types
  - USB cable (300mA max)
  - `VIN` CN6/pin8 or CN7/pin24 (unregulated 7-12V ; max 800mA)
  - `E5V` CN7/pin6 (regulated 5V ; max 500mA)        
  - `3V3` CN6/pin4 or CN7/pin12 (regulated 3V3) `[stlink debug unavail]`
- max current
  - I/O pins (25mA ; total 120mA)
  - `3.3V` pin (240mA)
- hardware interrupt on all digital pin
- 3 x 12bit ADC
- 2 x 12bit DAC

#### troubleshoot

- `libusb_open() failed with LIBUSB_ERROR_ACCESS`

```sh
apt install stlink-tools
systemctl restart udev
```

- create `/etc/udev/rules.d/90-stlink.rules` with content

```sh
SUBSYSTEMS=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="374b", MODE="0666
```

then reload udev

```sh
udevadm control --reload-rules && udevadm trigger
```

### nucleo-144 F767ZI

- [pinout](https://raw.githubusercontent.com/devel0/iot-examples/main/wirings/nucleo144-F767ZI.svg)
- CPU 216Mhz / SRAM 512KB / EEPROM 0KB / FLASH 2048KB
- power supply types
  - USB cable (300mA max)
  - `VIN` CN8/pin15 or CN11/pin24 (unregulated 7-12V ; max 800mA if VIN=7V ; max 450mA if 9>VIN>7 ; max 250mA if 12>VIN>9)
  - `E5V` CN11/pin6 (regulated 5V in 4.75-5.25V ; max 500mA)
  - `3V3` CN8/pin7 or CN11/pin6 (regulated 3V3 in 3-3.6V) `[stlink debug unavail]`
- max current
  - I/O pins (25mA ; total 120mA)
  - `3.3V` pin (420mA)
- hardware interrupt on all digital pin
- 3 x 12bit ADC
- 2 x 12bit DAC

### raspberry pi 4

- [pinout](https://raw.githubusercontent.com/devel0/iot-examples/main/wirings/raspberry-pi-4.svg)
