/************************************************************************************************************************************
*     @file:       schlossinator.ino
*     @author:     Berk Can Ulubas
*     @date:       18.06.2026
*     @brief:      This programm controlls an servo motor for an doorlock. 
*                 The 4x4 membrane keypad is used to switch modes and transfer the password. 
*
*************************************************************************************************************************************
*/
#include <Keypad.h>
#include <Servo.h>

#define ROWS 4
#define COLS 4
#define RED_LED A0
#define GREEN_LED A1
#define BLUE_LED A2
#define YELLOW_LED A3
#define INSIDE_BUTTON 12

enum LockState {
  UNLOCK,
  LOCK
};

enum InputMode {
  UNUSED,
  RESET_PASSWORD,
  CHECK_PASSWORD
};

enum Action {
  NONE,
  TRY_UNLOCK,
  TRY_LOCK,
  TRY_RESET_PASSWORD,
  RESETTING_NOW
};

const char* lock_state_names[] = { "UNLOCK", "LOCK" };
const char* input_mode_names[] = { "UNUSED", "RESET_PASSWORD", "CHECK_PASSWORD" };
const char* action_names[] = { "NONE", "TRY_UNLOCK", "TRY_LOCK", "TRY_RESET_PASSWORD", "RESETTING_NOW" };

LockState lock_state;
InputMode input_mode;
Action action;
Servo servo;
char password[10];
int last_button = LOW;
char input[10];
int input_index = 0;
int password_index = 0;
bool check_failed = false;
unsigned long last_press_time = 0;
const unsigned long debounce_time = 500;

char keys[ROWS][COLS] = {
  { 'D', 'C', 'B', 'A' },
  { '#', '9', '6', '3' },
  { '0', '8', '5', '2' },
  { '*', '7', '4', '1' }
};

byte rowPins[ROWS] = { 2, 3, 4, 5 };
byte colPins[COLS] = { 6, 7, 8, 9 };

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  servo.attach(10);
  pinMode(INSIDE_BUTTON, INPUT_PULLUP);

  calibrate();
  lock_state = UNLOCK;
  input_mode = RESET_PASSWORD;
  action = RESETTING_NOW;
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(RED_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(BLUE_LED, HIGH);
  input[0] = '\0';
  password[0] = '\0';
  Serial.begin(9600);
  Serial.println("System gestartet");
}


