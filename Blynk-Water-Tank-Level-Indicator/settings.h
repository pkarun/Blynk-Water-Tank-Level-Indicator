                    
                           
                           /////////////////////////////////////////////////////////////////
                          //                           Settings                          //
                         /////////////////////////////////////////////////////////////////



/***************************************************
 *      Hardware Settings
 **************************************************/

#define ECHOPIN 14  // Echo Pin D5
#define TRIGPIN 12 // Trigger Pin D6
                         

/***************************************************
 *        Variable Settings
 **************************************************/

#define FILTERSAMPLES 21  // filterSamples should  be an odd number, no smaller than 3

//Our rectangular tank capacity in liters; 39,600 Liters full tank capacity
//(length x width x height)/1000(to convert to ltrs) = (472*290*274)/1000=37505120/1000 = 37505.12 liters; I just used 37,000 Ltrs

#define FULLTANK 37000  //in liters                         
#define MINIMUMRANGE 20 // sensor needs minimum 20 cm distance to work


#define WATER_TANK_LENGTH_IN_CM 472 // Water tank lenght in cm
#define WATER_TANK_WIDTH_IN_CM 290 // water tank width in cm

//Tank original height(depth) is: 310cm. Sensor takes 2 inches (5.08 cm) of height and sensor need minimum 20cm distance to work.
// And I keep 10cm keep extra margin.
//So from our total tank height we need reduce from extra space left in tank that is, (sensor height + sensor minimum distance)
//Waste tank height = 5.08cm + 20cm + 10cm = 35.08cm.  New tank height = original tank height - waste tank height.
//New tank height = 310cm - 35.08cm = 274.92cm; I consider 274cm.

#define WATER_TANK_HEIGHT_IN_CM 274 // 274cm is after reducing 20cm + 5.08cm +10cm height from original tank height to work sensor
 


/***************************************************
 *        Blynk Virtual Pin Assignment
 **************************************************/

//Sending values to blynk VPIN
#define VPIN_WATER_PERCENTAGE                 V9
#define VPIN_LITERS_OF_WATER                  V4
#define VPIN_DISTANCE_IN_CM                   V12
#define VPIN_DISTANCE_IN_FEET                 V13
#define VPIN_WIFI_SIGNAL_STRENGTH             V3
#define VPIN_WIFI_STRENGTH_IN_WORDS           V5

//Numeric Input Widget on Blynk
#define VPIN_NUMERIC_FEET_IS_LESS_THAN        V10 //Recursive Alert. We don't want to soak our sensor in water.
#define VPIN_NUMERIC_CM_LESS_THAN             V46
#define VPIN_NUMERIC_PERCENTAGE_LESS_THAN     V36
#define VPIN_NUMERIC_LITERS_LESS_THAN         V56
#define VPIN_NUMERIC_LITERS_GREATER_THAN      V57 
#define VPIN_NUMERIC_PERCENTAGE_GREATER_THAN  V20
   
//Button widget on blynk app
#define VPIN_BUTTON_WATER_STATUS              V14
#define VPIN_BUTTON_FEET_IS_LESS_THAN         V6  //Recursive Alert. We don't want to soak our sensor in water.
#define VPIN_BUTTON_CM_LESS_THAN              V40
#define VPIN_BUTTON_PERCENTAGE_LESS_THAN      V30
#define VPIN_BUTTON_LITERS_LESS_THAN          V50
#define VPIN_BUTTON_LITERS_GREATER_THAN       V51
#define VPIN_BUTTON_PERCENTAGE_GREATER_THAN   V15
#define VPIN_BUTTON_RESET_ALL_ALERTS          V11 //Resets all alerts

/***************************************************
 * 
 *        Server Settings
 **************************************************/
      
#define OTA_HOSTNAME "Water Level Indicator"
 
