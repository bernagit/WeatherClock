#include <header.h>

void initDisplay()
{
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("SSD1306 allocation failed"));
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
}

void displayTempHumidity(float temp, float humidity)
{
  // clear display
  display.clearDisplay();
  // display temperature
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Temperatura: ");
  display.setTextSize(2, 2);
  display.setCursor(0, 10);
  display.printf("%.1f", temp);
  display.print(" ");
  // simbolo dei gradi centigradi
  display.setTextSize(1);
  display.cp437(true);
  display.write(167);
  display.setTextSize(2, 2);
  display.print("C");
  // display humidity
  display.setTextSize(1);
  display.setCursor(0, 35);
  display.print("Umidità: ");
  display.setTextSize(2);
  display.setCursor(0, 45);
  display.printf("%.1f", humidity);
  display.print(" %");

  display.drawBitmap(85, 12, thermometer, 16, 38, 1);

  display.display();
}

void displayClock(String time)
{
  display.clearDisplay();
  // display clock
  display.setTextSize(4, 5);
  display.setCursor(5, 14);
  time = time.substring(0, 5);
  display.print(time);
  display.display();
}

void displayOutWeather(WeatherData weatherData)
{
  // clear display
  display.clearDisplay();
  // display temperature
  display.setTextSize(1, 2);
  display.setCursor(8, 1);
  display.print("Meteo\n ");
  display.print(city);

  // display humidity
  display.setTextSize(1);
  display.setCursor(0, 35);
  display.print(" Gradi   |  Umidita'");
  display.setTextSize(2);
  if (weatherData.temp > 10)
  {
    display.setCursor(2, 47);
  }
  else
  {
    display.setCursor(7, 47);
  }
  display.printf("%.1f", weatherData.temp);
  display.setCursor(72, 47);
  display.printf("%.1f", weatherData.humidity);

  switch (weatherData.icon.toInt())
  {
  case 1:
    display.drawBitmap(xIcon, yIcon, sun, 40, 40, 1);
    break;
  case 2:
    display.drawBitmap(xIcon, yIcon, cloud, 40, 34, 1);
    break;
  case 3:
    display.drawBitmap(xIcon, yIcon, cloud, 40, 34, 1);
    break;
  case 4:
    display.drawBitmap(xIcon, yIcon, cloud, 40, 34, 1);
    break;
  case 9:
    display.drawBitmap(xIcon, yIcon, rain, 40, 35, 1);
    break;
  case 10:
    display.drawBitmap(xIcon, yIcon, rain, 40, 35, 1);
    break;
  case 11:
    display.drawBitmap(xIcon, yIcon, storm, 40, 40, 1);
    break;
  case 13:
    display.drawBitmap(xIcon, yIcon, snow, 30, 32, 1);
    break;
  }
  display.display();
}

void startupDisplay()
{
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(2, 8);
  display.print("Weather\n clock");
  display.setCursor(4, 45);
  display.setTextSize(1);
  display.print("by BerniTech");
  display.drawBitmap(xIcon, 15, sun, 40, 40, 1);
  display.display();
}

void displayWifiConfig(WiFiManager *myWiFiManager)
{
  display.clearDisplay();
  display.setTextSize(1, 2);
  display.setCursor(5, 10);
  display.print("Config settings at:\n");
  display.setTextSize(1, 2);
  display.setCursor(30, 38);
  display.print(WiFi.softAPIP());
  display.display();
}

void saveConfigCallback()
{
  saveConfig = true;
}

