#define BLYNK_TEMPLATE_ID "TMPL3viiVvggp"
#define BLYNK_TEMPLATE_NAME "ESP32"
#define BLYNK_AUTH_TOKEN "k120dIPgMXPwmeRPD23WDn59Ql-rmUG9"
#define BLYNK_PRINT Serial

#include <DHT.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <HTTPClient.h>

#define vibPin 22
#define dhtPin 23
#define pulsePin 34 // Pin for the pulse sensor

const char* ssid="Bhavani";
const char* password="123456789";

const char* aiserver="http://192.168.174.162:2000/predict?v=";


unsigned long lastTime = 0; // Last time BPM was calculated
int lastHeartBeatCount = 0; // Last heartbeat count for BPM calculation

DHT dht(dhtPin, DHT11);
String result;

void setup() {
  projectInit();
}

void loop() {
  int v = getVibration();
  float h = getHumidity();
  float t = getTemperature();
  int heartRate = getHeartRate();

  if(v==0 || (heartRate<50 && heartRate>30)){
    result="abnormal";
  } else {
    result="normal";
  }

  Blynk.virtualWrite(V0,h);
  Blynk.virtualWrite(V1,t);
  Blynk.virtualWrite(V2,v);
  Blynk.virtualWrite(V3,heartRate);

  String resultFromAI=getDataFromAI(h,t);
  if(resultFromAI=="abnormal"){
    Blynk.logEvent("snoring_alert","Anamoly Detected");
  } 

  Serial.print("#");
  Serial.print(",");
  Serial.print(v);
  Serial.print(",");
  Serial.print(h);
  Serial.print(",");
  Serial.print(t);
  Serial.print(",");
  Serial.print(heartRate);
  Serial.print(",");
  Serial.print(result);
  Serial.print(",");
  Serial.println("~");
  
  delay(1000);
}

void projectInit() {
  pinMode(vibPin, INPUT);
  pinMode(pulsePin, INPUT);
  Serial.begin(9600);
  dht.begin();

  WiFi.mode(WIFI_STA);
  delay(1000);
  WiFi.begin(ssid,password);
  while(WiFi.status()!=WL_CONNECTED){
    Serial.print(".");
    delay(1000);
  }
  Serial.println("WIFI Connected");
  Blynk.begin(BLYNK_AUTH_TOKEN,ssid,password);
}

int getVibration() {
  return digitalRead(vibPin);
}

float getHumidity() {
  float h = dht.readHumidity();
  if (isnan(h)) {
    Serial.println("Humidity Error");
    return -1; // Return -1 for error
  } else {
    return h;
  }
}

float getTemperature() {
  float t = dht.readTemperature();
  if (isnan(t)) {
    Serial.println("Temperature Error");
    return -1; // Return -1 for error
  } else {
    return t;
  }
}


int getHeartRate() {
  // Read the pulse sensor value
  int sensorValue = analogRead(pulsePin);
  // Implement a simple threshold to detect heartbeats
  static int lastValue = 0;
  static int heartBeatCount = 0;

  // Detect heartbeats
  if (sensorValue) { // Adjust threshold as necessary
    heartBeatCount+=sensorValue;
  }
  
  lastValue = sensorValue;

  // Calculate BPM every second
  unsigned long currentTime = millis();
  if (currentTime - lastTime >= 1000) {
    lastTime = currentTime;
    int bpm = (heartBeatCount/60); // Calculate BPM
    heartBeatCount=0; // Update the last count
    return bpm; // Return BPM
  }
  
  return -1; // Return -1 if BPM hasn't been calculated yet
}

String getDataFromAI(int v1,int hr1){
  if(WiFi.status()==WL_CONNECTED){
    HTTPClient http;

    http.begin(aiserver+String(v1)+"&hr="+String(hr1));
    int code=http.GET();
      if (code>0){
        String response=http.getString();
        Serial.println(response);
        http.end();
        return(response);
    }
  }
}
