#include<Keypad.h>
#include <EEPROM.h>

const byte ROWS = 4;
const byte COLUMNS = 3;
const byte INITIAL_ADDRESS = 10;
//byte address = 10;
const unsigned long long_press_interval = 3000;
unsigned long previous_time = 0;
byte counter = 0;
char track_press[4];
int track_press_counter = 0;
bool double_press_state = false;
const byte PHONE_NUMBER_ADDRESS = 20;
char phone_array[12]={0};
int buzzer=12;
unsigned int frequency=5000;




char keypad_digits[ROWS][COLUMNS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

//connections to arduino
byte rowPins[ROWS] = {9, 8, 7, 6};
byte columnPins[COLUMNS] = {5, 4, 3};

//CREATE KEYPAD OBJECT

Keypad custom_keypad = Keypad(makeKeymap(keypad_digits), rowPins, columnPins, ROWS, COLUMNS);


int eeprom_read(void);
void store_eeprom(char custom_key, int address);
void confirm_password();
void double_press(char key_gotten);
void delete_number(byte local_address,byte constant_initial_address ) ;
void password_validation(int password_length);
char get_phone_number();
void save_phone_number();


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  

}
void loop() {
  // put your main code here, to run repeatedly:
  create_new_password();
  save_phone_number();
  static byte flag=0;
  if(flag==0){
  Serial.println("Welcome Back!Please enter your password.Press # to confirm:");
  flag=1;
  }
  char key = custom_keypad.getKey();
  if (key == '*') {
    double_press(key);
  }
 
  confirm_password();



}
void create_new_password() {
  int local_address = INITIAL_ADDRESS;
  //Serial.println(local_address);
  int password_length = 0;
  if (eeprom_read() == 0) {
    Serial.println("This is the first time you are logging in.");
    Serial.println("Please enter a new password(Must not start with 0 and only four digits long).Press # to save password and * delete");

    char customKey = 0;
    while (customKey != '#' ) {
      customKey = custom_keypad.getKey();
      if (customKey) {
        
        Serial.println(customKey);
        if (customKey != '*') 
        {
          tone_sounds(100);
          store_eeprom(customKey,local_address);

          local_address = local_address + 1;
          if (local_address == EEPROM.length()) {
            local_address = INITIAL_ADDRESS;
          }
         // Serial.println(local_address);
          password_length++;
          //Serial.println(password_length);
        } else if (customKey == '*') {
          tone_sounds(100);
          delete_number(local_address,INITIAL_ADDRESS);
          local_address--;
          password_length--;
          Serial.println("Key deleted");
        }

      }

    }

    if (customKey == '#') {
      tone_sounds(100);
      tone_sounds(100);
      password_validation(password_length);
    }

    char check_value = 0;
    for (int i = INITIAL_ADDRESS; (check_value != '#') && (i < EEPROM.length()); i++) {
      check_value = EEPROM.read(i);
      Serial.println(check_value);
    }
    //save_phone_number();
  }
}

void tone_sounds(int delay_time){ 
   tone(buzzer,frequency);
  delay(delay_time);
  noTone(buzzer);
   delay(delay_time);
   
  }
void tone_sounds_hash(int delay_time){ 
   tone(buzzer,1000);
  delay(delay_time);
  noTone(buzzer);
   delay(delay_time);
   
  }
  
