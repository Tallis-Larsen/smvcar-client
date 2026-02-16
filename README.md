# SMVCar-Client

## Setup

The Raspberry Pi delivered to KHS has already had the drivers installed, and has been configured to run the client application on startup. The setup process is quite lengthy and complex, and it is unlikely that you will need to set up a new Raspberry Pi for the foreseeable future. If, for whatever reason, you DO need to set up another Raspberry Pi (or redo the setup process for the current one), then continue reading.

* To install the screen and screen drivers, follow [this](https://osoyoo.com/2026/02/09/osoyoo-3-5-spi-screen-working-with-raspberry-pi-os-bookworm-trixieversion-2025-10-01-and-later/) guide.

* Information on how to download and build the source code is provided in the `Building` section of this readme.

* You will need to add the following line the Pi's `~/.bash_profile` to make the client app run at startup:
  
  ```bash
  sudo QT_QPA_PLATFORM=linuxfb /home/smvcar1/smvcar-client/cmake-build-debug/smvcar-client
  ```

* Connect the clicker to the Pi using `bluetoothctl` and set it to auto-connect when detected (there are many tutorials for this online).

While these 4 steps may seem simple, doing it for the first time will likely prove to be a long and difficult process (unless you are very familiar with Linux and command-line tools).

## Operation

### Power On/Off

To turn on the Raspberry Pi, simply plug the USB-C port into an adequate power supply. The Pi will automatically run the client software after booting. The Pi can be turned off by pressing the small button next to the green LED, then unplugging the power once the shutoff process has completed.

### Connect to Wi-Fi

To connect to your Wi-Fi hotspot, you will need to connect a keyboard over USB to the Raspberry Pi. (As long as the hotspot's name and password do not change, you will only need to do this once.) Then, press `CTRL + W` on the keyboard to open the Wi-Fi dialogue. Use `Tab` to change focus to the list of networks, then use up and down arrows to select the correct network. Press `Tab` again to change focus to the password field once the desired network is highlighted. Enter the password, use `Tab` again to change focus to the `Connect` button, and press `Enter` to confirm. You were successful if the Wi-Fi status indicator changes to `Connected`.

### Connect to Server

To connect to the [server](https://github.com/Tallis-Larsen/smvcar-server), you will need to connect a keyboard over USB to the Raspberry Pi. (As long as the server's url does not change, you will only need to do this once.) Then, press `CTRL + S` on the keyboard to open the server dialogue. Use `Tab` to change focus to the url field and input the url provided by Digital Ocean, starting with `wss://`. Examples on how this link should look exist in the [smvcar-server readme](https://github.com/Tallis-Larsen/smvcar-server). Use `Tab` again to change focus to the `Connect` button and press `Enter` to confirm. You were successful if the server status indicator changes to `Connected`.

### Start/Stop/Reset

The `start`, `stop`, and `reset` functions are all mapped to the bottom button on the clicker. Simply press it once to start the stopwatch, press again to stop, and press once more to reset.

> NOTE: For networking simplicity purposes, only the Raspberry Pi client has the ability to start, stop, and reset the stopwatch. Because of this, make sure that the driver remembers to start the timer, because the pit crew will not be able to do it for them.

### Lapping

The `lap` function is mapped to the top button of the clicker. Simply press it once to record a lap. The HTML client served by the server (and accessible from the pits) can also record laps. Because of this, any laps recorded within 5 seconds of each other will have the latter one rejected. This way, both the driver and the pit crew can attempt to record a lap, and the earlier one will be saved. This makes the lap times more accurate and allows for either the driver or the pit crew to forget to record a lap and still have a redundancy. This also means that if either client loses connection or has other technical difficulties during a race, the other can continue to record laps in their absence.

### Target Lap Time

The pit crew / HTML client has the ability to set 2 variables: `targetLaps` and `targetTime`. When both variables are set, `targetLapTime` is calculated and 2 things will happen: 

1. Entries in the laps list will change color depending on whether a lap is ahead or behind schedule.

2. The driver will see 2 progress bars at the top of their display. The top bar will show how much time is left in the race. The bottom bar will show how many laps out of the total target laps have been completed. The bottom bar also has a smaller "ghost" bar, which shows the driver where the bar *would* be if they were to finish a lap at that moment.

These features are intended to help both the driver and the pit crew stay on pace throughout the race, without having to do any manual calculations. Because both the time remaining and laps remaining bars are the same size, it is easy to compare the two to see how far ahead/behind schedule you are.

## Parts

### Clicker

A cheap AliExpress [clicker](https://www.aliexpress.us/item/3256806867641044.html) (and its charger) has been provided with the Raspberry Pi. This will automatically connect to the Pi over bluetooth when powered on. To power on the clicker, simply press and hold the middle button for a few seconds. Do the same to power it off. 

> NOTE: The clicker will sometimes register an input of the bottom button if the middle button is still being held when it connects to the Raspberry Pi. Be wary of this fact while driving as to not perform any misinputs.

A wireless clicker was chosen specifically for the fact that it can be mounted anywhere, without requiring a direct connection to the Pi. Feel free to mount this device wherever it works best, as long as the charging port and buttons are still accessible. You are welcome to cut off the plastic "ring" part of the casing, or even discard the enclosure entirely and just use the raw PCB.

> WARNING: The clicker will automatically power itself off after 5 minutes of inactivity. I have yet to find a way around this, as it is built into the firmware of the device. To prevent the clicker from turning off during laps longer than 5 minutes, quickly press the middle button once. This will not power off the device or register any input, but it will keep the clicker from turning off for another 5 minutes.

### Raspberry Pi

The Raspberry Pi requires a `5V/5A/27W` power supply to work at full functionality. If the neccessary power is not delivered, the device may still work, but many functions (including Wi-Fi connectivity) may be inhibited.

### Hotspot

Unfortunately, the attempt to use a Raspberry Pi 4G hat was a failure, so an external hotspot is required to connect the Pi to the internet and the server. Either a phone hotspot or a dedicated hotspot device will work, though a dedicated hotspot is recommended if the phone is experiencing connection issues. If you do decide to use a dedicated hotspot device, I recommend the [MiFi X Pro 5G](https://inseego.com/products/mobile-hotspot-routers/mifi-x-pro/) for the hardware, and the [Calyx Institute Sprout BYOD](https://calyxinstitute.org/news/2025/introducing-sprout-byod-membership) membership for the SIM card. You will have to purchase the MiFi X Pro device used on a site like E-Bay, and ensure that the device is **unlocked** so that it can be used with any carrier. The SIM card costs $150/quarter, but with the convencience that it is completely contract-free. While expensive, I can personally vouch for the quality of this combination as I have been using it as my primary internet for the past month. While the network speed does slow down in more remote areas, I have only ever seen it lose connection completely while driving through the middle of nowhere to get to the U.P.

## Building

This project is set up with CMake as the build system, and requires Qt6 as a dependancy. This guide assumes that you are using SSH to connect to the Raspberry Pi. To download the repository, run the following:

```bash
git clone https://github.com/Tallis-Larsen/smvcar-client.git
```

Before building, you will need to install the Qt6 and build-system development libraries. To do this, run the following in the terminal:

```bash
sudo apt update
sudo apt install -y cmake build-essential extra-cmake-modules qt6-base-dev libqt6websockets6-dev libkf6networkmanagerqt-dev
```

Lastly, you will need to actually build the binary. First, create and navigate to the build directory:

```bash
cd ~/smvcar-client
mkdir cmake-build-debug
cd cmake-build-debug
```

Then build:

```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build . --parallel
```

Done! Now you can run the program with:

```bash
sudo QT_QPA_PLATFORM=linuxfb ./smvcar-client
```
