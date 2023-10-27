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
### Home page

![IMG_3440](https://github.com/KyrianBunel/Automatic-Watering-System/assets/136705314/23ac50d8-a991-4f8e-9b6d-126abe4a75db)

### Dashboard

![IMG_3442](https://github.com/KyrianBunel/Automatic-Watering-System/assets/136705314/78d3fa6a-2e36-4462-bc9b-9edaf7544a07)
