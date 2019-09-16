//Download all essential libraries by searching on Tools->Manage Libraries from Arduino IDE menu

#include <Wire.h> 
#include <Keypad.h>

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {     //4x4 keypad is used here
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = { 11, 10, 9, 8 };
byte colPins[COLS] = { 7, 6, 5, 4 };
Keypad myKeypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,20,4);

#include <SoftwareSerial.h>
SoftwareSerial fingerPrint(2, 3);


char keypressed;
char code[]={'1','2','3','4'}; //Password (you can also change this by any numbers or alphabets)
char c[4];                     //Here 1234 is the default password which you can change it
int ij;


#include <Adafruit_Fingerprint.h>
uint8_t id;
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerPrint);



void setup()  
{
  Serial.begin(9600); //For serial comm.
 
  finger.begin(57600); //For Fingerprint Sensor comm.
 
  lcd.init();  
  lcd.backlight();
  lcd.clear();
  lcd.print("loading....");
  delay(2000);
  finger.getTemplateCount();
  lcd.clear();
  lcd.print("No of User ID:");
  lcd.setCursor(0,1); 
  lcd.print(finger.templateCount); 
  delay(2000);
  lcd.clear();
  }
  




void loop() 
{ 
  getFingerprintIDez();                //Match fingerprint function in loop.
  keypressed = myKeypad.getKey();      //Reading the characters typed by using the keypad.
  if(keypressed == 'A'){               //If it's 'A' it triggers Enroll process.

            ij=0;                      
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Enter Password");
            getPassword();            //Check for password 
            if(ij==4){                //If the passcode is correct we can start enrolling new finger template
            Enrolling();              //Enrolling function
            delay(2000);
            lcd.clear();
            }
            else{                    //If the code is wrong we can't add new users (templates)
            lcd.setCursor(0,0);
            lcd.print("Incorrect Passwd");
            delay(1000);
            lcd.clear();
            lcd.print("Enter again");
            delay(1000);
            lcd.clear();
            }
    
  
  }
 if(keypressed == 'B')                //If B key is pressed, Delete process is triggered.
 {                            
            ij=0;                      //ij is set to 0 again for B. 
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Enter Password");
            getPassword();            
            if(ij==4){                
            Deleting();              //Deleting function
            delay(2000);
            lcd.clear();
            }
            else{                    
            lcd.setCursor(0,0);
            lcd.print("Incorrect Passwd");
            delay(1000);
            lcd.clear();
            lcd.print("Enter again");
            delay(1000);
            lcd.clear();
            }
  
  
 }
  
  lcd.setCursor(0,0);
  lcd.print("Place finger"); //loop msg for match function.
  delay(50);
}

//Getting password (code) function, it gets the characters typed
//and store them in c[4] array
void getPassword(){
   for (int i=0 ; i<4 ; i++){
    c[i]= myKeypad.waitForKey();
    lcd.setCursor(i,1);
    lcd.print("*");
   }
   lcd.clear();
   for (int j=0 ; j<4 ; j++){ //comparing the code entred with the code stored
    if(c[j]==code[j])
     ij++;                    //Everytime the char is correct we increment the ij until it reaches 4 which means the code is correct
   }                          //Otherwise it won't reach 4 and it will show "wrong code" as written above
}



void Enrolling()  {
  keypressed = NULL;
  lcd.clear();
  lcd.print("Enroll New User");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Enter ID:");
  id = readnumber();                           
  if (id == 0) {// ID #0 not allowed, try again!
     return;
  }
  
  while (!  getFingerprintEnroll() );
  }


void Deleting()   {
  keypressed = NULL;
  lcd.clear();
  lcd.print("Delete User");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Enter ID to Del:");
  id = readnumber();
  if (id == 0) {// ID #0 not allowed, try again!
     return;
  }
  
  while (!  deleteFingerprint(id) );
  }



//Enroll