void initWiFi()
{
  /*
      Serial.println("mounting FS...");

      if (SPIFFS.begin())
      {
        Serial.println("mounted file system");
        if (SPIFFS.exists("/config.json"))
        {
          // file exists, reading and loading
          Serial.println("reading config file");
          File configFile = SPIFFS.open("/config.json", "r");
          if (configFile)
          {
            Serial.println("opened config file");
            size_t size = configFile.size();
            // Allocate a buffer to store contents of the file.
            std::unique_ptr<char[]> buf(new char[size]);

            configFile.readBytes(buf.get(), size);
            DynamicJsonDocument json(1024);
            auto deserializeError = deserializeJson(json, buf.get());
            serializeJson(json, Serial);
            if (!deserializeError)
            {
              Serial.println("\nparsed json");
              strcpy(api_token, json["api_token"]);
              strcpy(cityName, json["mqtt_port"]);
            }
            else
            {
              Serial.println("failed to load json config");
            }
            configFile.close();
          }
        }
      }
      else
      {
        Serial.println("failed to mount FS");
      }

      WiFiManagerParameter custom_api_token("apikey", "API token", api_token, 40);
      WiFiManagerParameter custom_cityName("cityName", "CityName", cityName, 20);

      wifiManager.addParameter(&custom_api_token);
      wifiManager.addParameter(&custom_cityName);
       */
  wifiManager.setAPCallback(displayWifiConfig);
  wifiManager.autoConnect(ssid, password);

  wifiManager.setSaveConfigCallback(saveConfigCallback);
}

void printWifiStatus()
{
}

String httpGETRequest(const char *serverName)
{
  WiFiClient client;
  HTTPClient http;

  // Your Domain name with URL path or IP address with path
  http.begin(client, serverName);

  // Send HTTP POST request
  int httpResponseCode = http.GET();

  String payload = "{}";

  if (httpResponseCode > 0)
  {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else
  {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}

WeatherData getWeatherData()
{
  WeatherData weatherData;
  Serial.println("serverPath: " + serverPath);
  jsonBuffer = httpGETRequest(serverPath.c_str());
  Serial.println(jsonBuffer);
  deserializeJson(doc, jsonBuffer);
  JsonObject we = doc["weather"][0];
  String icon = we["icon"];
  weatherData.icon = icon.substring(0, 2);
  weatherData.dayTime = icon.charAt(2);
  Serial.println("icon: " + weatherData.icon);
  Serial.println("dayTime: " + weatherData.dayTime);
  JsonObject main = doc["main"];
  float temp = main["temp"];
  weatherData.temp = temp - 273.15;
  weatherData.humidity = main["humidity"];
  return weatherData;
}

void getWeatherUpdate()
{
  currentTime = millis();
  if (currentTime - startTime > timerDelayHttpReq && page != 4)
  {
    weatherData = getWeatherData();
    startTime = currentTime;
  }
}

void setup()
{
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  initDisplay();
  startupDisplay();
  delay(startPageDelay);
  initWiFi();
  printWifiStatus();
  dht.begin();
  timeClient.begin();
  weatherData = getWeatherData();
  currentTime = millis();
  startTime = currentTime;
}

void loop()
{
  getWeatherUpdate();

  timeClient.update();
  String formattedTime = timeClient.getFormattedTime();

  // read temperature and humidity
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  currentState = digitalRead(BUTTON_PIN);

  if (lastState == HIGH && currentState == LOW)
  { // button is pressed
    pressedTime = millis();
    isPressing = true;
    isLongDetected = false;
  }
  else if (lastState == LOW && currentState == HIGH)
  { // button is released
    isPressing = false;
    releasedTime = millis();

    long pressDuration = releasedTime - pressedTime;

    if (pressDuration < SHORT_PRESS_TIME)
    {
      page = (page + 1) % 3;
    }
  }

  if (isPressing == true && isLongDetected == false)
  {
    long pressDuration = millis() - pressedTime;

    if (pressDuration > LONG_PRESS_TIME)
    {
      wifiManager.resetSettings();
      wifiManager.startConfigPortal(ssid, password);
      page = 4;
      displayWifiConfig(&wifiManager);
      isLongDetected = true;
    }
  }
  if (WiFi.status() == WL_CONNECTED && page == 4)
  {
    page = 0;
  }
  // save the the last state
  lastState = currentState;

  switch (page)
  {
  case 0:
    displayTempHumidity(t, h);
    break;
  case 1:
    displayClock(formattedTime);
    break;
  case 2:
    displayOutWeather(weatherData);
    break;
  }
}