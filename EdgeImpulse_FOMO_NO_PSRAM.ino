/**
 * Run Edge Impulse FOMO model.
 * It works on both PSRAM and non-PSRAM boards.
 * 
 * The difference from the PSRAM version
 * is that this sketch only runs on 96x96 frames,
 * while PSRAM version runs on higher resolutions too.
 * 
 * The PSRAM version can be found in my
 * "ESP32S3 Camera Mastery" course
 * at https://dub.sh/ufsDj93
 *
 * BE SURE TO SET "TOOLS > CORE DEBUG LEVEL = INFO"
 * to turn on debug messages
 */
#include <LiquidCrystal_I2C.h> 
#include <Wire.h> 
#include <your-fomo-project_inferencing.h>
#include <eloquent_esp32cam.h>
#include <eloquent_esp32cam/edgeimpulse/fomo.h>



using eloq::camera;
using eloq::ei::fomo;


#define SDA 15
#define SCL 14
#define COLUMNS 16
#define ROWS    2

 LiquidCrystal_I2C lcd(0x27, COLUMNS, ROWS);
/**
 * 
 */
String label0;
int motorcycle_count = 0;
int truck_count = 0;
int car_count = 0;
int bicycle_count = 0;
unsigned long timer0;
bool count = true;

void setup() {
    
    Wire.begin(SDA, SCL);
    lcd.begin();  
    lcd.clear();           
    lcd.backlight();   
    delay(3000);
    
    Serial.begin(115200);
    Serial.println("__EDGE IMPULSE FOMO (NO-PSRAM)__");

    // camera settings
    // replace with your own model!
    camera.pinout.aithinker();
    camera.brownout.disable();
    // NON-PSRAM FOMO only works on 96x96 (yolo) RGB565 images
    camera.resolution.yolo();
    camera.pixformat.rgb565();

    // init camera
    while (!camera.begin().isOk())
        Serial.println(camera.exception.toString());
    
    Serial.println("Camera OK");
    Serial.println("Put object in front of camera");
}


void loop() {


    // capture picture
    if (!camera.capture().isOk()) {
        Serial.println(camera.exception.toString());
        return;
    }

    // run FOMO
    if (!fomo.run().isOk()) {
      Serial.println(fomo.exception.toString());
      return;
    }

    // how many objects were found?
    Serial.printf(
      "Found %d object(s) in %dms\n", 
      fomo.count(),
      fomo.benchmark.millis()
    );

    // if no object is detected, return
    if (!fomo.foundAnyObject())
      return;

    // if you expect to find a single object, use fomo.first
    Serial.printf(
      "Found %s at (x = %d, y = %d) (size %d x %d). "
      "Proba is %.2f\n",
      fomo.first.label,
      fomo.first.x,
      fomo.first.y,
      fomo.first.width,
      fomo.first.height,
      fomo.first.proba
    );
    
    if(fomo.first.label){
      if(fomo.first.label != label0){
        lcd.clear();
       }
      label0 = fomo.first.label;
      lcd.setCursor(2, 0);
      lcd.print(label0);
      
      lcd.setCursor(2, 1);
      lcd.print("Proba is ");
      lcd.print(int(fomo.first.proba * 100));
      lcd.print("%");

      label0 = fomo.first.label;

    }
    else{
      label0 = "Finding ....";
      lcd.setCursor(2, 0);
      lcd.print(label0);
      }
    if(count == true){
      count = false;
      timer0 = millis();  
     }
    if(millis() - timer0 == 3000 && count == false){
      count == true;
      
      if (label0 == "car") {
        car_count += 1;
        Serial.print("Car Count: ");
        Serial.println(car_count);
      } 
      if (label0 == "truck") {
        truck_count += 1;
        Serial.print("Truck Count: ");
        Serial.println(truck_count);
      } 
      if (label0 == "motorcycle") {
        motorcycle_count += 1;
        Serial.print("Motorcycle Count: ");
        Serial.println(motorcycle_count);
      } 
      if (label0 == "bicycle") {
        bicycle_count += 1;
        Serial.print("Bicycle Count: ");
        Serial.println(bicycle_count);
      }
     }

     
    // if you expect to find many objects, use fomo.forEach
    if (fomo.count() > 1) {
      fomo.forEach([](int i, bbox_t bbox) {
        Serial.printf(
          "#%d) Found %s at (x = %d, y = %d) (size %d x %d). "
          "Proba is %.2f\n",
          i + 1,
          bbox.label,
          bbox.x,
          bbox.y,
          bbox.width,
          bbox.height,
          bbox.proba
        );
      });
    }
}
