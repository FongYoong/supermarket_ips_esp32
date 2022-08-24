#include "FirebaseUtils.h"
#include "addons/TokenHelper.h" //Provide the token generation process info.
#include "addons/RTDBHelper.h"  //Provide the RTDB payload printing info and other helper functions.


String FirebaseUtils::configPath = String("/trolley_config/") + String(GlobalVariables::trolleyId);
String FirebaseUtils::logPath = String("/trolley_data/") + String(GlobalVariables::trolleyId);
String FirebaseUtils::pastLogsPath = String("/trolley_past_data/") + String(GlobalVariables::trolleyId);
FirebaseAuth FirebaseUtils::auth;                      // The user UID can be obtained from auth.token.uid
FirebaseConfig FirebaseUtils::config;                  // Firebase Config data
FirebaseData FirebaseUtils::configReceiveFirebaseData; // FirebaseData received from config changes
FirebaseData FirebaseUtils::loggingFirebaseData; // FirebaseData received from uploading logs

Utilities::RepeatingTask FirebaseUtils::getConfigTask(GET_CONFIG_INTERVAL);
Utilities::RepeatingTask FirebaseUtils::uploadLogTask(UPLOAD_LOG_INTERVAL);

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

  // The code below is to automatically detect config changes but it is unused due to high memory usage.
  // if (!Firebase.beginStream(configReceiveFirebaseData, configPath))
  // {
  //   Serial.println("Could not begin config stream\nREASON: " + configReceiveFirebaseData.errorReason());
  // }
  // Firebase.setStreamCallback(configReceiveFirebaseData, configStreamCallback, configStreamTimeoutCallback);
}

void FirebaseUtils::run()
{
  if (GlobalVariables::trolleyEnabled && uploadLogTask.isReady())
  {
    uploadLogToFirebase(GlobalVariables::coordinates);
  }
  if (getConfigTask.isReady())
  {
    getConfigFromFirebase();
  }
}

void FirebaseUtils::getConfigFromFirebase()
{
  Serial.println("------------------------------------");
  Serial.println("Retrieving trolley config...");
  if (Firebase.ready() && Firebase.getJSON(configReceiveFirebaseData, configPath)) {
    if (configReceiveFirebaseData.dataType() == "json")
    {
      FirebaseJson *config_json = configReceiveFirebaseData.to<FirebaseJson *>(); // Config data received from Firebase
      FirebaseJsonData trolleyEnabledResult;
      config_json->get(trolleyEnabledResult, "enabled");
      if (trolleyEnabledResult.success)
      {
        GlobalVariables::trolleyEnabled = trolleyEnabledResult.to<bool>();
        Serial.println("trolleyEnabled:" + String(GlobalVariables::trolleyEnabled));
      }
    }
  }
  Serial.println("------------------------------------");
}

void FirebaseUtils::uploadLogToFirebase(String coordinates)
{
  Serial.println("------------------------------------");
  Serial.println("Uploading log...");
  FirebaseJson log_json;
  log_json.add("coordinates", coordinates);
  if (Firebase.ready() && Firebase.pushJSON(loggingFirebaseData, pastLogsPath, log_json))
  {
    Serial.println("Uploaded and pushed latest log to past logs");
    if (Firebase.setTimestamp(loggingFirebaseData, pastLogsPath + "/" + loggingFirebaseData.pushName() + "/dateCreated"))
    {
      Serial.println("Uploaded and pushed to past logs");
    }
    else
    {
      Serial.println("Failed to set Firebase timestamp: " + loggingFirebaseData.errorReason());
    }

    // Update log
    log_json.add("name", GlobalVariables::trolleyName);
    if (Firebase.ready() && Firebase.setJSON(loggingFirebaseData, logPath, log_json)) {
      if (Firebase.setTimestamp(loggingFirebaseData, logPath + "/dateCreated"))
      {
        Serial.println("Uploaded and set latest log");
      }
      else
      {
        Serial.println("Failed to set Firebase timestamp: " + loggingFirebaseData.errorReason());
      }
    }
    else
    {
      Serial.println("Failed to update log in Firebase: " + loggingFirebaseData.errorReason());
    }
  }
  else
  {
    Serial.println("Failed to push log to Firebase: " + loggingFirebaseData.errorReason());
  }
  Serial.println("------------------------------------");
}

// void FirebaseUtils::configStreamCallback(StreamData data)
// {
//   // Triggered when a config change occurs in Firebase
//   Serial.println("Config change occurred");
//   if (data.dataType() == "json")
//   {
//     FirebaseJson *config_json = data.to<FirebaseJson *>(); // Config data received from Firebase
//     FirebaseJsonData trolleyEnabledResult;
//     config_json->get(trolleyEnabledResult, "enabled");
//     if (trolleyEnabledResult.success)
//     {
//       GlobalVariables::trolleyEnabled = trolleyEnabledResult.to<bool>();
//       Serial.println("trolleyEnabled:" + String(GlobalVariables::trolleyEnabled));
//     }
//   }
// }

// void FirebaseUtils::configStreamTimeoutCallback(bool timeout)
// {
//   // Triggered if config stream is unresponsive or loses connection
//   if (timeout)
//   {
//     Serial.println("\nConfig stream timeout, resume streaming...\n");
//   }
//   if (!configReceiveFirebaseData.httpConnected())
//   {
//     Serial.printf("Config stream error code: %d, reason: %s\n\n", configReceiveFirebaseData.httpCode(), configReceiveFirebaseData.errorReason().c_str());
//   }
// }