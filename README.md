# Automatic-Watering-System
esp32-based automatic irrigation system capable of controlling up to 6 different zones


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
- Ability to view the current program via a dashboard page (LOCAL_IP_ADDRESS/dashboard ex : 192.168.1.10/dashboard)
- Compatible with arduino OTA (update through the local network)
- Transmits sensor data hourly to a Google sheet to create a database

# Future improvements :
- Web pages :
    - add a settings page
    - add the possibility of choosing the language (English/French)
    - add the ability to modify the Google Sheet page ID
- PCB :
    - upgrade to an ESP32-WROOM-32UE to add an external antenna and increase the WiFi connection range
    - move mounting holes & components away from board edge
    - increase mounting hole size to M3
    - move decoupling capacitors closer to power supply pins
    - add a differential pair to the usb port

# How to start up :
- Download the code and upload it to the ESP
- Press the reset button
- Go to the phone settings and connect to the Wifi network: ArrosageAUTO_XXX
- Wait for a web page to open. If the web page doesn't open automatically, go to your browser and type "192.168.4.1"
- Click on "Configure WiFi"
- select your wifi network and enter your password
- Click on "Save"
- And you're done, you can now access the following web pages: LOCAL_IP_ADDRESS/accueil & LOCAL_IP_ADDRESS/dashboard

# Web pages :
### Home page & Dashboard
<img width="482" alt="Capture d’écran 2023-11-24 à 09 25 02" src="https://github.com/KyrianBunel/Automatic-Watering-System/assets/136705314/00163eab-d90e-48f1-a0c9-2fe2f81a1990">

### Extract from the database
<img width="903" alt="Capture d’écran 2023-11-24 à 09 27 08" src="https://github.com/KyrianBunel/Automatic-Watering-System/assets/136705314/ae5eccb6-c697-4cf3-a932-965565101b48">
