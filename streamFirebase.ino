#include <Arduino.h>
#include <base64.h>
#define BTN_PIN 12
boolean streamFlag = false;
const char *ssid = "RIGJAZZ";
const char *password = "Kashmiri786";

String FIREBASE_HOST = "https://livestreamesp32-default-rtdb.firebaseio.com/";
String FIREBASE_AUTH = "AIzaSyBh8lfFrhhw8hzBSZ6lRUKMOcH46pLBeH8";
#include "FirebaseESP32.h"
FirebaseData firebaseData;
#include <WiFi.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "esp_camera.h"
int counter = 0;

#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27
#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22



String urlencode(String str);
// WARNING!!! Make sure that you have either selected ESP32 Wrover Module,

//            or another board which has PSRAM enabled

// CAMERA_MODEL_AI_THINKER





/**
 * Created by K. Suwatchai (Mobizt)
 *
 * Email: k_suwatchai@hotmail.com
 *
 * Github: https://github.com/mobizt/Firebase-ESP-Client
 *
 * Copyright (c) 2022 mobizt
 *
 */

/** This example shows the basic RTDB usage with external Client.
 * This example used SAMD21 device and WiFiNINA as the client.
 */


#include <Firebase_ESP_Client.h>

// Provide the token generation process info.
#include <addons/TokenHelper.h>

// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>


// For the following credentials, see examples/Authentications/SignInAsUser/EmailPassword/EmailPassword.ino

/* 2. Define the API Key */
#define API_KEY "AIzaSyBh8lfFrhhw8hzBSZ6lRUKMOcH46pLBeH8"

/* 3. Define the RTDB URL */
#define DATABASE_URL "https://livestreamesp32-default-rtdb.firebaseio.com/"  //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "goparajukarthik2@gmail.com"
#define USER_PASSWORD "qwerty@123"

FirebaseAuth auth;
FirebaseConfig config;






////////////

String Photo2Base64() {

  camera_fb_t *fb = NULL;

  fb = esp_camera_fb_get();

  if (!fb) {

    Serial.println("Camera capture failed");

    return "";
  }

  String imageFile = "data:image/jpeg;base64,";
  String encrypt = base64::encode(fb->buf, fb->len);



  esp_camera_fb_return(fb);

  //  return imageFile;
  //Serial.println(encrypt);
  return encrypt;
}

// https://github.com/zenmanenergy/ESP8266-Arduino-Examples/

String urlencode(String str){
  String encodedString = "";
  char c,code0,code1,code2;

  for (int i = 0; i < str.length(); i++) {
    c = str.charAt(i);
    if (c == ' ') {
      encodedString += '+';
    } else if (isalnum(c)) {
      encodedString += c;
    } else {
      code1 = (c & 0xf) + '0';
      if ((c & 0xf) > 9) {
        code1 = (c & 0xf) - 10 + 'A';
      }
      c = (c >> 4) & 0xf;
      code0 = c + '0';
      if (c > 9) {
        code0 = c - 10 + 'A';
      }
      code2 = '\0';
      encodedString += '%';
      encodedString += code0;
      encodedString += code1;
      encodedString += code2;
    }
    yield();
  }
  return encodedString;
}

void setupWiFi() {

  WiFi.begin(ssid, password);
  long int StartTime = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if ((StartTime + 10000) < millis())
      break;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.print("Camera Ready! Use 'http://");
    Serial.print(WiFi.localIP());
    Serial.println("' to connect");
    // WiFi.softAP((WiFi.localIP().toString()+"_"+(String)apssid).c_str(), appassword);
  }
  else {
    Serial.println("Connection failed");
    return;
  }
}
void setupCamera() {
  camera_config_t config;



  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  // init with high specs to pre-allocate larger buffers
  if (psramFound()) {
    config.frame_size = FRAMESIZE_VGA;
    config.jpeg_quality = 12;  // 0-63 lower number means higher quality
    config.fb_count = 1;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;  // 0-63 lower number means higher quality
    config.fb_count = 1;
  }
  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    delay(1000);
    ESP.restart();
  }
  sensor_t *s = esp_camera_sensor_get();
  s->set_framesize(s, FRAMESIZE_QQVGA);  // VGA|CIF|QVGA|HQVGA|QQVGA   ( UXGA? SXGA? XGA? SVGA? )
}



void startStreaming() {
  String jsonData = "{\"photo\":\"" + Photo2Base64() + "\"}";
  String photoPath = "/esp32-cam";
  FirebaseJson json2;
  json2.set("photo", Photo2Base64());
  json2.set("Counter", (String)counter);
  counter++;
  if (Firebase.setJSON(firebaseData, photoPath, json2)) {
    Serial.println(firebaseData.dataPath());
    Serial.println(firebaseData.pushName());
    Serial.println(firebaseData.dataPath() + "/" + firebaseData.pushName());
    Serial.println("Image Uploaded");
  } else {
    Serial.println(firebaseData.errorReason());
  }
}



void setup() {

  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  Serial.begin(115200);
  Serial.setDebugOutput(true);
  setupCamera();;
  setupWiFi();
  /* Assign the api key (required) */
  config.api_key = API_KEY;
  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;
  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback;  // see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  Firebase.setMaxRetry(firebaseData, 3);
  Firebase.setMaxErrorQueue(firebaseData, 30);
  Firebase.enableClassicRequest(firebaseData, true);
}

void loop() {

  startStreaming();
  delay(1000);
}
