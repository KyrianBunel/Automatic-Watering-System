# Automatic-Watering-System
Esp32-based automatic irrigation system capable of controlling up to 6 different zones independently


# Features :
- Create a wifi access point to enter wifi information
- Automatically connects to the local network if available
- Compatible with a weather station included in the project to collect temperature, humidity and brightness data, also features a rain sensor
- Able to use up to 3 soil moisture sensors
- Controllable through a web page (LOCAL_IP_ADDRESS/accueil ex : 192.168.1.10/accueil)
    - 3 operating modes :
  
          - MANU -> allows manual control of the 6 valves independently; once the selected valve is active, it can be deactivated only if the user does it manually.
          - AUTO -> uses weather station and soil moisture sensors to automatically trigger a 15-minute watering cycle.
          - PROG -> allows the user to enter information such as watering start and end date, watering duration, recurrence and watering time for each valve via a form.
- Ability to view the current program and sensors values via a dashboard page (LOCAL_IP_ADDRESS/dashboard ex : 192.168.1.10/dashboard)
- Compatible with arduino OTA (update through the local network)
- Automatic watering stop in case of rain (except in MANU mode)
- Transmits sensor data hourly to a Google sheet to create a database

# Future improvements :
- Web pages :
    - Add a settings page
    - Add the possibility of choosing the language (French only for the moment)
    - Add the ability to modify the ID of Google Sheet page in the web interface
- Database :
    - Improve the data sent as the total duration of watering during the last hour to be able to deduct the cost of watering per month and per year
- PCB :
    - Upgrade to an ESP32-WROOM-32UE to add an external antenna and increase the WiFi connection range
    - Move mounting holes & components away from board edge
    - Increase mounting hole size to M3
    - Move decoupling capacitors closer to power supply pins
    - Add a differential pair to the usb port
    - Improve the DC-DC Buck Converter design (produce heat + noise coming from the inductance during current peaks)

# How to start up :
- Download the code and upload it to the ESP
- Press the reset button
- Go to the phone settings and connect to the Wifi network: ArrosageAUTO_XXX
- Wait for a web page to open. If the web page doesn't open automatically, go to your browser and type "192.168.4.1"
- Click on "Configure WiFi"
- select your wifi network and enter your password
- Click on "Save"
- And you're done, you can now access the following web pages: LOCAL_IP_ADDRESS/accueil & LOCAL_IP_ADDRESS/dashboard

# Additional material required :
- DHT22 temperature and humidity sensor
- Capacitive moisture sensor (3 maximum)
- 9V 24V tranformer (depending on your instlation)
- Rain sensor
- Wi-Fi network (a powerful network to allow pages to load faster)

# Web pages :
### Home page & Dashboard
<img width="482" alt="Capture d’écran 2023-11-24 à 09 25 02" src="https://github.com/KyrianBunel/Automatic-Watering-System/assets/136705314/ae5d1ec4-e019-4077-bf2f-7e15ebe8740a">

### Extract from the database
<img width="903" alt="Capture d’écran 2023-11-24 à 09 27 08" src="https://github.com/KyrianBunel/Automatic-Watering-System/assets/136705314/5a0231ec-6d6f-4ed7-8dd2-d076064bc66f">

(the data displayed is random, as no sensor was connected to the board)

# PCB :
<img width="1091" alt="Capture d’écran 2023-11-24 à 10 55 46" src="https://github.com/KyrianBunel/Automatic-Watering-System/assets/136705314/d388fdc2-1045-42d5-a5e7-aa54745b1d12">

You can directly order the PCB via this link: https://www.pcbway.com/project/shareproject/Automatic_Watering_System_7e1279b2.html

# Weather station :
