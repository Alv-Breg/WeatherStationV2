# 1 "d:\\projects\\WeatherStationV2\\WeatherStationV2.ino"
/***************************

 * Library Includes 

 **************************/
# 4 "d:\\projects\\WeatherStationV2\\WeatherStationV2.ino"
# 5 "d:\\projects\\WeatherStationV2\\WeatherStationV2.ino" 2

# 7 "d:\\projects\\WeatherStationV2\\WeatherStationV2.ino" 2
# 8 "d:\\projects\\WeatherStationV2\\WeatherStationV2.ino" 2
# 9 "d:\\projects\\WeatherStationV2\\WeatherStationV2.ino" 2

# 11 "d:\\projects\\WeatherStationV2\\WeatherStationV2.ino" 2
# 12 "d:\\projects\\WeatherStationV2\\WeatherStationV2.ino" 2
# 13 "d:\\projects\\WeatherStationV2\\WeatherStationV2.ino" 2
# 14 "d:\\projects\\WeatherStationV2\\WeatherStationV2.ino" 2
# 15 "d:\\projects\\WeatherStationV2\\WeatherStationV2.ino" 2
# 16 "d:\\projects\\WeatherStationV2\\WeatherStationV2.ino" 2
# 17 "d:\\projects\\WeatherStationV2\\WeatherStationV2.ino" 2

# 19 "d:\\projects\\WeatherStationV2\\WeatherStationV2.ino" 2
# 20 "d:\\projects\\WeatherStationV2\\WeatherStationV2.ino" 2
# 21 "d:\\projects\\WeatherStationV2\\WeatherStationV2.ino" 2
# 22 "d:\\projects\\WeatherStationV2\\WeatherStationV2.ino" 2
# 23 "d:\\projects\\WeatherStationV2\\WeatherStationV2.ino" 2
# 24 "d:\\projects\\WeatherStationV2\\WeatherStationV2.ino" 2
# 25 "d:\\projects\\WeatherStationV2\\WeatherStationV2.ino" 2


/***************************

 * Begin Settings

 **************************/
# 31 "d:\\projects\\WeatherStationV2\\WeatherStationV2.ino"
// WIFI
const char* WIFI_SSID = "Huawei";
const char* WIFI_PWD = "qwertyui";




// Setup
const int UPDATE_INTERVAL_SECS = 20 * 60; // Update every 20 minutes

// Display Settings
const int I2C_DISPLAY_ADDRESS = 0x3c;

// Defined param I2_C bus

const int SDA_PIN = 4;
const int SDC_PIN = 5;






// OpenWeatherMap Settings
// Sign up here to get an API key:
// https://docs.thingpulse.com/how-tos/openweathermap-key/
String OPEN_WEATHER_MAP_APP_ID = "fc0ab4fc09f4cb2e2fe0b59a0a23d54a";
/*

Go to https://openweathermap.org/find?q= and search for a location. Go through the

result set and select the entry closest to the actual location you want to display 

data for. It'll be a URL like https://openweathermap.org/city/2657896. The number

at the end is what you assign to the constant below.

 */
# 64 "d:\\projects\\WeatherStationV2\\WeatherStationV2.ino"
String OPEN_WEATHER_MAP_LOCATION_ID = "709717";

// Pick a language code from this list:
// Arabic - ar, Bulgarian - bg, Catalan - ca, Czech - cz, German - de, Greek - el,
// English - en, Persian (Farsi) - fa, Finnish - fi, French - fr, Galician - gl,
// Croatian - hr, Hungarian - hu, Italian - it, Japanese - ja, Korean - kr,
// Latvian - la, Lithuanian - lt, Macedonian - mk, Dutch - nl, Polish - pl,
// Portuguese - pt, Romanian - ro, Russian - ru, Swedish - se, Slovak - sk,
// Slovenian - sl, Spanish - es, Turkish - tr, Ukrainian - ua, Vietnamese - vi,
// Chinese Simplified - zh_cn, Chinese Traditional - zh_tw.
String OPEN_WEATHER_MAP_LANGUAGE = "en";
const uint8_t MAX_FORECASTS = 4;

const boolean IS_METRIC = true;

// Adjust according to your language
const String WDAY_NAMES[] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
const String MONTH_NAMES[] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};

/***************************

 * End Settings

 **************************/
# 86 "d:\\projects\\WeatherStationV2\\WeatherStationV2.ino"
 // Initialize the oled display for address 0x3c
 // sda-pin=14 and sdc-pin=12
 SSD1306Wire display(I2C_DISPLAY_ADDRESS, SDA_PIN, SDC_PIN);
 OLEDDisplayUi ui( &display );

