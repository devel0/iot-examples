# arduino - ds18b20

## wirings

![](wirings.png)

- ds18b20
    - power 3-5.5v
    - data line
        - to PD3 ( or PD2 )
        - pull-up with 4.7 kΩ to VCC

## notes

- multiple thermometer can be connected using single data wire
- each device has its own hardware address ( printed when program starts )
