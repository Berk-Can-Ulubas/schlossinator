/************************************************************************************************************************************
*     @file:       schlossinator.ino
*     @author:     Berk Can Ulubas
*     @date:       19.07.2025
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

Servo servo;
char password[10];
bool locked = false;

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() 
{
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  servo.attach(11); 

  calibrate();
  reset_password();
  unlock();
}

/***************************************************************************************************************************************************************
*     brief:      super loop in which the next state is determined
*
****************************************************************************************************************************************************************
*/
void loop() 
{
  char key = keypad.getKey();
  
  if (key) 
  {
    switch (key)
    {
      case 'A': 
        if (check_password())
        {
          unlock();
        }
        else
        {
          password_is_wrong();
        }
        break;
      case 'B': 
        if (check_password())
        {
          lock();
        }
        else
        {
          password_is_wrong();
        }
        break;
      case 'C': 
        if (check_password())
        {
          reset_password();
        }
        else
        {
          password_is_wrong();
        }
        break;
    }
  }
}

/***************************************************************************************************************************************************************
*     brief:      turns on red led and moves the servo motor so the door is locked
*
****************************************************************************************************************************************************************
*/
void lock()
{
  digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, LOW);
  locked = true;
  servo.write(140);
}

/***************************************************************************************************************************************************************
*     brief:      turns on green led and moves the servo motor so the door is unlocked
*
****************************************************************************************************************************************************************
*/
void unlock()
{
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(RED_LED, LOW);
  locked = false;
  servo.write(70);
}

/***************************************************************************************************************************************************************
*     @brief:      reads input (only numbers) asnd overwrites the current password, while turning on the blue led
*
****************************************************************************************************************************************************************
*/
void reset_password()
{
  digitalWrite(BLUE_LED, HIGH);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);
  char key = keypad.getKey();
  int password_length = 0;
  while(password_length <=9 && key != 'D')
  {
    key = keypad.getKey();
    if (key == 'D')
    {
      break;
    }
    if (key >= '0' &&  key<= '9')
    {
      password[password_length] = key;
      password_length++;
    }
  }
  password[password_length] = '\0';
  digitalWrite(BLUE_LED, LOW);
  if(locked)
  {
    digitalWrite(RED_LED, HIGH);
  }
  else
  {
    digitalWrite(GREEN_LED, HIGH);
  }
}

/***************************************************************************************************************************************************************
*     @brief:       reads input (only numbers) and after pressing D or typing 9 numbers it compers the input with the current password
*     @return:      true when input equals password
*
****************************************************************************************************************************************************************
*/
bool  check_password()
{
  digitalWrite(YELLOW_LED, HIGH);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);
  char key = keypad.getKey();
  int input_length = 0;
  char input[10];
  while(input_length <=9 && key != 'D')
  {
    key = keypad.getKey();
    if (key >= '0' &&  key<= '9')
    {
      input[input_length] = key;
      input_length++;
    }
  }
  digitalWrite(YELLOW_LED, LOW);
  if(locked)
  {
    digitalWrite(RED_LED, HIGH);
  }
  else
  {
    digitalWrite(GREEN_LED, HIGH);
  }
  return compare_char_arrays(input, password);
}

/***************************************************************************************************************************************************************
*     @brief:       compares to char arrays (Strings) they must containt null-terminator at the end
*     @param:       char array1[] the one array that should be compared with array2
*     @param:       char array2[] the other array that should be compared with array1
*     @return:      true when both arrays have the same chars until the null-terminator
*
****************************************************************************************************************************************************************
*/
bool compare_char_arrays(char array1[], char array2[])
{
  int i = 0;
  while (array1[i] != '\0' && array2[i] != '\0')
  {
    if(array1[i] != array2[i])
    {
      return false;
    }
    i++;
  }
  return true;
}

/***************************************************************************************************************************************************************
*     @brief:      this fnction lets the yellow led blink beacause the input was wrong
*
****************************************************************************************************************************************************************
*/
void password_is_wrong()
{
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);
  for (int i = 0; i < 6; i++)
  {
    digitalWrite(YELLOW_LED, HIGH);
    delay(200);
    digitalWrite(YELLOW_LED, LOW);
    delay(200);
  } 
  if(locked)
  {
    digitalWrite(RED_LED, HIGH);
  }
  else
  {
    digitalWrite(GREEN_LED, HIGH);
  }
}

/***************************************************************************************************************************************************************
*     @brief:      positions the motor so that the cap would point downwards. 
*
****************************************************************************************************************************************************************
*/
void calibrate()
{
  servo.write(100);   
}