OpenWeatherMapCurrentData currentWeather;
OpenWeatherMapCurrent currentWeatherClient;

OpenWeatherMapForecastData forecasts[MAX_FORECASTS];
OpenWeatherMapForecast forecastClient;

BlueDot_BME280 bme280 = BlueDot_BME280();

File root;




time_t now;

// flag changed in the ticker function every 10 minutes
bool readyForWeatherUpdate = false;

String lastUpdate = "1727";

long timeSinceLastWUpdate = 0;

//declaring prototypes 
void drawProgress(OLEDDisplay *display, int percentage, String label);
void updateData(OLEDDisplay *display);
void drawDateTime(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
void drawCurrentWeather(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
void drawForecast(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
void drawForecastDetails(OLEDDisplay *display, int x, int y, int dayIndex);
void drawHeaderOverlay(OLEDDisplay *display, OLEDDisplayUiState* state);
void setReadyForWeatherUpdate();
void drawBME280(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
void measure_and_visible();
void logs();

// Add frames
// this array keeps function pointers to all frames
// frames are the single views that slide from right to left
FrameCallback frames[] = { drawDateTime, drawCurrentWeather, drawForecast, drawBME280 };
int numberOfFrames = 4;

OverlayCallback overlays[] = { drawHeaderOverlay };
int numberOfOverlays = 1;

char *measures[4];

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.print("Initializing SD card...");

  if (!SD.begin(15)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  root = SD.open("/");

  if (SD.exists("data.txt")) {

    Serial.println("data.txt exists.");
    Serial.println("Start logging.");
    Serial.println();
    Serial.println();
  } else {

  Serial.println("data.txt doesn't exist.");
  Serial.println("Check directory");
  printDirectory(root, 0);
  Serial.println();
  Serial.println();

  }

  // initialize dispaly
  display.init();
  //start clear display buffer and 
  display.clear();
  display.display();

  //display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setContrast(255);

  WiFi.begin(WIFI_SSID, WIFI_PWD);
// run animation for wifi connect
  byte counter = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    display.clear();
    display.drawString(64, 10, "Connecting to WiFi");
    display.drawXbm(46, 30, 8, 8, counter % 3 == 0 ? activeSymbole : inactiveSymbole);
    display.drawXbm(60, 30, 8, 8, counter % 3 == 1 ? activeSymbole : inactiveSymbole);
    display.drawXbm(74, 30, 8, 8, counter % 3 == 2 ? activeSymbole : inactiveSymbole);
    display.display();

    counter++;
  }

/***************************

 Param BME280

 **************************/
# 197 "d:\\projects\\WeatherStationV2\\WeatherStationV2.ino"
  //Set the I2C address of your breakout board  
  //Or ignore this, if you're using SPI Communication

  //0x76:       Alternative I2C Address (SDO pin connected to GND)
  //0x77:       Default I2C Address (SDO pin unconnected)

  bme280.parameter.I2CAddress = 0x77; //Choose I2C Address

  //Now choose on which mode your device will run
  //On doubt, just leave on normal mode, that's the default value

  //0b00:     In sleep mode no measurements are performed, but power consumption is at a minimum
  //0b01:     In forced mode a single measured is performed and the device returns automatically to sleep mode
  //0b11:     In normal mode the sensor measures continually (default value)

  bme280.parameter.sensorMode = 0b11; //Choose sensor mode

  //The IIR (Infinite Impulse Response) filter suppresses high frequency fluctuations
  //In short, a high factor value means less noise, but measurements are also less responsive
  //You can play with these values and check the results!
  //In doubt just leave on default

  //0b000:      factor 0 (filter off)
  //0b001:      factor 2
  //0b010:      factor 4
  //0b011:      factor 8
  //0b100:      factor 16 (default value)

  bme280.parameter.IIRfilter = 0b100; //Setup for IIR Filter

  //Next you'll define the oversampling factor for the humidity measurements
  //Again, higher values mean less noise, but slower responses
  //If you don't want to measure humidity, set the oversampling to zero

  //0b000:      factor 0 (Disable humidity measurement)
  //0b001:      factor 1
  //0b010:      factor 2
  //0b011:      factor 4
  //0b100:      factor 8
  //0b101:      factor 16 (default value)

  bme280.parameter.humidOversampling = 0b101; //Setup Humidity Oversampling

  //Now define the oversampling factor for the temperature measurements
  //You know now, higher values lead to less noise but slower measurements

  //0b000:      factor 0 (Disable temperature measurement)
  //0b001:      factor 1
  //0b010:      factor 2
  //0b011:      factor 4
  //0b100:      factor 8
  //0b101:      factor 16 (default value)

  bme280.parameter.tempOversampling = 0b101; //Setup Temperature Ovesampling

  //Finally, define the oversampling factor for the pressure measurements
  //For altitude measurements a higher factor provides more stable values
  //On doubt, just leave it on default

  //0b000:      factor 0 (Disable pressure measurement)
  //0b001:      factor 1
  //0b010:      factor 2
  //0b011:      factor 4
  //0b100:      factor 8
  //0b101:      factor 16 (default value)

  bme280.parameter.pressOversampling = 0b101; //Setup Pressure Oversampling 

  //For precise altitude measurements please put in the current pressure corrected for the sea level
  //On doubt, just leave the standard pressure as default (1013.25 hPa)

  bme280.parameter.pressureSeaLevel = 982.58; //default value of 1013.25 hPa

  //Now write here the current average temperature outside (yes, the outside temperature!)
  //You can either use the value in Celsius or in Fahrenheit, but only one of them (comment out the other value)
  //In order to calculate the altitude, this temperature is converted by the library into Kelvin
  //For slightly less precise altitude measurements, just leave the standard temperature as default (15°C)
  //Remember, leave one of the values here commented, and change the other one!
  //If both values are left commented, the default temperature of 15°C will be used
  //But if both values are left uncommented, then the value in Celsius will be used    

  bme280.parameter.tempOutsideCelsius = 15; //default value of 15°C
  //bme280.parameter.tempOutsideFahrenheit = 59;           //default value of 59°F

/***************************

 OTA Update

 **************************/
# 285 "d:\\projects\\WeatherStationV2\\WeatherStationV2.ino"
  ArduinoOTA.onStart([]() {
  String type;
  if (ArduinoOTA.getCommand() == 0) {
    type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }
      // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  /***************************

  Start OLED UI

 **************************/
# 324 "d:\\projects\\WeatherStationV2\\WeatherStationV2.ino"
  // Get time from network time service
  configTime(((3 /* (utc+) TZ in hours*/)*3600), ((0 /* use 60mn for summer time in some countries*/)*60), "pool.ntp.org");

  ui.setTargetFPS(30);

  ui.setActiveSymbol(activeSymbole);
  ui.setInactiveSymbol(inactiveSymbole);

  // You can change this to
  // TOP, LEFT, BOTTOM, RIGHT
  ui.setIndicatorPosition(BOTTOM);

  // Defines where the first frame is located in the bar.
  ui.setIndicatorDirection(LEFT_RIGHT);

  // You can change the transition that is used
  // SLIDE_LEFT, SLIDE_RIGHT, SLIDE_TOP, SLIDE_DOWN
  ui.setFrameAnimation(SLIDE_LEFT);

  ui.setFrames(frames, numberOfFrames);

  ui.setOverlays(overlays, numberOfOverlays);

  // Inital UI takes care of initalising the display too.
  ui.init();

  Serial.println("");

  updateData(&display);

}

void measure_and_visible()
{
    dtostrf(bme280.readTempC(),4,2,measures[0]);
    dtostrf(bme280.readHumidity(),4,2,measures[1]);
    dtostrf(bme280.readPressure(),6,2,measures[2]);
    dtostrf(bme280.readAltitudeMeter(),6,2,measures[3]);

   Serial.print(((reinterpret_cast<const __FlashStringHelper *>((__extension__({static const char __c[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "WeatherStationV2.ino" "." "363" "." "21" "\", \"aSM\", @progbits, 1 #"))) = ("Temperature in Celsius:\t\t"); &__c[0];}))))));
   Serial.println(measures[0]);

   Serial.print(((reinterpret_cast<const __FlashStringHelper *>((__extension__({static const char __c[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "WeatherStationV2.ino" "." "366" "." "22" "\", \"aSM\", @progbits, 1 #"))) = ("Humidity in %:\t\t\t"); &__c[0];}))))));
   Serial.println(measures[1]);

   Serial.print(((reinterpret_cast<const __FlashStringHelper *>((__extension__({static const char __c[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "WeatherStationV2.ino" "." "369" "." "23" "\", \"aSM\", @progbits, 1 #"))) = ("Pressure in hPa:\t\t"); &__c[0];}))))));
   Serial.println(measures[2]);

   Serial.print(((reinterpret_cast<const __FlashStringHelper *>((__extension__({static const char __c[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "WeatherStationV2.ino" "." "372" "." "24" "\", \"aSM\", @progbits, 1 #"))) = ("Altitude in Meters:\t\t"); &__c[0];}))))));
   Serial.println(measures[3]);

   Serial.println();
}

