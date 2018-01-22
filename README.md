# Getting started with LoRa using Arm and The Things Network

This repository contains sample code for [the webinar](https://os.mbed.com/blog/entry/Getting-started-with-LoRa-using-Arm-Mbed/) presented 29 November 2017 by [Jan Jongboom](https://github.com/janjongboom) (Developer Evangelist IoT, Arm Mbed) and [Johan Stokking](https://github.com/johanstokking), CTO and Co-Founder, The Things Industries.

The video of the webinar, and the Q&A are [here](https://os.mbed.com/blog/entry/Getting-started-LoRa-Mbed-Things-Network/).

## Requirements

- [L-Tek IoT FF1705](https://l-tek.si/web-shop/ltek-iot-ff1705/)

## Setup

First, add your device in the [TTN console](http://console.thethingsnetwork.org/). Then you can import and compile this application.

1. Go to [os.mbed.com](https://os.mbed.com) and sign in (or create an account).
1. Go to the [Multi-Tech xDot platform page](https://os.mbed.com/platforms/MTS-xDot-L151CC/) and click *Add to your compiler*.
1. Open the [Mbed Online Compiler](https://os.mbed.com/compiler).
1. Click *Import*.
1. Click *Click here to import from URL*.
1. Enter `https://github.com/johanstokking/arm-ttn-webinar`.
1. Click *Import*.
1. Open `ttn_config.h` and enter the application EUI and application key from the TTN console.
1. Make sure that you have selected the Multi-Tech xDot platform in the top right corner.
1. Click *Compile*.
1. A `.bin` file downloads. Drag this file to your board (mounted as a USB mass-storage device).
1. When flashing is complete, hit the `RESET` button on the board.
1. Attach a [serial monitor](https://os.mbed.com/handbook/SerialPC) to see debug messages.
