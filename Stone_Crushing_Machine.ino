#define infraredSensor1 D2
#define infraredSensor2 D1
#define grinderActuator D7
#define coveyourActuator D6

#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>


// Provide the token generation process info.
#include "addons/TokenHelper.h"

// Insert Firebase project API Key
#define API_KEY "AIzaSyDZwlg6q3By0ESegmCmiZpEQpSqmlk7pZI"

/* 3. Define the project ID */
#define FIREBASE_PROJECT_ID "stone-crushing-machine"

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"


#define WIFI_SSID "DreamNetSDA" // enter the wifi address
#define WIFI_PASSWORD "Daniyal444" // enter it's password

String MACHINE_ID = "786786"; //Unique identity of machine
String MACHINE_DELAY = "5000";
bool MACHINE_STATE = false;

// Define Firebase objects~~~~~~~~~~~~~~
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;


FirebaseJson json;
FirebaseJsonData jsonData;


bool signupOK = false;



// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;
unsigned long previousMillis2 = 0;


// constants won't change:
const long intervalForConveyour = 500;           // interval at which to start Conveyour belt (milliseconds)

// constants won't change:
int long intervalForGrinder = 5000;           // how long grinder may work (milliseconds)

// constants won't change:
const long intervalForGettingDataFromFirebase = 5000;           // how long grinder may work (milliseconds)

bool isStonePicked = false;
int stoneCounter = 0;

bool isDataUpdated =  false; // For first time load of data


// Initialize WiFi
void initWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.println();
}



void setup() {

  Serial.begin(9600);

  pinMode(infraredSensor1, INPUT);
  pinMode(infraredSensor2, INPUT);
  pinMode(grinderActuator, OUTPUT);
  pinMode(coveyourActuator, OUTPUT);

  digitalWrite(grinderActuator, HIGH);
  digitalWrite(coveyourActuator, HIGH);

  // Initialize WiFi
  initWiFi();

  //Firebase ~~~~~~~~~~~~~~~~~START~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  /* Assign the api key (required) */
  config.api_key = API_KEY;


  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Signed up (ok)");
    signupOK = true;
  }
  else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  //Firebase ~~~~~~~~~~~~~~~~~END~~~~~~~~~~~~~~~~~~~~~~~~~~~~

}

// Loop Started~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void loop() {

  // check to see if it's time to on the grinder; that is, if the difference
  // between the current time and last time you turned on the machine is bigger than
  // the interval at which you want to turn on the grinder.
  unsigned long currentMillis = millis();


  //  For first time loading of Data~~~~~~~~~~~~~~~~~~~~~~~~~
  if (!isDataUpdated) {

    isDataUpdated = getDataFromFirebase();
  }

  //For check data when there is new Stone on machine~~~~~~~~~~~~~~~~~~~~
  int statusSensor1 = digitalRead(infraredSensor1);
  if (statusSensor1 == LOW && currentMillis - previousMillis2 >= intervalForGettingDataFromFirebase) // If there is stone in front of sensor 1
  {
    getDataFromFirebase();

    previousMillis2 = currentMillis;
  }

  //Conditions for machine~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (isDataUpdated && MACHINE_STATE) {

    intervalForGrinder = MACHINE_DELAY.toInt() * 1000;

    int statusSensor1 = digitalRead(infraredSensor1);
    int statusSensor2 = digitalRead(infraredSensor2);


    turnOnConveyourBelt(statusSensor1);
    isStoneReached(currentMillis, statusSensor2);


    turnOnGrinder(currentMillis, statusSensor1, statusSensor2);
  }

}
// Loop Ended~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


bool getDataFromFirebase() {

  String documentPath = "machines/" + MACHINE_ID;
  //    String mask = "delay";

  //If the document path contains space e.g. "a b c/d e f"
  //It should encode the space as %20 then the path will be "a%20b%20c/d%20e%20f"

  Serial.print("Get a document... ");

  if (Firebase.Firestore.getDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str()))
  {

    //Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
    json.setJsonData(fbdo.payload());

    json.get(jsonData, "fields/delay/integerValue");
    Serial.println(jsonData.type);
    Serial.println(jsonData.stringValue);
    MACHINE_DELAY = jsonData.stringValue;


    json.get(jsonData, "fields/state/booleanValue");
    Serial.println(jsonData.type);
    Serial.println(jsonData.boolValue);
    MACHINE_STATE = jsonData.boolValue;

    return true;

  }
  else
  {
    Serial.println(fbdo.errorReason());
    return false;
  }
}




void turnOnConveyourBelt(int statusSensor1)
{
  //  Conditions for conveyour belt~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (statusSensor1 == LOW) // If there is stone in front of sensor 1
  {
    digitalWrite(coveyourActuator, LOW); // Turn on conveyour
    isStonePicked = true;

    Serial.print("Turning on Conveyour belt: ");
    Serial.println(statusSensor1);
  }

}




void isStoneReached(int currentMillis, int statusSensor2) {

  if (statusSensor2 == LOW && currentMillis - previousMillis >= intervalForConveyour) // If there is stone in front of sensor 2
  {
    stoneCounter += 1;
    Serial.print("Number of stone Stone Reached: ");
    Serial.println(stoneCounter);
  }

}




void turnOnGrinder(int currentMillis, int statusSensor1, int statusSensor2)
{

  //Conditions for grinder machine~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (isStonePicked == true && currentMillis - previousMillis >= intervalForGrinder && stoneCounter >= 1 ) {

    Serial.print("Turning on Grinder after: ");
    Serial.println(intervalForGrinder);


    digitalWrite(grinderActuator, LOW); // Turn on ginder
    delay(intervalForGrinder);
    digitalWrite(grinderActuator, HIGH); // Turn off ginder
    stoneCounter = 0;

    Serial.println("Turning off Grinder");


    if (statusSensor1 == HIGH) // If there is no stone in front of sensor 1
    {
      digitalWrite(coveyourActuator, HIGH); // Turn off conveyour
      isStonePicked = false;

      Serial.println("Turning off Conveyour, no more stone is present");
    }

    // save the last time
    previousMillis = currentMillis;

  }

}
