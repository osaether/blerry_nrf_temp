This application implements a temperature sensor using the nRF52xx and `TMP112 <https://www.ti.com/product/TMP112>`_ compatible with the
`Tasmota Blerry project <https://github.com/tony-fav/tasmota-blerry>`_.

To build this project you need the `nRF Connect SDK <http://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest>`_.
For instructions on how to install the nRF Connect SDK see the `nRF Connect SDK getting started guide <http://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/getting_started.html>`_.
After you have installed the SDK, copy the files in this repo to a subfolder of the nRF Connect SDK (e.g samples/blutetooth/blerry_nrf_temp) and
build with this command:

``west build -b <board name>``

Where `board name` is one of the names in the leftmost column in the table below.

The I2C pins for connecting the TMP112 are the default ones for each board as seen in the following table.

.. list-table:: TMP112 pins
   :widths: 30 25 25 25 25
   :header-rows: 1

   * - Board name
     - SDA
     - SCL
     - V+
     - GND
   * - nrf52840dk_nrf52840
     - P0.26
     - P0.27
     - VDD
     - GND
   * - nrf52840dongle_nrf52840
     - P0.26
     - P0.27
     - VDD
     - GND
   * - nrf52dk_nrf52832
     - P0.26
     - P0.27
     - VDD
     - GND
   * - nrf52_adafruit_feather
     - SDA
     - SCL
     - 3.3V
     - GND
