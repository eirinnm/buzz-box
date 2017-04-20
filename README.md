# Buzz-box: Audio and light stimulus using Arduino.

This is a project using hardware developed at UCL so it's unlikely to be of use to anyone else.

## Hardware details
### Audio - amplifier shield

* Tone is sent on pin 3.
* Gain pins are 2 and 4
* Switch is connected to ground and pin 12.

### Lights - RGB shield

This uses pins 3, 5 and 6. Use the channels connected to 5 or 6 so it doesn't interfere with the audio.

### Button

The button can be configured to do different things. By default, it toggles the lights. If a jumper is connected between pin 11 and Gnd, the button instead sends an audio pulse.

### External LED

An LED can be connected to pin 13 and Gnd (with a resistor). It will turn on for the duration of a stimulus.

## Software details

Connect to the Arduino using UART at 115200 baud. Commands:

* "on" / "off" - turn lights fully on or off.
* B<0-255> - set lights to this brightness
* F<brightness>,<duration> - set the lights to this brightness for a certain amount of time (ms)
* S\<vol\>,\<duration\> - play a tone for this duration (ms). Volumes are 0-4, where 0 is no sound.
* S\<vol\>,\<duration\>,\<wait\>,\<vol2\>,\<duration2\> - send two audio pulses with a delay between them
* H<freq> - set the freqency of future tones
