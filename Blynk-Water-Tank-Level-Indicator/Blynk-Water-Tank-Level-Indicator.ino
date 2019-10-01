/*
 Blynk Water Tank Level Indicator
 Source: https://github.com/pkarun/Blynk-Water-Tank-Level-Indicator
 Reference: https://community.blynk.cc/t/water-tank-level-indicator-with-low-level-warning-notifications/26271
 */


#include "settings.h" //Make sure you UNCOMMENT this before you use.
//#include "my_settings.h" //This is my personal settings. You can remove this line or COMMENT it when you are using.

#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#include <ESP8266mDNS.h>  // For OTA with ESP8266
#include <WiFiUdp.h>  // For OTA
#include <ArduinoOTA.h>  // For OTA


BlynkTimer timer;



int sensSmoothArray1 [FILTERSAMPLES];   // array for holding raw sensor values for sensor1
int sensSmoothArray2 [FILTERSAMPLES];   // array for holding raw sensor values for sensor2

int rawData1, smoothData1;  // variables for sensor1 data
int rawData2, smoothData2;  // variables for sensor2 data

int long count = 0; //to ignore first few filtered data (power fail, device reset)


int volume; //used to calculate volume of water tank
int percent; //used to calculate percent of water present in water tank
float distanceinFeet; //used to calculate distance in feet
float waterFillLevel;  // to calculate at what height water was present in tank

long duration, distance, originalDistanceinCm;     // Duration used to calculate distance

int wifiStrength; //used to find wifi signal strenght

// Recurring Feet level alert variable

float topWaterLevelAlertValue; // for Top water level alert value storing
float topWaterLevelAlertOnOffState; // Top water level alert store ON or OFF state


//One time Percentage Greater than alert variable

int percentageGreaterThanOneTimeAlertOnOffState1;

int percentageGreaterThanOneTimeAlertValue1;

bool percentageGreaterThanOneTimeAlert1Flag = true;



// One time Percentage Less than alert variables

int lessThanPercentOneTimeAlertOnOffState1;

int lessThanPercentOneTimeAlertValue1;

bool lessThanPercentOneTimeAlert1Flag = true;


// One time Cm Less than alert variables

int lessThanCmOneTimeAlertOnOffState1;

int lessThanCmOneTimeAlertValue1;

bool lessThanCmOneTimeAlert1Flag = true;


// One time Liters Less than alert variables

int litersLessThanOneTimeAlertOnOffState1;

int litersLessThanOneTimeAlertValue1;

bool litersLessThanOneTimeAlert1Flag = true;



// One time Liters Greater than alert variables

int litersGreaterThanOneTimeAlertOnOffState1;

int litersGreaterThanOneTimeAlertValue1;

bool litersGreaterThanOneTimeAlert1Flag = true;


//Water level increasing variables

bool increaseTrigger = false;
int tempWaterLevel1 = 0;
int tempWaterLevel2 = 0;
int waterComingOnOff;

//Reset all alerts variable

int resetAlertStatus;


BLYNK_CONNECTED() { // runs once at device startup, once connected to server.

  Blynk.syncVirtual(VPIN_BUTTON_FEET_IS_LESS_THAN);
  Blynk.syncVirtual(VPIN_NUMERIC_FEET_IS_LESS_THAN);
  Blynk.syncVirtual(VPIN_BUTTON_RESET_ALL_ALERTS);
  Blynk.syncVirtual(VPIN_BUTTON_WATER_STATUS);
  Blynk.syncVirtual(VPIN_BUTTON_PERCENTAGE_GREATER_THAN);
  Blynk.syncVirtual(VPIN_NUMERIC_PERCENTAGE_GREATER_THAN);
  Blynk.syncVirtual(VPIN_BUTTON_PERCENTAGE_LESS_THAN);
  Blynk.syncVirtual(VPIN_NUMERIC_PERCENTAGE_LESS_THAN);
  Blynk.syncVirtual(VPIN_BUTTON_CM_LESS_THAN);
  Blynk.syncVirtual(VPIN_NUMERIC_CM_LESS_THAN);
  Blynk.syncVirtual(VPIN_BUTTON_LITERS_LESS_THAN);
  Blynk.syncVirtual(VPIN_BUTTON_LITERS_GREATER_THAN);
  Blynk.syncVirtual(VPIN_NUMERIC_LITERS_LESS_THAN);
  Blynk.syncVirtual(VPIN_NUMERIC_LITERS_GREATER_THAN);
}