void printDirectory(File dir, int numTabs) {
  while (true) {
    File entry = dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println(((reinterpret_cast<const __FlashStringHelper *>((__extension__({static const char __c[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "WeatherStationV2.ino" "." "390" "." "25" "\", \"aSM\", @progbits, 1 #"))) = ("/"); &__c[0];}))))));
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), 10);
    }
    entry.close();
  }
}

void loop()
{
//start OTA
  ArduinoOTA.handle();

  if (millis() - timeSinceLastWUpdate > (1000L*UPDATE_INTERVAL_SECS))
  {
    setReadyForWeatherUpdate();
    timeSinceLastWUpdate = millis();
  }

  if (readyForWeatherUpdate && ui.getUiState()->frameState == FIXED)
  {
    updateData(&display);
  }

  int remainingTimeBudget = ui.update();

  if (remainingTimeBudget > 0)
  {
    // You can do some work here
    // Don't do stuff if you are below your
    // time budget.
//  Serial.println(millis());
    delay(remainingTimeBudget);
  }
    if (millis() - timeSinceLastWUpdate > (1000*60*10))
  {
    timeSinceLastWUpdate = millis();
    Serial.println(millis());
    logs();
  }
  //update timers for logs
}


/***************************

 * Draw progress update

 **************************/
# 441 "d:\\projects\\WeatherStationV2\\WeatherStationV2.ino"
void drawProgress(OLEDDisplay *display, int percentage, String label)
{
  display->clear();
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_10);
  display->drawString(64, 10, label);
  display->drawProgressBar(2, 28, 124, 10, percentage);
  display->display();
}