void save_phone_number() {

  if (EEPROM.read(1)!= 1){
  Serial.println("You password has been saved.We will require your phone number incase you would ever want to reset your password");
  Serial.println("Please enter your phone number(Start with 07... and Press # to submit and * to delete):");
  byte local_address = PHONE_NUMBER_ADDRESS;
 // int phone_number_length=0;
  char phone_number = custom_keypad.getKey();
 
  int i=0;
  char confirm_flag=0;
  
  while (phone_number != '#'){
    phone_number = custom_keypad.getKey();
  if (phone_number) {
   
      phone_number=get_phone_number();
          
        }
        //}
        /* else{
            Serial.println("Your phone number is more than 10 digits please try again");
            phone_number=0;
           // phone_number_length=0;
            for(int i=0;i<10;i++)
            {
            phone_array[i]=0;
            }*/
            
            }
    
  


   
    
if(phone_number=='#'){
  Serial.print("Your phone number has been saved as:");
  for(int i=0;i<10;i++){
    Serial.print(phone_array[i]);
    
    }
  Serial.println(" .Press 1 to confirm and any other key to re-enter phone_number");
    char confirm=custom_keypad.getKey();
    while(confirm_flag == 0){
    confirm=custom_keypad.getKey();
    if(confirm){
      Serial.println(confirm);
      if(confirm=='1'){
        save_phone_number_eeprom(local_address);
        Serial.println("You have confirmed your phone number");
        confirm_flag=1;
}else{

  Serial.println("Please re-enter your phone number and press # to submit");
  phone_number=get_phone_number();
  
  }
      
      }
  
  }}
//update eeprom to show that you have set your phone number
EEPROM.update(1,1);
}
}
void save_phone_number_eeprom(byte local_address){
  
for(int i=0;i<10;i++){
       store_eeprom(phone_array[i], local_address);
    //Serial.print("eeprom");
    //Serial.println(local_address);
    //Serial.println(phone_array[i]);
     local_address = local_address + 1;
          if (local_address == EEPROM.length()) {
            local_address = PHONE_NUMBER_ADDRESS;
          }
        }       
  
  }
char get_phone_number(){
  int phone_number_length=0;
  char phone_number = custom_keypad.getKey();
  int i=0;
  char end_function='#';
  
  
   while (phone_number != '#'){
    
  if (phone_number) {
    if( phone_number != '#' &&phone_number != '*'){
     
         
         Serial.println(phone_number);
         phone_array[i]=phone_number;
         
         //Serial.print("Array");
            //Serial.println(phone_array[i]);
         phone_number_length++;
         //Serial.println(phone_number_length);
         i++; 
                  
 }else if(phone_number=='*'){
     if(i<=0){
      i=0;
      Serial.println("You have deleted all keys");
      }else if(i>0){
    Serial.print(phone_array[i-1]);
    Serial.println(" Deleted");
    phone_array[i-1]=0;
    --i;
    phone_number_length--;
   
      }
  
  }
  }
    phone_number = custom_keypad.getKey();
   }
   if(phone_number_length<10 ){
   
   Serial.println("Your phone number length is too short. Please start again");
   return phone_number_length; 
   }else if(phone_number_length==10){
    return end_function;
    }else if(phone_number_length>10){
   Serial.println("Your phone number length is too long. Please start again");
   return phone_number_length; }

}
void delete_number(byte local_address,byte constant_initial_address ) {
  if(local_address>=constant_initial_address){
  char value=EEPROM.read(local_address-1);
  Serial.print(value);
  EEPROM.update(local_address - 1, 0);
  }

  //address -= 1;

}

void password_validation(int password_length) {

  if (password_length == 5) {
    Serial.println("Password has been saved");
    EEPROM.update(0, password_length);
  }
  else {
    Serial.println("This password is not 4 digits long!Please try again.");
    for (int i = 0 ; i < EEPROM.length() ; i++) {
      EEPROM.update(i, 0);
    }
    create_new_password();
  }
}

void store_eeprom(char custom_key, int address) {

  int val = custom_key;

  /***
    Update the particular EEPROM cell.
    these values will remain there when the board is
    turned off.
  ***/
  EEPROM.update(address, val);



  // Serial.println(address);
  //delay(100);
}

int eeprom_read() {
  int value = (int)EEPROM.read(0);
  delay(500);
  return value;
}

