#include "FirebaseUtils.h"
#include "addons/TokenHelper.h" //Provide the token generation process info.
#include "addons/RTDBHelper.h"  //Provide the RTDB payload printing info and other helper functions.

FirebaseAuth FirebaseUtils::auth;                      // The user UID can be obtained from auth.token.uid
FirebaseConfig FirebaseUtils::config;                  // Firebase Config data
FirebaseData FirebaseUtils::configReceiveFirebaseData; // FirebaseData received from config changes

void FirebaseUtils::setup()
{
  Serial.printf("Initializing Firebase v%s\n\n", FIREBASE_CLIENT_VERSION);
  // Firebase Credentials
  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;
  Firebase.reconnectWiFi(true);                       // Reconnect when possible
  config.token_status_callback = tokenStatusCallback; // Token generation for authentication
  config.max_token_generation_retry = 5;              // Maximum retries for token generation
  Firebase.begin(&config, &auth);                     // Initialize Firebase authentication and config
  String configPath = String("/trolleys/") + String(GlobalVariables::trolleyName) + String("/config");
  if (!Firebase.beginStream(configReceiveFirebaseData, configPath))
  {
    Serial.println("Could not begin config stream\nREASON: " + configReceiveFirebaseData.errorReason());
  }
  Firebase.setStreamCallback(configReceiveFirebaseData, configStreamCallback, configStreamTimeoutCallback);
}

// void uploadLocationtoFirebase(char* location, unsigned long custom_timestamp)
// {
//   // Upload log to Firebase
//   Serial.println("------------------------------------");
//   FirebaseJson log_json; // JSON object to send to Firebase
//   log_json.add("temperature", temperatureValue);
//   if (Firebase.pushJSON(loggingFirebaseData, "/essential_data", log_json))
//   {
//     // Successfully pushed
//     if (custom_timestamp == 0)
//     {
//       // Use Firebase's timestamp
//       if (Firebase.setTimestamp(loggingFirebaseData, "/essential_data/" + loggingFirebaseData.pushName() + "/dateCreated"))
//       {
//         // Successfully set Firebase timestamp
//         Serial.println("Added log to Firebase");
//       }
//       else
//       {
//         Serial.println("Failed to set Firebase timestamp: " + loggingFirebaseData.errorReason());
//       }
//     }
//     else
//     {
//       // Use custom timestamp
//       if (Firebase.setInt(loggingFirebaseData, "/essential_data/" + loggingFirebaseData.pushName() + "/dateCreated", custom_timestamp))
//       {
//         // Successfully set custom timestamp
//         Serial.println("Added CUSTOM log to Firebase");
//       }
//       else
//       {
//         Serial.println("Failed to set CUSTOM timestamp in Firebase: " + loggingFirebaseData.errorReason());
//       }
//     }
//   }
//   else
//   {
//     Serial.println("Failed to log to Firebase: " + loggingFirebaseData.errorReason());
//   }
//   Serial.println("------------------------------------");
// }

void FirebaseUtils::configStreamCallback(StreamData data)
{
  // Triggered when a config change occurs in Firebase
  if (data.dataType() == "json")
  {
    Serial.println("Config change occurred");
    FirebaseJson *config_json = data.to<FirebaseJson *>(); // Config data received from Firebase
    FirebaseJsonData trolleyEnabledResult;
    config_json->get(trolleyEnabledResult, "enabled");
    if (trolleyEnabledResult.success)
    {
      GlobalVariables::trolleyEnabled = trolleyEnabledResult.to<bool>();
    }
  }
}

void FirebaseUtils::configStreamTimeoutCallback(bool timeout)
{
  // Triggered if config stream is unresponsive or loses connection
  if (timeout)
  {
    Serial.println("\nConfig stream timeout, resume streaming...\n");
  }
  if (!configReceiveFirebaseData.httpConnected())
  {
    Serial.printf("Config stream error code: %d, reason: %s\n\n", configReceiveFirebaseData.httpCode(), configReceiveFirebaseData.errorReason().c_str());
  }
}