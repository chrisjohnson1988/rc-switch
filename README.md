# RC-Switch

RC-Switch is a port of the Arduino Library for controlling radio controlled power outlets to the raspberry pi.

The project produces a command line utility. Which can be executed as a non-root user, as long as a version of [wiringpi](http://wiringpi.com/download-and-install/) is installed which supports `WIRINGPI_GPIOMEM` ([2.29 or later](http://wiringpi.com/wiringpi-update-to-2-29/))

## Usage

### Connecting your 433mhz transmitter to the GPIO pins

By default the `send` assumes that your data pin of the 433mhz transmitter is connected to BCM GPIO pin 17 ([wiringPi pin 0](https://projects.drogon.net/raspberry-pi/wiringpi/pins/)). See the image below for example wiring.

![Wiring Diagram](https://rcplugs.info/static/img/connect_transmitter_to_raspberrypi.png)

To achieve the best range, connect a 17.3cm straight piece of wire as an antenna.

### Building

1. Install the latest version of [wiring pi](http://wiringpi.com/download-and-install/)
2. Compile the project

        make

3. Optionally copy the output to your PATH

        cp send /usr/bin/send

## Acknowledgements
- [ninjablocks 433Utils](https://github.com/ninjablocks/433Utils)
- [Arduino RC-Switch Library](https://github.com/sui77/rc-switch)
- [Controlling 433Mhz RF outlets](https://www.raspberrypi.org/forums/viewtopic.php?f=37&t=66946)