void confirm_password() {

  char  customKey = 0;
  int check_value = 0;
  int i = INITIAL_ADDRESS;
  static int confirm_flag = 0;
  static int failed_attempts = 0;
  char key_pressed[10];
  int counter = 0;
  byte failed_flag_address = 5;
  int flag = 0;
  byte flag_value = EEPROM.read(failed_flag_address);
  if (flag_value == 0) {
    while (customKey != '#') {
      customKey = custom_keypad.getKey();



      if (customKey) {
        //Serial.print("Address at start");
        //Serial.println(i);
        if (customKey != '*') {

          key_pressed[counter] = customKey;
          Serial.println(key_pressed[counter]);
          //Serial.println(customKey);
          check_value = EEPROM.read(i);
          //Serial.println(check_value);

          if (check_value == customKey) {
            i++;
            counter++;
            // Serial.print("true i:");
            //Serial.println(i);
            flag++;


          } else {
            i++;
            counter++;

          }
        } else if (customKey == '*') {
          if (counter < 0) {
            counter = 0;
          }
          if (counter == 0) {

            if (i == INITIAL_ADDRESS) {

              Serial.println("You have erased all digits");
            }
            //Serial.print(key_pressed[counter]);
            //Serial.println(" deleted!");
            if (i > INITIAL_ADDRESS) {
              i--;
            }
          }
          if (counter > 0) {
            counter--;
            Serial.print(key_pressed[counter]);
            Serial.println(" deleted!");
            //if (counter==0){counter--;}
            //counter--;
            //Serial.print("Address before");
            //Serial.println(i);
            if (i > 10) {
              i--;
            }
          }

        }

      }
    }
    //Serial.print("password length");
    //Serial.println(EEPROM.read(0));
    if (flag == EEPROM.read(0)) {
      Serial.println("Passwords match!Access granted");
      failed_attempts = 0;
      EEPROM.update(failed_flag_address, confirm_flag);
    }
    else {
      Serial.println("Access denied!Wrong password");
      failed_attempts++;


    }
    if (failed_attempts == 3) {

      confirm_flag = 1;
      EEPROM.update(failed_flag_address, confirm_flag);
      retrieve_password();
    }

  } else {
    retrieve_password();
  }

}

void retrieve_password() {
  byte retrieve_flag = 0;
  byte first_six_digits=6;
  byte phone_number_address = PHONE_NUMBER_ADDRESS+first_six_digits;
  if (retrieve_flag == 0) {
    Serial.println("Do you want to reset your password?(1 for Yes)press # to repeat instruction");


  }
  char response = custom_keypad.getKey();
  while (response != '#') {
    response = custom_keypad.getKey();
    if (response) {
     
      if (response == '1') {
          tone_sounds(100);
        Serial.println(response);
        Serial.println("Please enter the last 4 digits of your phone number(Press # to submit):");
        char phone_number = custom_keypad.getKey();
        
        while (phone_number != '#' && phone_number_address<=(PHONE_NUMBER_ADDRESS+10)) {
          phone_number = custom_keypad.getKey();
          if (phone_number) {
            tone_sounds(100);
            Serial.println(phone_number);
            char value = EEPROM.read(phone_number_address);
            if (value == phone_number) {
              Serial.println("Nice!!");
            }
  phone_number_address++;
          }

        }
      } else if (response != '#') {
         tone_sounds(100);
        Serial.println(response);
        Serial.println("Not a valid response!Please try again!");
        //Serial.println("Do you want to reset your password?(1 for Yes)press # to submit");
      }
     
    }
  }
  if(response=='#'){
        tone_sounds_hash(100);
        tone_sounds_hash(100);
        
        }

}



void double_press(char key_gotten) {



  //char key_gotten=custom_keypad.getKey();
  if (key_gotten) {
    Serial.println(key_gotten);
    if (track_press_counter < 4 && track_press_counter >= 0) {
      track_press[track_press_counter] = key_gotten;
    }
    unsigned long current_time = millis();
    //Serial.println(keys);
    if (key_gotten = '*' && track_press_counter > 0) {
      if (  track_press[track_press_counter] == '*'  && track_press[track_press_counter - 1] == '*') {
        if ((current_time - previous_time) < long_press_interval) {
          counter++;
        }
        previous_time = current_time;

      }
    }
    else if (track_press_counter == 0 && key_gotten == '*'  ) {
      if ((current_time - previous_time) < long_press_interval) {
        counter++;
      }
      previous_time = current_time;
    }
    if (counter == 1) {
      double_press_state = true;
      Serial.println("Yesss!");
      counter = 0;

    }
    Serial.println(counter);
    track_press_counter++;
    // Serial.println(track_press_counter);
    if (track_press_counter == 3) {
      track_press_counter = 0;
    }

  }
}