/***************************************************************************************************************************************************************
*     brief:      super loop in which the next state is determined
*
****************************************************************************************************************************************************************
*/
void loop() {
  // 1. read input
  char key = keypad.getKey();
  bool inside_toggle = read_inside_button();


  //2.1 check if key is valid
  if (key != NO_KEY) {
    Serial.print("Key gedrückt: ");
    Serial.println(key);

    if (key == 'A' || key == 'B' || key == 'C') {  //2.2 if key is action change Action
      switch (key) {
        case 'A':
          action = TRY_UNLOCK;
          input_mode = CHECK_PASSWORD;
          break;
        case 'B':
          action = TRY_LOCK;
          input_mode = CHECK_PASSWORD;
          break;
        case 'C':
          action = TRY_RESET_PASSWORD;
          input_mode = CHECK_PASSWORD;
      }
    } else if (key >= '0' && key <= '9') {  // 3. if key is number change input_mode
      switch (input_mode) {
        case UNUSED:
          break;
        case CHECK_PASSWORD:
          input[input_index] = key;
          input_index++;
          if (input_index >= 9) {
            input_mode = UNUSED;
            key = 'D';
          }
          break;
        case RESET_PASSWORD:
          password[password_index] = key;
          password_index++;
          if (password_index >= 9) {
            password[9] = '\0';
            action = NONE;
            input_mode = UNUSED;
          }
      }
    } else if (key == 'D') {  // 4.1 if key is D change state
      input[input_index] = '\0';
      password[password_index] = '\0';
      Serial.print("Input: ");
      Serial.println(input);
      Serial.print("password: ");
      Serial.println(password);
      switch (action) {
        case NONE:
          break;
        case TRY_UNLOCK:
          if (compare_char_arrays(input, password)) {
            lock_state = UNLOCK;
            action = NONE;
            input_mode = UNUSED;
          } else {
            input_mode = UNUSED;
            check_failed = true;
            action = NONE;
          }
          break;
        case TRY_LOCK:
          if (compare_char_arrays(input, password)) {
            lock_state = LOCK;
            action = NONE;
            input_mode = UNUSED;
          } else {
            input_mode = UNUSED;
            check_failed = true;
            action = NONE;
          }
          break;
        case TRY_RESET_PASSWORD:
          if (compare_char_arrays(input, password)) {
            action = RESETTING_NOW;
            input_mode = RESET_PASSWORD;
            password_index = 0;
            password[0] = '\0';
          } else {
            input_mode = UNUSED;
            check_failed = true;
            action = NONE;
          }
          break;
        case RESETTING_NOW:
          action = NONE;
          input_mode = UNUSED;
      }

      //reset input
      input_index = 0;
      input[0] = '\0';
    }
  }



  // 4.2 change state based on inside_button
  if (inside_toggle && action != RESETTING_NOW) {
    if (lock_state == UNLOCK) {
      lock_state = LOCK;
    } else {
      lock_state = UNLOCK;
    }
  }

  // 5. output based on state and input_mode (LED, Servo)
  if (key != NO_KEY || inside_toggle) {
    Serial.print("Action: ");
    Serial.println(action_names[action]);

    Serial.print("InputMode: ");
    Serial.println(input_mode_names[input_mode]);

    Serial.print("LockState: ");
    Serial.println(lock_state_names[lock_state]);

    switch (lock_state) {
      case UNLOCK:
        digitalWrite(GREEN_LED, HIGH);
        digitalWrite(RED_LED, LOW);
        servo.write(70);
        break;
      case LOCK:
        digitalWrite(RED_LED, HIGH);
        digitalWrite(GREEN_LED, LOW);
        servo.write(140);
    }

    switch (input_mode) {
      case UNUSED:
        digitalWrite(YELLOW_LED, LOW);
        digitalWrite(BLUE_LED, LOW);
        break;
      case RESET_PASSWORD:
        digitalWrite(BLUE_LED, HIGH);
        digitalWrite(YELLOW_LED, LOW);
        break;
      case CHECK_PASSWORD:
        digitalWrite(YELLOW_LED, HIGH);
        digitalWrite(BLUE_LED, LOW);
    }
    if (check_failed) {
      for (int i = 0; i < 6; i++) {
        digitalWrite(YELLOW_LED, HIGH);
        delay(200);
        digitalWrite(YELLOW_LED, LOW);
        delay(200);
      }
      check_failed = false;
    }
  }
}

/***************************************************************************************************************************************************************
*     @brief:       compares to char arrays (Strings) they must containt null-terminator at the end
*     @param:       char array1[] the one array that should be compared with array2
*     @param:       char array2[] the other array that should be compared with array1
*     @return:      true when both arrays have the same chars until the null-terminator
*
****************************************************************************************************************************************************************
*/
bool compare_char_arrays(char* string1, char* string2) {
  int i = 0;
  while (string1[i] != '\0' && string2[i] != '\0') {
    if (string1[i] != string2[i]) {
      return false;
    }
    i++;
  }
  return string1[i] == '\0' && string2[i] == '\0';
}

/***************************************************************************************************************************************************************
*     @brief:      positions the motor so that the cap would point downwards. 
*
****************************************************************************************************************************************************************
*/
void calibrate() {
  servo.write(100);
}

/***************************************************************************************************************************************************************
*     @brief:      shows if the inside botton got pressed
*
****************************************************************************************************************************************************************
*/
bool read_inside_button() {
  int current_button = digitalRead(INSIDE_BUTTON);
  bool toggle = false;
  if (last_button == HIGH && current_button == LOW) {
    if (millis() - last_press_time  > debounce_time) {
      toggle = true;
      last_press_time = millis();
    }
  }
  if (toggle) {
    Serial.println("toggle");
  }
  last_button = current_button;
  return toggle;
}
