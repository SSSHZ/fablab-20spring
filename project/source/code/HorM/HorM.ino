#include <LiquidCrystal_I2C.h>

#define LEFT 1 
#define RIGHT 0 
#define UP 1
#define DOWN 0 
#define STAY -1

#define Xlimit 700 // 900pulse == 17.3cm
#define Ylimit 700

#define stepper_rate 800

#define X_dir_pin 12 
#define X_step_pin 11
#define X_enable_pin 13
#define Y_dir_pin 8 
#define Y_step_pin 9
#define Y_enable_pin 10

#define left_pin 2
#define right_pin 3
#define up_pin 4
#define down_pin 5

// scenes
#define END 3

int goodman = 1;
int killed = 0;

// set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27,16,2);  

// necessary flags for buttons
int new_left = LOW;
int old_left = new_left;
int new_right = LOW;
int old_right = new_right;
int new_up = LOW;
int old_up = new_up;
int new_down = LOW;
int old_down = new_down;

// let's start from LEFT-DOWN corner
int Xdir = STAY;
int Ydir = STAY;
int Xpos = 0;
int Ypos = 0;

// scenes
int scene = 0;
// prologue
int prologue_cnt = 0;

// fighting scenes
int MONSTER = 0;
int fight()
{
  int M1X1 = 400;
  int M1X2 = 600;
  int M1Y1 = 0;
  int M1Y2 = 300;

  if ((Xpos > M1X1) && (Xpos < M1X2) && (Ypos > M1Y1) && (Ypos < M1Y2) && killed==0)
    return 1;
  if ((Xpos > 600) &&  (Ypos > 600))
    return -1;
  return 0;
}


void setup()
{
  pinMode(X_dir_pin,OUTPUT); 
  pinMode(X_step_pin,OUTPUT); 
  pinMode(X_enable_pin,OUTPUT);
  pinMode(Y_dir_pin,OUTPUT); 
  pinMode(Y_step_pin,OUTPUT); 
  pinMode(Y_enable_pin,OUTPUT);

  pinMode(left_pin, INPUT);
  pinMode(right_pin, INPUT);
  pinMode(up_pin, INPUT);
  pinMode(down_pin, INPUT);
  // close the motor at the start
  digitalWrite(X_enable_pin,HIGH);
  digitalWrite(Y_enable_pin,HIGH);
  // initialize the lcd 
  lcd.init(); 
  delay(200);
  lcd.backlight();
  lcd.clear();
}
 
// enable = LOW，电机可转动
// 200高低脉冲是一圈
// 高低脉冲频率决定了转动速度，即每个脉冲持续时间越短，转动越快

void move(int Xdir, int Ydir)
{
  // open if move
  if (Xdir == STAY || (Xpos<50 && Xdir==LEFT) || (Xpos>Xlimit-50 && Xdir==RIGHT))
    digitalWrite(X_enable_pin, HIGH);
  else
  {
    digitalWrite(X_enable_pin, LOW);
    if (Xdir == LEFT) Xpos -= 25;
    else if (Xdir == RIGHT) Xpos += 25;
  }
    
  if (Ydir == STAY || (Ypos<50 && Ydir==DOWN) || (Ypos>Ylimit-50 && Ydir==UP))
    digitalWrite(Y_enable_pin, HIGH);
  else
  {
    digitalWrite(Y_enable_pin, LOW);
    if (Ydir == DOWN) Ypos -= 25;
    else if (Ydir == UP) Ypos += 25;
  }
      
  // select direction
  if(Xdir == LEFT)
    digitalWrite(X_dir_pin,HIGH); //***
  else
    digitalWrite(X_dir_pin,LOW); //***
  if(Ydir == UP)
    digitalWrite(Y_dir_pin,LOW); //***
  else
    digitalWrite(Y_dir_pin,HIGH); //***
    
  // 1/8 round
  for(int i = 0; i < 25; i++) 
  {
    digitalWrite(X_step_pin,HIGH);
    digitalWrite(Y_step_pin,HIGH); 
    delayMicroseconds(800); 
    digitalWrite(X_step_pin,LOW); 
    digitalWrite(Y_step_pin,LOW); 
    delayMicroseconds(800); 
  }
  
  // close
  digitalWrite(X_enable_pin,HIGH);
  digitalWrite(Y_enable_pin,HIGH);
}

void loop()
{
  // necessary flags
  old_left = new_left;
  new_left = digitalRead(left_pin);
  old_right = new_right;
  new_right = digitalRead(right_pin);
  old_up = new_up;
  new_up = digitalRead(up_pin);
  old_down = new_down;
  new_down = digitalRead(down_pin);
  
  // welcome
  if (scene == 0)
  {
    if ((new_right == HIGH)&&(old_right == LOW))
    {
      lcd.clear();
      prologue_cnt++; 
    }
    if (prologue_cnt == 0)
    {
      lcd.setCursor(0,0);
      lcd.print("Welcome to the");
      lcd.setCursor(2,1);
      lcd.print("UNDERGROUND!");
      lcd.setCursor(15,1);
      lcd.print(">");
    }
    else if(prologue_cnt == 1)
    {
      lcd.setCursor(0,0);
      lcd.print("Run to exit");
      lcd.setCursor(0,1);
      lcd.print("or DIE here!");
      lcd.setCursor(15,1);
      lcd.print(">");
    }
    else if(prologue_cnt == 2)
    {
      lcd.setCursor(0,0);
      lcd.print("Run! Hero!");
      lcd.setCursor(4,1);
      lcd.print("Run!");
      lcd.setCursor(15,1);
      lcd.print(">");
    }
    else
      scene++;
  }
  else if (scene == 1)
  {
    if ((new_left == HIGH)&&(old_left == LOW))
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Moving leftward");
    }
    else if ((new_down == HIGH)&&(old_down == LOW))
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Moving downward");
    }
    else if ((new_right == HIGH)&&(old_right == LOW))
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Moving rightward");
    }
    else if ((new_up == HIGH)&&(old_up == LOW))
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Moving upward");
    }
    
    
    Xdir = Ydir = STAY;
    if (digitalRead(left_pin) == HIGH && digitalRead(right_pin) == LOW)
      Xdir = LEFT;
    else if (digitalRead(left_pin) == LOW && digitalRead(right_pin) == HIGH)
      Xdir = RIGHT;
    if (digitalRead(up_pin) == HIGH && digitalRead(down_pin) == LOW)
      Ydir = UP;
    else if (digitalRead(up_pin) == LOW && digitalRead(down_pin) == HIGH)
      Ydir = DOWN;
    move(Xdir, Ydir);

    // encounter monsters
    MONSTER = fight();
    if (MONSTER > 0)
    {
      scene=2;
      lcd.clear();
    }
      
    else if(MONSTER == -1)
    {
      scene = 3;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("You win!");
      lcd.setCursor(0,1);
      if (goodman == 0)
        lcd.print("You, MONSTER!");
      else
        lcd.print("You, the hero!");
    }
      
  }
  else if (scene == 2)
  {
    lcd.setCursor(0,0);
    lcd.print("A MUSHROOM!");
    lcd.setCursor(0,1);
    lcd.print("< Kill >Mercy");
    if ((new_left == HIGH)&&(old_left == LOW))
    {
      goodman = 0;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Killing!");
      lcd.setCursor(0,1);
      lcd.print("Run again!");
      scene = 1;
    }  
    else if ((new_right == HIGH)&&(old_right == LOW))
    {
      goodman = 1;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Mercy!");
      lcd.setCursor(0,1);
      lcd.print("Run again!");
      scene = 1;
    }
    killed = 1;
  }

}