/***************************

 * Update data and weather

 **************************/
# 455 "d:\\projects\\WeatherStationV2\\WeatherStationV2.ino"
void updateData(OLEDDisplay *display)
{
  drawProgress(display, 10, "Updating time...");
  drawProgress(display, 30, "Updating weather...");
  currentWeatherClient.setMetric(IS_METRIC);
  currentWeatherClient.setLanguage(OPEN_WEATHER_MAP_LANGUAGE);
  currentWeatherClient.updateCurrentById(&currentWeather, OPEN_WEATHER_MAP_APP_ID, OPEN_WEATHER_MAP_LOCATION_ID);
  drawProgress(display, 50, "Updating forecasts...");
  forecastClient.setMetric(IS_METRIC);
  forecastClient.setLanguage(OPEN_WEATHER_MAP_LANGUAGE);
  uint8_t allowedHours[] = {12};
  forecastClient.setAllowedHours(allowedHours, sizeof(allowedHours));
  forecastClient.updateForecastsById(forecasts, OPEN_WEATHER_MAP_APP_ID, OPEN_WEATHER_MAP_LOCATION_ID, MAX_FORECASTS);

  readyForWeatherUpdate = false;
  drawProgress(display, 100, "Done...");
  delay(1000);
}

/***************************

 * draw Date && Time on widget 

 **************************/
# 478 "d:\\projects\\WeatherStationV2\\WeatherStationV2.ino"
void drawDateTime(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y)
{
  now = time(nullptr);
  struct tm* timeInfo;
  timeInfo = localtime(&now);
  char buff[16];


  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_10);
  String date = WDAY_NAMES[timeInfo->tm_wday];

  sprintf_P(buff, (__extension__({static const char __c[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "WeatherStationV2.ino" "." "490" "." "26" "\", \"aSM\", @progbits, 1 #"))) = ("%s, %02d/%02d/%04d"); &__c[0];})), WDAY_NAMES[timeInfo->tm_wday].c_str(), timeInfo->tm_mday, timeInfo->tm_mon+1, timeInfo->tm_year + 1900);
  display->drawString(64 + x, 5 + y, String(buff));
  display->setFont(ArialMT_Plain_24);

  sprintf_P(buff, (__extension__({static const char __c[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "WeatherStationV2.ino" "." "494" "." "27" "\", \"aSM\", @progbits, 1 #"))) = ("%02d:%02d:%02d"); &__c[0];})), timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec);
  display->drawString(64 + x, 15 + y, String(buff));
  display->setTextAlignment(TEXT_ALIGN_LEFT);
}

/***************************

 * draw current weather in second widget 

 **************************/