void MeasureCm() {
  //  The following trigPin/echoPin cycle is used to determine the
  // distance of the nearest object by bouncing soundwaves off of it.

  digitalWrite(TRIGPIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIGPIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIGPIN, LOW);

  duration = pulseIn(ECHOPIN, HIGH);


  originalDistanceinCm = duration / 58.2; //We get water level distance from top in cm. Calculate the distance (in cm) based on the speed of sound. Distance in centimeters = Time / 58



  smoothData1 = digitalSmooth(originalDistanceinCm, sensSmoothArray1);  // every sensor you use with digitalSmooth needs its own array


  count++;  // we are using this counter to ignore first few filter data

  Serial.println(" ");
  Serial.print("Ignore Counter for Smooth Filter: ");
  Serial.print(count);
  Serial.println(" ");


  if (count > FILTERSAMPLES)

  {
    count = FILTERSAMPLES; // I thought of not to increase counter when it's satisfies our condition
    distance = smoothData1; // we assign fitered data only after ignoring first few samples so we can avoid water level starting from 0 cm

    Serial.print("Original Distance: ");
    Serial.print(originalDistanceinCm);
    Serial.println(" cm");
    Serial.print("Smooth Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
  }

  else {
    distance = originalDistanceinCm;
  }




  distanceinFeet = (distance / 30.48); //We get water level distance from top in feet. Divide the length value by 30.48 to convert cm to feet.

  waterFillLevel = (WATER_TANK_HEIGHT_IN_CM - distance); //total water tank hight - fill water level hight


  {

    if (distance >= MINIMUMRANGE && distance <= WATER_TANK_HEIGHT_IN_CM )  {  // here we are finding tank volume

      volume = ((WATER_TANK_LENGTH_IN_CM * waterFillLevel * WATER_TANK_WIDTH_IN_CM) / 1000); // Filled Volume = Length * Width * Fill Height (Liquid Height) and divide by 1000 because we are passing vlaues in cm so to convert to liters we need to divide
      percent = ((float)volume / FULLTANK) * 100;

    }

    if (distance >= WATER_TANK_HEIGHT_IN_CM) { //we don't want to display negative values
      volume = 0;
      percent = 0;
    }


    if (distance < MINIMUMRANGE) { //we don't want to display wrong data
      volume = FULLTANK;
      percent = 100;
    }


  }



  Serial.println(" ");

  Blynk.virtualWrite(VPIN_WATER_PERCENTAGE, percent); // virtual pin
  Serial.print("Percent of Water in Tank: ");
  Serial.print(percent);
  Serial.println("%");


  Blynk.virtualWrite(VPIN_LITERS_OF_WATER, volume); // virtual pin
  Serial.print("Liters of Water in Sump: ");
  Serial.print(volume);
  Serial.println(" Liters");

  Blynk.virtualWrite(VPIN_DISTANCE_IN_CM, distance);// virtual pin
  Serial.print("Distance in cm to Reach Water Level: ");
  Serial.print(distance);
  Serial.println(" cm");

  Blynk.virtualWrite(VPIN_DISTANCE_IN_FEET, distanceinFeet); // virtual pin
  Serial.print("Distance in Feet to Reach Water Level: ");
  Serial.print(distanceinFeet);
  Serial.println(" Feet");




}






//Data Filtering


int digitalSmooth(int rawIn, int *sensSmoothArray) {    // "int *sensSmoothArray" passes an array to the function - the asterisk indicates the array name is a pointer
  int j, k, temp, top, bottom;
  long total;
  static int i;
  //static int raw[FILTERSAMPLES];
  static int sorted[FILTERSAMPLES];
  boolean done;

  i = (i + 1) % FILTERSAMPLES;    // increment counter and roll over if necc. -  % (modulo operator) rolls over variable
  sensSmoothArray[i] = rawIn;                 // input new data into the oldest slot

  // Serial.print("raw = ");

  for (j = 0; j < FILTERSAMPLES; j++) { // transfer data array into anther array for sorting and averaging
    sorted[j] = sensSmoothArray[j];
  }

  done = 0;                // flag to know when we're done sorting
  while (done != 1) {      // simple swap sort, sorts numbers from lowest to highest
    done = 1;
    for (j = 0; j < (FILTERSAMPLES - 1); j++) {
      if (sorted[j] > sorted[j + 1]) {    // numbers are out of order - swap
        temp = sorted[j + 1];
        sorted [j + 1] =  sorted[j] ;
        sorted [j] = temp;
        done = 0;
      }
    }
  }

  /*
    for (j = 0; j < (FILTERSAMPLES); j++){    // print the array to debug
      Serial.print(sorted[j]);
      Serial.print("   ");
    }
    Serial.println();
  */

  // throw out top and bottom 15% of samples - limit to throw out at least one from top and bottom
  bottom = max(((FILTERSAMPLES * 15)  / 100), 1);
  top = min((((FILTERSAMPLES * 85) / 100) + 1  ), (FILTERSAMPLES - 1));   // the + 1 is to make up for asymmetry caused by integer rounding
  k = 0;
  total = 0;
  for ( j = bottom; j < top; j++) {
    total += sorted[j];  // total remaining indices
    k++;
    // Serial.print(sorted[j]);
    // Serial.print("   ");
  }

  //  Serial.println();
  //  Serial.print("average = ");
  //  Serial.println(total/k);
  return total / k;    // divide by number of samples
}





void wifiSignalStrength() { //function to calculate wifi signal strength

  wifiStrength = WiFi.RSSI(); //here we are getting wifi signal strength

  Blynk.virtualWrite(VPIN_WIFI_SIGNAL_STRENGTH, wifiStrength); // assigning value to V3
  Serial.print("WiFi Strength: "); // to print data on serial monitor
  Serial.println(wifiStrength);

  //For easy to understand values in words. Assign V5 on app.  got info here: https://support.randomsolutions.nl/827069-Best-dBm-Values-for-Wifi

  if (wifiStrength > -50)
  {
    Blynk.virtualWrite(VPIN_WIFI_STRENGTH_IN_WORDS, "Excellent");
    Serial.println("WiFi Signal is Excellent");
  }

  if (wifiStrength <= -50 && wifiStrength >= -60)
  {
    Blynk.virtualWrite(VPIN_WIFI_STRENGTH_IN_WORDS, "Good");
    Serial.println("WiFi Signal is Good");
  }

  if (wifiStrength <= -60 && wifiStrength >= -70)
  {
    Blynk.virtualWrite(VPIN_WIFI_STRENGTH_IN_WORDS, "Fair");
    Serial.println("WiFi Signal is Fair");
  }


  if (wifiStrength < -70)
  {
    Blynk.virtualWrite(VPIN_WIFI_STRENGTH_IN_WORDS, "Weak");
    Serial.println("WiFi Signal is Weak");
  }


}




//***************************************************************************
//*** TOP Water Level Alert - Continous Alert till we Turn OFF Alert  *******
//***************************************************************************




BLYNK_WRITE(VPIN_BUTTON_FEET_IS_LESS_THAN) // button widget is writing value to pin v6. Sending data from app (V6) to hardware(nodemcu)
{
  topWaterLevelAlertOnOffState = param.asInt();

}


BLYNK_WRITE(VPIN_NUMERIC_FEET_IS_LESS_THAN) // get numeric widget value from app to know top water level alert value
{
  topWaterLevelAlertValue = param.asFloat();


}


void topWaterLevelAlertCheck() {

  Serial.println(" ");
  Serial.println(" ****Top Feet Water Level Alerts**** ");
  Serial.println(" ");

  if (topWaterLevelAlertOnOffState == 0) //0 is HIGH or ON in Nodemcu; If we enable water level check on app, then it perform below actions
  {
    Serial.println("Alert status: ON");
    topWaterLevelAlert(); //go and execute this function
  }
  else {
    Serial.println("Alert status: OFF");
  }
}


void topWaterLevelAlert() {


  Serial.print("Alert Value: ");
  Serial.print(topWaterLevelAlertValue);
  Serial.println (" Feet");


  if (topWaterLevelAlertValue >= distanceinFeet) // if top water level alert value is greater than current water level distance; means it triggers when water level cross specified water level in feet
  {

    Blynk.notify(String("Water Level is Less than ") + topWaterLevelAlertValue + " Feet now (from Top)" ); //here we are sending push notification if the above condition satisfies
    Serial.println("Alert Triggered");
  }
}







//**********************************************************************
//************* ALERT 1: Percentage Greater than One time alert function *****
//**********************************************************************

BLYNK_WRITE(VPIN_BUTTON_PERCENTAGE_GREATER_THAN) // button widget is writing value to pin v15. Sending data from app (V15) to hardware(nodemcu)
{
  percentageGreaterThanOneTimeAlertOnOffState1 = param.asInt();

}


BLYNK_WRITE(VPIN_NUMERIC_PERCENTAGE_GREATER_THAN) // get numeric widget value from app to know top water level alert value
{
  percentageGreaterThanOneTimeAlertValue1 = param.asInt();


}



void percentageGreaterThanAlertCheck1() {

  Serial.println(" ");
  Serial.println(" **** Percentage Greater Than One Time Water Level Alert 1 **** ");
  Serial.println(" ");

  if (percentageGreaterThanOneTimeAlertOnOffState1 == 0) //0 is HIGH or ON in Nodemcu; If we enable alert check on app, then it perform below actions
  {
    Serial.println("Alert Status: ON");
    Serial.print("Alert Value: ");
    Serial.print(percentageGreaterThanOneTimeAlertValue1);
    Serial.println("%");
    percentageGreaterThanOneTimeAlert1(); //go and execute this function
  }
  else {
    Serial.println("Alert Status: OFF");
  }
}



void percentageGreaterThanOneTimeAlert1() {

  checkGreaterThanOneTimeAlert1PercentageFlag();

  if (percentageGreaterThanOneTimeAlertValue1 < percent && percentageGreaterThanOneTimeAlert1Flag == true) // if water level is greater than specified water level percentage then send alert
  {

    Blynk.notify(String("Water is Greater than: ") + percentageGreaterThanOneTimeAlertValue1 + "% now." ); //here we are sending push notification if the above condition satisfies
    Serial.println("Alert Triggered");
    percentageGreaterThanOneTimeAlert1Flag = false; //We set flase so that we don't get alerts repeatedly
    Serial.println("Alert Flag status: FALSE");

  }

}




void checkGreaterThanOneTimeAlert1PercentageFlag() {

  if (percentageGreaterThanOneTimeAlertValue1 > percent)
  {
    percentageGreaterThanOneTimeAlert1Flag = true;
    Serial.println("Alert Flag status: TRUE");

  }

}


//************* ALERT 1: Percentage Greater than One time alert function END *************










//*****************************************************************
//************* ALERT 1: Percentage Less than One time alert function *****
//*****************************************************************

BLYNK_WRITE(VPIN_BUTTON_PERCENTAGE_LESS_THAN) // button widget is writing value to pin v15. Sending data from app (V15) to hardware(nodemcu)
{
  lessThanPercentOneTimeAlertOnOffState1 = param.asInt();

}


BLYNK_WRITE(VPIN_NUMERIC_PERCENTAGE_LESS_THAN) // get numeric widget value from app to know top water level alert value
{
  lessThanPercentOneTimeAlertValue1 = param.asInt();


}



void lessThanOneTimePercentAlertCheck1() {

  Serial.println(" ");
  Serial.println(" **** Less Than One Time Water Level Percent Alert 1 **** ");
  Serial.println(" ");

  if (lessThanPercentOneTimeAlertOnOffState1 == 0) //0 is HIGH or ON in Nodemcu; If we enable alert check on app, then it perform below actions
  {
    Serial.println("Alert Status: ON");
    Serial.print("Alert Value: ");
    Serial.print(lessThanPercentOneTimeAlertValue1);
    Serial.println("%");
    lessThanOneTimePercentAlert1(); //go and execute this function
  }
  else {
    Serial.println("Alert Status: OFF");
  }
}



void lessThanOneTimePercentAlert1() {

  checkLessThanOneTimePercentAlert1Flag();

  if (lessThanPercentOneTimeAlertValue1 > percent && lessThanPercentOneTimeAlert1Flag == true) // if water level is less than specified water level percentage then send alert
  {

    Blynk.notify(String("Water Level Less than: ") + lessThanPercentOneTimeAlertValue1 + "% now." ); //here we are sending push notification if the above condition satisfies
    Serial.println("Alert Triggered");
    lessThanPercentOneTimeAlert1Flag = false; //We set flase so that we don't get alerts repeatedly
    Serial.println("Alert Flag status: FALSE");

  }

}


void checkLessThanOneTimePercentAlert1Flag() {

  if (lessThanPercentOneTimeAlertValue1 < percent)
  {
    lessThanPercentOneTimeAlert1Flag = true;
    Serial.println("Alert Flag status: TRUE");

  }

}


//************* ALERT 1: Percentage Less than One time alert function END *************






//*****************************************************************
//************* ALERT 1: Cm Less than One time alert function *****
//*****************************************************************

BLYNK_WRITE(VPIN_BUTTON_CM_LESS_THAN) // button widget is writing value to pin v15. Sending data from app (V15) to hardware(nodemcu)
{
  lessThanCmOneTimeAlertOnOffState1 = param.asInt();

}


BLYNK_WRITE(VPIN_NUMERIC_CM_LESS_THAN) // get numeric widget value from app to know top water level alert value
{
  lessThanCmOneTimeAlertValue1 = param.asInt();


}



void lessThanOneTimeCmAlertCheck1() {

  Serial.println(" ");
  Serial.println(" **** Less Than One Time Water Level Cm Alert 1 **** ");
  Serial.println(" ");

  if (lessThanCmOneTimeAlertOnOffState1 == 0) //0 is HIGH or ON in Nodemcu; If we enable alert check on app, then it perform below actions
  {
    Serial.println("Alert Status: ON");
    Serial.print("Alert Value: ");
    Serial.print(lessThanCmOneTimeAlertValue1);
    Serial.println("cm");
    lessThanOneTimeCmAlert1(); //go and execute this function
  }
  else {
    Serial.println("Alert Status: OFF");
  }
}



void lessThanOneTimeCmAlert1() {

  checkLessThanOneTimeCmAlert1Flag();

  if (lessThanCmOneTimeAlertValue1 > distance && lessThanCmOneTimeAlert1Flag == true) // if water level is less than specified water level percentage then send alert
  {

    Blynk.notify(String("Water Distance is Less than (From Top): ") + lessThanCmOneTimeAlertValue1 + "cm now." ); //here we are sending push notification if the above condition satisfies
    Serial.println("Alert Triggered");
    lessThanCmOneTimeAlert1Flag = false; //We set flase so that we don't get alerts repeatedly
    Serial.println("Alert Flag status: FALSE");

  }

}



void checkLessThanOneTimeCmAlert1Flag() {

  if (lessThanCmOneTimeAlertValue1 < distance)
  {
    lessThanCmOneTimeAlert1Flag = true;
    Serial.println("Alert Flag status: TRUE");

  }

}




//************* ALERT 1: Cm Less than One time alert function END *************





//**********************************************************************
//************* ALERT 1: Liters Less than One time alert function *****
//**********************************************************************

BLYNK_WRITE(VPIN_BUTTON_LITERS_LESS_THAN) // button widget is writing value to pin v15. Sending data from app (V15) to hardware(nodemcu)
{
  litersLessThanOneTimeAlertOnOffState1 = param.asInt();

}


BLYNK_WRITE(VPIN_NUMERIC_LITERS_LESS_THAN) // get numeric widget value from app to know top water level alert value
{
  litersLessThanOneTimeAlertValue1 = param.asInt();


}



void litersLessThanAlertCheck1() {

  Serial.println(" ");
  Serial.println(" **** Liters Less Than One Time Water Level Alert 1 **** ");
  Serial.println(" ");

  if (litersLessThanOneTimeAlertOnOffState1 == 0) //0 is HIGH or ON in Nodemcu; If we enable alert check on app, then it perform below actions
  {
    Serial.println("Alert Status: ON");
    Serial.print("Alert Value: ");
    Serial.print(litersLessThanOneTimeAlertValue1);
    Serial.println(" Liters");
    litersLessThanOneTimeAlert1(); //go and execute this function
  }
  else {
    Serial.println("Alert Status: OFF");
  }
}



void litersLessThanOneTimeAlert1() {

  checkLessThanOneTimeAlert1LitersFlag();

  if (litersLessThanOneTimeAlertValue1 > volume && litersLessThanOneTimeAlert1Flag == true) // if water level is less than specified water level percentage then send alert
  {

    Blynk.notify(String("Water is Less than: ") + litersLessThanOneTimeAlertValue1 + " Lts now." ); //here we are sending push notification if the above condition satisfies
    Serial.println("Alert Triggered");
    litersLessThanOneTimeAlert1Flag = false; //We set flase so that we don't get alerts repeatedly
    Serial.println("Alert Flag status: FALSE");

  }

}




void checkLessThanOneTimeAlert1LitersFlag() {

  if (litersLessThanOneTimeAlertValue1 < volume)
  {
    litersLessThanOneTimeAlert1Flag = true;
    Serial.println("Alert Flag status: TRUE");

  }

}


//************* ALERT 1: Liters Less than One time alert function END *************





//**********************************************************************
//************* ALERT 1: Liters Greater than One time alert function *****
//**********************************************************************

BLYNK_WRITE(VPIN_BUTTON_LITERS_GREATER_THAN) // button widget is writing value to pin v15. Sending data from app (V15) to hardware(nodemcu)
{
  litersGreaterThanOneTimeAlertOnOffState1 = param.asInt();

}


BLYNK_WRITE(VPIN_NUMERIC_LITERS_GREATER_THAN) // get numeric widget value from app to know top water level alert value
{
  litersGreaterThanOneTimeAlertValue1 = param.asInt();


}



void litersGreaterThanAlertCheck1() {

  Serial.println(" ");
  Serial.println(" **** Liters Greater Than One Time Water Level Alert 1 **** ");
  Serial.println(" ");

  if (litersGreaterThanOneTimeAlertOnOffState1 == 0) //0 is HIGH or ON in Nodemcu; If we enable alert check on app, then it perform below actions
  {
    Serial.println("Alert Status: ON");
    Serial.print("Alert Value: ");
    Serial.print(litersGreaterThanOneTimeAlertValue1);
    Serial.println(" Liters");
    litersGreaterThanOneTimeAlert1(); //go and execute this function
  }
  else {
    Serial.println("Alert Status: OFF");
  }
}



void litersGreaterThanOneTimeAlert1() {

  checkGreaterThanOneTimeAlert1LitersFlag();

  if (litersGreaterThanOneTimeAlertValue1 < volume && litersGreaterThanOneTimeAlert1Flag == true) // if water level is greater than specified water level percentage then send alert
  {

    Blynk.notify(String("Water is Greater than: ") + litersGreaterThanOneTimeAlertValue1 + " Lts now." ); //here we are sending push notification if the above condition satisfies
    Serial.println("Alert Triggered");
    litersGreaterThanOneTimeAlert1Flag = false; //We set flase so that we don't get alerts repeatedly
    Serial.println("Alert Flag status: FALSE");

  }

}




void checkGreaterThanOneTimeAlert1LitersFlag() {

  if (litersGreaterThanOneTimeAlertValue1 > volume)
  {
    litersGreaterThanOneTimeAlert1Flag = true;
    Serial.println("Alert Flag status: TRUE");

  }

}


//************* ALERT 1: Liters Greater than One time alert function END *************




//***************************************************************************
//************************ Water Coming Alert  ******************************
//***************************************************************************



BLYNK_WRITE(VPIN_BUTTON_WATER_STATUS) // button widget is writing value to pin v15. Sending data from app (V15) to hardware(nodemcu)
{
  waterComingOnOff = param.asInt();

}

void waterComingCheck()
{
  Serial.println(" ");
  Serial.println(" **** Water Coming Alert **** ");
  Serial.println(" ");

  if (waterComingOnOff == 0)
  {

    Serial.println("Alert Status: ON");
    waterComing();

  }
  else
  {
    Serial.println("Alert Status: OFF");
  }
}

void waterComing()
{
  if (tempWaterLevel1 == 0)
  {
    tempWaterLevel1 = percent;
  }

  if (tempWaterLevel2 == 0)
  {
    tempWaterLevel2 = percent;
  }


  if (!increaseTrigger && percent > tempWaterLevel1)
  {
    tempWaterLevel1 = percent;
    tempWaterLevel2 = percent;
    Blynk.notify("Water is coming now...");
    Serial.println("Water is coming now...");
    increaseTrigger = true;
  }

  else if (percent >= tempWaterLevel2)
  {
    tempWaterLevel2 = percent;
  }
  else if (percent < tempWaterLevel2)
  {
    tempWaterLevel1 = percent;
    tempWaterLevel2 = percent;
    increaseTrigger = false;
  }
}




//***************************************************************************
//******************************* Reset all Alerts  *************************
//***************************************************************************



BLYNK_WRITE(VPIN_BUTTON_RESET_ALL_ALERTS) // button widget is writing value to pin v6. Sending data from app (V6) to hardware(nodemcu)
{
  resetAlertStatus = param.asInt();

}

void resetAlerts() {

  if (resetAlertStatus == 0)
  {
    Serial.println("All alerts status set to: TRUE");
    percentageGreaterThanOneTimeAlert1Flag = true;
    lessThanPercentOneTimeAlert1Flag = true;
    lessThanCmOneTimeAlert1Flag = true;
    litersLessThanOneTimeAlert1Flag = true;
    litersGreaterThanOneTimeAlert1Flag = true;
    increaseTrigger = false;

  }
  else {
    Serial.println("Reset button is not pressed");
  }

}




void setup() {

  Serial.begin(9600);
  //Blynk.begin(auth, ssid, pass);
  Blynk.begin(AUTH, WIFI_SSID, WIFI_PASS, SERVER, PORT);
  ArduinoOTA.setHostname(OTA_HOSTNAME);  // For OTA - Use your own device identifying name
  ArduinoOTA.begin();  // For OTA



  pinMode(TRIGPIN, OUTPUT);
  pinMode(ECHOPIN, INPUT);



  //15sec
  timer.setInterval(15000L, MeasureCm);
  timer.setInterval(15000L, wifiSignalStrength);
  timer.setInterval(15000L, topWaterLevelAlertCheck);
  timer.setInterval(15000L, percentageGreaterThanAlertCheck1);
  timer.setInterval(15000L, lessThanOneTimePercentAlertCheck1);
  timer.setInterval(15000L, lessThanOneTimeCmAlertCheck1);
  timer.setInterval(15000L, litersLessThanAlertCheck1);
  timer.setInterval(15000L, litersGreaterThanAlertCheck1);
  timer.setInterval(15000L, waterComingCheck);
  timer.setInterval(15000L, resetAlerts);




}

void loop() {

  Blynk.run(); // Initiates Blynk
  ArduinoOTA.handle();  // For OTA
  timer.run(); // Initiates SimpleTimer


}
