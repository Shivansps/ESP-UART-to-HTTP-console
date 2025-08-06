# ESP UART to HTTP Console
This project allows to create a in-browser console to read and send commands to a UART port using a ESP8266 or ESP-01.
Simply add your wifi SSID and password to the sketch and you are ready to go.

## How it works

- The microcontroller connects to the provided wifi SSID and Password
- Then you access the IP the MCU is assigned to in a browser
- You are ready to send and recieve messages.

## Features
- Bi-directional communication
- Websockets for realtime streaming
- Changing baudrate from the browser interface and saving the value to be used again next time (saved to flash with LittleFS)
- Saving UART data to ran if no clients are connected, and sending it once a client connects. (warning, ESP-01/ESP8266 have limited ram and they will likely crash and reset once its full)

## Potential Issues
This was developed to be able to read SBC UART port for debugging without having to attach a USB UART every time i wanted to do so or i would not have SSH access, and ESP-01 modules are excellent for this. While this can be effectively used as a remote UART console, it was never intended to be used for extended periods of time. There is no security, eveyone that can see the MCU IP can access to the console, and the websocket implementation is not exactly robust. There is no ping/pong or auto-reconnects if the connection is dropped. But it will warn you about it.

Remember to check that the device you are attaching to uses 3.3v logic.

## RS-232?
Yes, you can use this to connect to standard RS232 serial port and use it remotely. But you will need to use MAX3232 to drive the connection. Do not try to connect the MCU to the RS232 directly.

## Reading UART port from a Lichee PI 3A using a ESP-01
![connectingESP01](https://i.imgur.com/zBzLiJ5.jpeg)

![webconsole](https://i.imgur.com/ScySDww.png)
