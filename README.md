# Blynk Water Tank Level Indicator with Alerts Feature
I have been using this project to measure sump (under ground) water tank water level with NodeMCU and ultrasonic sensor. 

<h2> Features</h2>
1) Calculates how many liters of water available in tank (Here we used rectangular tank)<br>
2) Calculates Percentage of water in tank<br>
3) Calculates distance of water level in cm<br>
4) Calculates distance of water level in feet<br>
5) Shows historical record of water level in cm, percentage and in liters<br>
6) Different types of Water Level Alerts by taking input from app)<br>
7) Reset All Alerts feature to reset one time alert flag <br>
8) Water coming alert (doesn't works properly as sensor data fluctuates)<br>
9) Wifi signal strength indicator<br>

<h2>Requirements</h2>
1) Blynk App <br>
2) NodeMCU <br>
3) Ultrasonic sensor (I used HCSRO4)<br>
<h2> Installation </h2>
Open settings.h and change Bynk Auth code, Wifi settings, server settings and flow calibration.

<h2>Hardware Connection </h2>
<img src="/images/nodemcu-to-ultrasonic-sensor-connection.png" width="400" height=" " alt="NodeMCU to Ultrasonic Sensor Connection" title="NodeMCU to Ultrasonic Sensor Connection">

<h3>Ultrasonic Sensor to NodeMCU Connection</h3>

5V/VCC to Vin
Trig to D6/12
Echo to D5/14
GND  to GND

<h2>Software Setup </h2>

1) Download and install the Blynk Mobile App for iOS or Android.<br>

2) Scan the QR code at the bottom of this page to clone the screenshot below, or create a new project yourself and manually arrange and setup the widgets.<br>

3) Email yourself the Auth code.<br>

4) Download this repo and copy the files in to your sketches directory. Open the sketch in Arduino IDE.<br>

5) Go to the settings.h tab. This is where all the customisable settings are. You should be able to change almost everything from there before compiling.<br>

<h2>Screenshots</h2>

<img src="/images/water-tank-level-blynk-screenshot-1.png" width="400" height=" " alt="Water tank level screenshot 1" title="Water tank level screenshot 1"><br>

<img src="/images/water-tank-level-blynk-screenshot-2.png" width="400" height=" " alt="Water tank level screenshot 2" title="Water tank level screenshot 2"><br>


<h2>Scan QR Code on Blynk App</h2>

<img src="/images/blynk-water-tank-level-qr-code.png" width="400" height=" " alt="Water tank level blynk QR Code" title="Water tank level blynk QR Code"><br>
