//-------------------------------------------------------< Servo Config >
#include <Servo.h>
Servo servo;

//-------------------------------------------------------< LCD Config >
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

//-------------------------------------------------------< Keypad Config >
#include <Keypad.h>
#define ROW_NUM 4 // ประกาศ 4 แถว
#define COLUMN_NUM 3 // ประกาศ 3 คอลัมน์

char keys[ROW_NUM][COLUMN_NUM] = {
{'1', '2', '3'},
{'4', '5', '6'},
{'7', '8', '9'},
{'*', '0', '#'}
};

byte pin_rows[ROW_NUM] = {8, 7, 6, 5}; //เชื่อมต่อกับ pinouts ของแถว ของปุ่มกด
byte pin_column[COLUMN_NUM] = {4, 3, 2}; //เชื่อมต่อกับ pinout ของคอลัมน์ ของปุ่มกด

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

const String password = “1504”; // กำหนดรหัสผ่าน
String input_password;
String raw_pass;

//-------------------------------------------------------< Device Variables >
int index = 4;

int doorValue = 0;
int doorState_k0 = 0;
int doorState_k1 = 0;
int count = 0;

//-------------------------------------------------------< System Variables >
int state = 0;

//=============================================================< setup >
void setup() {
  Serial.begin(9600);
  lcd.begin();
  
  input_password.reserve(32); // maximum password size is 32, change if needed

  servo.attach(9);
  pinMode(10, INPUT_PULLUP);  // Magetic sensor

  lcd.setCursor(1, 0);  lcd.print("INPUT PASSWORD");
  lcd.setCursor(index, 1);  lcd.print("* * * *");
}

//=============================================================< loop >
void loop() {
  //++++++++++++++++++++++++++++++++++++++++++++++++++< state 0 >
  if(state == 0){
    doorValue = digitalRead(10);
    if(doorValue == 1){ // turn on system & Door is close.
      lcd.setCursor(1, 0);  lcd.print("INPUT PASSWORD");
      lcd.setCursor(index, 1);  lcd.print("* * * *");
      state = 1;
    }
    else{  // turn on system & Door is open.
      state = 2;
    }
  }

  //++++++++++++++++++++++++++++++++++++++++++++++++++< state 1 >
  if(state == 1){
    char key = keypad.getKey();

    if (key) {  //----------------< if pressed some key
      if (key == '*'){  //----------------< reset password
        input_password = "";
        raw_pass = "";
        index = 4;
        lcd.clear();
        lcd.setCursor(1, 0);  lcd.print("INPUT PASSWORD");
        lcd.setCursor(index, 1);  lcd.print("* * * *");
      }
      else if (key == '#'){ //----------------< confirmed password
        if (raw_pass == password) { //----------------< if correct
          lcd.clear();
          lcd.setCursor(0, 5);  lcd.print("UNLOCK");
          Servo.write(0); //----------------< Unlock Door
          state = 3;
        } 
        else{                     //----------------< if not correct
          lcd.setCursor(0, 4);  lcd.print("PASSWARD");
          lcd.setCursor(1, 2);  lcd.print("NOT CONNECT");
          delay(1000);
          lcd.clear();
          lcd.setCursor(1, 0);  lcd.print("INPUT PASSWORD");
          lcd.setCursor(index, 1);  lcd.print("* * * *");
        }
        // reset values
        input_password = "";  
        raw_pass = "";
        index = 4;
      }
      else {
        raw_pass += key;  // for compare with password
        input_password = key; // for display in LCD
        lcd.setCursor(index, 1);  lcd.print(input_password);
        index = index + 2;
      }
    }
    else{
      Servo.write(90); //----------------< Lock Door
    }
  }

  //++++++++++++++++++++++++++++++++++++++++++++++++++< state 2 >
  if(state == 2){
    lcd.setCursor(5, 0);  lcd.print("PLEASE");
    lcd.setCursor(0, 1);  lcd.print("CLOSE THE DOOR");
    doorValue = digitalRead(10);
    if(doorValue == 1){
      delay(1500);
      servo.write(90);  //----------------< Lock Door
      lcd.clear();
      state = 0;
    }
  }
  
  //++++++++++++++++++++++++++++++++++++++++++++++++++< state 3 >
  if(state == 3){
    doorValue = digitalRead(10);

    if(doorState_k0 == doorState_k1){ 
      if(doorValue == 0){ //Open
        doorState_k1 = 1;
        count++;
      }
    }
    else if(doorState_k0 != doorState_k1){
      if(doorValue == 1){ //Close
        doorState_k1 = 0;
        count++;
      }
    }

    if(count == 2){ //----------------< closed door
      delay(1500);
      servo.write(90);  //----------------< Lock Door
      state = 0;
    }

  }
  
}


