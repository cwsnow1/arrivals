# CTA Train Tracker Map
## Setup
On initial boot, the device will start in Access Point mode. To configure, connect with the SSID and password displayed on screen and navigate in a web browser to the URL also on scren. Note that is it begins http, not https, and your browser may autocorrect to https.

On the configuration page, you will need to set the SSID and password for your Wi-Fi network in order to use the Live Tracking LED mode or the Arrivals LCD mode. Also required is an API key acquired from CTA. To obtain a key use this link, it is an automated process and takes less than a minute to receive a response: https://www.transitchicago.com/developers/traintrackerapply/.

After making your choices on the configuration page, click Save Config. Most options require a reboot to take effect.

Once the device is able to connect to a network, the config page will be reachable without having to go into Access Point mode. The IP address of the device is briefly displayed after connecting to the network and can be reached from a web browser in the same way as in AP mode. If the device is unable to connect to network, and you need it to enter AP mode to make changes, press the top-left button and the device will reboot into AP mode.

## Build from Source

Firmware images are available in the Releases page and, if the auto-update feature is enabled, are automatically fetched and applied. If, however, you would like to modify the firmware and flash your own custom image, here is how.

### Install ESP-IDF

See [this page](https://docs.espressif.com/projects/esp-idf/en/v5.5.2/esp32s3/get-started/linux-macos-setup.html) for complete instructions on installing ESP-IDF. Not that this project is based off the `release/v6.0` branch.

Ensure the following packages are installed:

`git wget flex bison gperf python3 python3-pip python3-venv cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0`

Clone the [repository](https://github.com/espressif/esp-idf), checkout the `release/v6.0` branch, update git submodules, and run `install.sh esp32s3`.

### Build

Run `. <esp-idf location>/export.sh` to setup the environment variables needed to build, and finally build with `idf.py build`.

### Flash

To flash, connect the board via USB and run `idf.py flash`. Optionally give the port with `-p`, but it should be able to find the device automatically.