uint8_t getFingerprintEnroll() {

  int p = -1;
  lcd.clear();
  lcd.print("Enroll New User:"); //Message to print for every step
  lcd.setCursor(10,0);
  lcd.print(id);
  lcd.setCursor(0,1);
  lcd.print("Place finger"); //First step
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      break;
    case FINGERPRINT_IMAGEMESS:
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      return p;
    case FINGERPRINT_FEATUREFAIL:
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      return p;
    default:
      return p;
  }
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Remove finger"); //After getting the first template successfully
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  
  p = -1;

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Place same"); //We launch the same thing another time to get a second template of the same finger
  lcd.setCursor(0,1);
  lcd.print("finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      break;
    case FINGERPRINT_IMAGEMESS:
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      return p;
    case FINGERPRINT_FEATUREFAIL:
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      return p;
    default:
      return p;
  }
  
  
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    lcd.clear();
    lcd.print("Prints Mismatch");
    lcd.setCursor(0,1);
    lcd.print("Try Again");
    delay(2000);
    lcd.clear();
    return p;
  } else {
    return p;
  }   
  
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
  lcd.clear();                //if both images are gotten without problem we store the template as the Id we entred
  lcd.setCursor(0,0);
  lcd.print("Stored in");    //Print a message after storing and showing the ID where it's stored
  lcd.setCursor(0,1);
  lcd.print("ID: ");
  lcd.setCursor(5,1);
  lcd.print(id);
  delay(3000);
  lcd.clear();
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    return p;
  } else {
    return p;
  }   
}

//This function gets the ID number as 3 digits format like 001 for ID 1


//Readnumber

uint8_t readnumber(void) {
  uint8_t num = 0;
   while (num == 0) {
      char keey = myKeypad.waitForKey();
    int  num1 = keey-48;
         lcd.setCursor(0,1);
         lcd.print(num1);
         keey = myKeypad.waitForKey();
    int  num2 = keey-48;
         lcd.setCursor(1,1);
         lcd.print(num2);
         keey = myKeypad.waitForKey();
    int  num3 = keey-48;
         lcd.setCursor(2,1);
         lcd.print(num3);
         delay(1000);
         num=(num1*100)+(num2*10)+num3;
         keey=NO_KEY;
  }
  return num;
}

//Fingerprint match checking

int getFingerprintIDez() {
  uint8_t p = finger.getImage();        
  if (p != FINGERPRINT_OK)  return -1;  

  p = finger.image2Tz();               
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();     
  if (p != FINGERPRINT_OK){          
    lcd.clear();                     
    lcd.print("Access denied");
    delay(2000);
    lcd.clear();
    lcd.print("Place finger");
    return -1;
  }
  
  
  //If we found a match we proceed to this process
  
  lcd.clear();
  lcd.print("Welcome");        //Printing a message for the recognized template
  lcd.setCursor(2,1);
  lcd.print("ID: ");
  lcd.setCursor(5,1);
  lcd.print(finger.fingerID); //And the ID of the finger template
  
  delay(5000);
  
  lcd.clear();
  lcd.print("Place finger");
  return finger.fingerID; 
  
}

//Delete fingerprint ID

uint8_t deleteFingerprint(uint8_t id) {
  uint8_t p = -1;
  
  p = finger.deleteModel(id);

  if (p == FINGERPRINT_OK) {
    lcd.clear();
    lcd.print("Deleted!");
    delay(1000);
    
  finger.getTemplateCount();
  lcd.clear();
  lcd.print("No of User ID:");
  lcd.setCursor(0,1); 
  lcd.print(finger.templateCount); 
  delay(2000);
  lcd.clear();

  
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    lcd.clear();
    lcd.println("Comm error");
    delay(1000);
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    lcd.clear();
    lcd.print("Location error");
    delay(1000);
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    lcd.clear();
    lcd.print("Error flashwrite");
    delay(1000);
    return p;
  } else {
    lcd.clear();
    lcd.print("Unknown error:");
    delay(1000);
    lcd.clear(); 
    lcd.print(p, HEX);
    delay(1000);
    return p;
  }   
}