# 503 "d:\\projects\\WeatherStationV2\\WeatherStationV2.ino"
void drawCurrentWeather(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y)
{
  display->setFont(ArialMT_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->drawString(64 + x, 38 + y, currentWeather.description);

  display->setFont(ArialMT_Plain_24);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  String temp = String(currentWeather.temp, 1) + (IS_METRIC ? "°C" : "°F");
  display->drawString(60 + x, 5 + y, temp);

  display->setFont(Meteocons_Plain_36);
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->drawString(32 + x, 0 + y, currentWeather.iconMeteoCon);
}

/***************************

 * draw forecast weather in third widget 

 **************************/
# 523 "d:\\projects\\WeatherStationV2\\WeatherStationV2.ino"
void drawForecast(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y)
{
  drawForecastDetails(display, x, y, 0);
  drawForecastDetails(display, x + 44, y, 1);
  drawForecastDetails(display, x + 88, y, 2);
}

/***************************

 *  system display settings function for third widget

 **************************/
# 534 "d:\\projects\\WeatherStationV2\\WeatherStationV2.ino"
void drawForecastDetails(OLEDDisplay *display, int x, int y, int dayIndex)
{
  time_t observationTimestamp = forecasts[dayIndex].observationTime;
  struct tm* timeInfo;
  timeInfo = localtime(&observationTimestamp);
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_10);
  display->drawString(x + 20, y, WDAY_NAMES[timeInfo->tm_wday]);

  display->setFont(Meteocons_Plain_21);
  display->drawString(x + 20, y + 12, forecasts[dayIndex].iconMeteoCon);
  String temp = String(forecasts[dayIndex].temp, 0) + (IS_METRIC ? "°C" : "°F");
  display->setFont(ArialMT_Plain_10);
  display->drawString(x + 20, y + 34, temp);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
}

/***************************

 * draw line and time&&weather under line

 **************************/
# 555 "d:\\projects\\WeatherStationV2\\WeatherStationV2.ino"
void drawHeaderOverlay(OLEDDisplay *display, OLEDDisplayUiState* state)
 {
  now = time(nullptr);
  struct tm* timeInfo;
  timeInfo = localtime(&now);
  char buff[14];
  sprintf_P(buff, (__extension__({static const char __c[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "WeatherStationV2.ino" "." "561" "." "28" "\", \"aSM\", @progbits, 1 #"))) = ("%02d:%02d"); &__c[0];})), timeInfo->tm_hour, timeInfo->tm_min);

  display->setColor(WHITE);
  display->setFont(ArialMT_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->drawString(0, 54, String(buff));
  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  String temp = String(currentWeather.temp, 1) + (IS_METRIC ? "°C" : "°F");
  display->drawString(128, 54, temp);
  display->drawHorizontalLine(0, 52, 128);
}

/***************************

 *  system display settings function for fourth widget

 **************************/
# 577 "d:\\projects\\WeatherStationV2\\WeatherStationV2.ino"
void drawBME280(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y)
{

  now = time(nullptr);
  struct tm* timeInfo;
  timeInfo = localtime(&now);
  char buff[14];
  sprintf_P(buff, (__extension__({static const char __c[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "WeatherStationV2.ino" "." "584" "." "29" "\", \"aSM\", @progbits, 1 #"))) = ("%02d:%02d"); &__c[0];})), timeInfo->tm_hour, timeInfo->tm_min);

  display->setColor(WHITE);
  display->setFont(ArialMT_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->drawString(0, 54, String(buff));
  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  String temp = String(currentWeather.temp, 1) + (IS_METRIC ? "°C" : "°F");
  display->drawString(128, 54, temp);
  display->drawHorizontalLine(0, 52, 128);
  display->drawString(128, 0, "string");

}

/***************************

 * flag for update weather

 **************************/
# 602 "d:\\projects\\WeatherStationV2\\WeatherStationV2.ino"
void setReadyForWeatherUpdate()
{
  Serial.println("Setting readyForUpdate to true");
  readyForWeatherUpdate = true;
}

/***************************

  start logs

 **************************/
# 612 "d:\\projects\\WeatherStationV2\\WeatherStationV2.ino"
void logs()
{
  Serial.println("Logg status");
  now = time(nullptr);
  struct tm* timeInfo;
  timeInfo = localtime(&now);
  char buff[50];
  sprintf_P(buff, (__extension__({static const char __c[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "WeatherStationV2.ino" "." "619" "." "30" "\", \"aSM\", @progbits, 1 #"))) = ("%s, %02d/%02d/%04d"); &__c[0];})), WDAY_NAMES[timeInfo->tm_wday].c_str(), timeInfo->tm_mday, timeInfo->tm_mon+1, timeInfo->tm_year + 1900,measures[0],measures[1],measures[2],measures[3]);

  root = SD.open("data.txt",(sdfat::O_RDONLY | sdfat::O_WRONLY | sdfat::O_CREAT | sdfat::O_APPEND));
  root.println("string tesst");
  root.write(buff,45);
  root.close();
//print measures and time/
  }
