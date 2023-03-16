/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

// Import required libraries
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>

// Replace with your network credentials
const char* ssid = "KUWIN.";
const char* password = "watermelon";

#define DHTPIN 4     // Digital pin connected to the DHT sensor
#define led 14
#define relay 26

// Uncomment the type of sensor in use:
#define DHTTYPE    DHT11     // DHT 11
//#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

String readDHTHumidity() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  if (isnan(h)) {
    Serial.println("Failed to read from DHT sensor!");
    return "--";
  }
  else {
    Serial.println(h);
    return String(h);
  }
}

String check() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  if(isnan(h)){
    digitalWrite(relay, LOW);
    digitalWrite(led, LOW);
    Serial.println("OFF");
    return "OFF";
    }
  else if (h <= 30) {
    digitalWrite(relay, HIGH);
    digitalWrite(led, HIGH);
    Serial.println("ON");
    return "ON";
  }
  else if (h >= 75){
    digitalWrite(relay, LOW);
    digitalWrite(led, LOW);
    Serial.println("OFF");
    return "OFF";
  }
}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 2.0rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
    .label_index{
      font-size: 2.0rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body bgcolor = "#AEC2B6">
  <h2>ESP32 DHT Server</h2>
  <p>
    <i class="fas fa-tint" style="color:#00add6;"></i> 
    <span class="dht-labels">Humidity</span>
    <span id="humidity" class="label_index">%HUMIDITY%</span>
    <sup class="units">&percnt;</sup>
  </p>
  <p>
    <span class="dht-labels">Status : </span>
    <span id="check" class="label_index">%CHECK%</span> 
  </p>
</body>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("humidity").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/humidity", true);
  xhttp.send();
}, 10000 ) ;
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("check").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/check", true);
  xhttp.send();
}, 10000 ) ;
</script>
</html>)rawliteral";

// Replaces placeholder with DHT values
String processor(const String& var){
  //Serial.println(var);
  if(var == "HUMIDITY"){
    return readDHTHumidity();
  }
  return String();
}

String processor_check(const String& var){
  //Serial.println(var);
  if(var == "CHECK"){
    return check();
  }
  return String();
}

void setup(){
  pinMode(relay, OUTPUT);
  pinMode(led, OUTPUT);

  digitalWrite(relay, LOW);
  // Serial port for debugging purposes
  Serial.begin(115200);

  dht.begin();
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
  server.on("/check", HTTP_GET, [](AsyncWebServerRequest *request){
  request->send_P(200, "text/plain", check().c_str());
  });
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
  request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
  request->send_P(200, "text/plain", readDHTHumidity().c_str());
  });

  
  

  // Start server
  server.begin();
}
 
void loop(){
  
}
