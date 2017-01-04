# 16hacku_zabuton
## About
This repository is source codes of IoT-Zabuton that displayed [hacku 2016 osaka](http://hacku.yahoo.co.jp/hacku2016osaka/).  

This repository consists of two elements programs.
+ server (nodejs and HTML)
+ mbed

## Requirement packages
### server
* node.js (v6.9.0+)
* npm  
Requirement npm packages are written in file "packages.json".
Therefore, please enter the command ```$ npm install ```.
* jQuery (v1.9.1+)
* Bootstrap3
* [jQuery UI](https://jqueryui.com/)
* [encoding.js](https://github.com/polygonplanet/encoding.js)
* [amCharts](https://www.amcharts.com/)

### mbed
All requirement libraries are exists in [https://developer.mbed.org](https://developer.mbed.org)  

mbed1_main.cpp
* ADXL345_I2C.h
* wave_player.h
* rtos.h
* SDFileSystem.h

mbed2_main.cpp
* Websocket.h
* EthernetNetIf.h
* MbedJSONValue.h

## collaborators
+ wezard706 (https://github.com/wezard706/)
+ K.K (no github accounts)
+ Y.Y (no github accounts)
