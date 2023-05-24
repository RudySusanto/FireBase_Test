#include "firebase.h"

// change this with your google firebase realtime database
#define FIREBASE_HOST "https://iotlb40-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "VXGBcK0M8m1vEiYZtBMrnVg1V5VOxU3ZexJIwGqv"

FirebaseData fbdo;
FirebaseConfig fbConfig;
FirebaseData fbdoStream;

void Firebase_Init(const String& streamPath)
{
  FirebaseAuth fbAuth;
  fbConfig.host = FIREBASE_HOST;
  fbConfig.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&fbConfig, &fbAuth);
  Firebase.reconnectWiFi(true);

#if defined(ESP8266)
  //Set the size of WiFi rx/tx buffers in the case where we want to work with large data.
  fbdo.setBSSLBufferSize(2*1024, 1024);
#endif

  //Set the size of HTTP response buffers in the case where we want to work with large data.
  fbdo.setResponseSize(1024);

  //Set database read timeout to 1 minute (max 15 minutes)
  // Firebase.RTDB.setReadTimeout(&fbdo, 1000 * 60);
  //tiny, small, medium, large and unlimited.
  //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  Firebase.RTDB.setwriteSizeLimit(&fbdo, "small");

  //optional, set the decimal places for float and double data to be stored in database
  // Firebase.setFloatDigits(2);
  // Firebase.setDoubleDigits(6);
  while (!Firebase.ready())
  {
    Serial.println("Connecting to firebase...");
    delay(1000);
  }
  String path = streamPath;
  if (Firebase.RTDB.beginStream(&fbdoStream, path.c_str()))
  {
    Serial.println("Firebase stream on "+ path);
    Firebase.RTDB.setStreamCallback(&fbdoStream, onFirebaseStream, onFirebaseStreamTimeout);
  }
  else
    Serial.println("Firebase stream failed: "+fbdoStream.errorReason());
}

void onFirebaseStreamTimeout(bool timeout)
{
  // Serial.printf("Firebase stream timeout: %d\n", timeout);
}
