#include "WiFi.h" // ESP32 WiFi include
#include <ThingsBoard.h>
#define WIFI_AP        "littleeight"
// WiFi password
#define WIFI_PASSWORD       "LittleEightIsLily"

#define TOKEN               "xmF4RAORNyI1SLxyrMHU"
// ThingsBoard server instance.
#define THINGSBOARD_SERVER  "192.168.86.189:1883"

char thingsboardServer[] = "192.168.86.189";

// Initialize ThingsBoard client
WiFiClient espClient;
// Initialize ThingsBoard instance
ThingsBoard tb(espClient);


int status = WL_IDLE_STATUS;
unsigned long lastSend;

void setup() {
  // initialize serial for debugging
  Serial.begin(9600);
 
  InitWiFi();
  reconnect();
}

void loop() {
  status = WiFi.status();
  if ( status != WL_CONNECTED) {
    while ( status != WL_CONNECTED) {
      Serial.print("Attempting to connect to WPA SSID: ");
      Serial.println(WIFI_AP);
      // Connect to WPA/WPA2 network
      status = WiFi.begin(WIFI_AP, WIFI_PASSWORD);
      delay(500);
    }
    Serial.println("Connected to AP");
  }

  if ( !tb.connected() ) {
    reconnect();
  }

  if ( millis() - lastSend > 1000 ) { // Update and send only after 1 seconds
    getAndSendTemperatureAndHumidityData();
    lastSend = millis();
  }

  tb.loop();
}

void getAndSendTemperatureAndHumidityData()
{
  Serial.println("Collecting temperature data.");

  // Reading temperature or humidity takes about 250 milliseconds!
  float humidity = 34;
  // Read temperature as Celsius (the default)
  float temperature = 45;

  Serial.println("Sending data to ThingsBoard:");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" *C ");

  bool temp_sent = tb.sendTelemetryFloat("temperature", temperature);
  bool humi_sent = tb.sendTelemetryFloat("humidity", humidity);
  if (!temp_sent) {
    Serial.println("temperature was not sent");
  }
  if (!humi_sent) {
    Serial.println("humidity was not sent");
  }
}

void InitWiFi()
{
  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network

  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to AP");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!tb.connected()) {
    Serial.print("Connecting to ThingsBoard node ...");
    // Attempt to connect (clientId, username, password)
    if ( tb.connect(thingsboardServer, TOKEN) ) {
      Serial.println( "[DONE]" );
    } else {
      Serial.print( "[FAILED]" );
      Serial.println( " : retrying in 5 seconds" );
      // Wait 5 seconds before retrying
      delay( 5000 );
    }
  }
}
