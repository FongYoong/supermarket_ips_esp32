#ifndef FIREBASE_UTILS_H
#define FIREBASE_UTILS_H

#include "Arduino.h"
#include <FirebaseESP32.h>
#include "../GlobalVariables/GlobalVariables.h"

// Firebase Login Credentials
#define USER_EMAIL "trolley@trolley.com"
#define USER_PASSWORD "trolley"
#define API_KEY "AIzaSyD_x7HHCmyzOC3KEzUEIGNFxry8YteM4Mw"
#define DATABASE_URL "https://idp-vlc-default-rtdb.asia-southeast1.firebasedatabase.app"

#define UPDATE_FIREBASE_CONFIG_INTERVAL 2000 // Milliseconds // Interval between updating pending config changes

class FirebaseUtils
{
public:
    static FirebaseAuth auth;     // The user UID can be obtained from auth.token.uid
    static FirebaseConfig config; // Firebase Config data
    static FirebaseData configReceiveFirebaseData;
    static void setup();
    // static void uploadLocationtoFirebase(float temperatureValue, unsigned long custom_timestamp = 0);
    static void configStreamCallback(StreamData data);
    static void configStreamTimeoutCallback(bool timeout);
};

// extern FirebaseAuth auth; // The user UID can be obtained from auth.token.uid
// extern FirebaseConfig config; // Firebase Config data

void firebaseSetup();
void uploadLocationtoFirebase(float temperatureValue, unsigned long custom_timestamp = 0);
void configStreamCallback(StreamData data);
void configStreamTimeoutCallback(bool timeout);

#endif