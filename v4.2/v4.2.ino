//Подключение библеотек
#include <Adafruit_ST7735.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
#include <SPI.h>
#include "EEPROM.h"
#include "DHT.h"


//Объявление выводы дисплея
#define TFT_CS     10
#define TFT_RST    9                      
#define TFT_DC     8
#define TFT_SCLK 13   
#define TFT_MOSI 11   

//Создания объекта дисплея
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

//Объявление переменных для температуры
float maxTemperature=0;
float minTemperature=200;
char charMinTemperature[10];
char charMaxTemperature[10];
char temperatureChar[10];
float temperature = 0;
float previousTemperature = 0;

//Объявление переменных для времени
char timeChar[100];
char dateChar[50];
String dateString;
int minuteNow=0;
int minutePrevious=0;
unsigned long last_time;
#define DS3231_I2C_ADDRESS 104
byte tMSB, tLSB;
byte hh, mm, ss;
byte yy, ll, dd, zz;

//Объявление вывод кнопок и пищалки
#define meniu A0 
#define minus A1
#define plus A2
#define alarm A3
#define buzzer 4

//Переменная режимов
int nivel = 0;   

int hh1, mm1, zz1, dd1, ll1, yy1;   
int maxday;
boolean initial = 1;
char chartemp[3];
int hha, mma;   
byte al;     
byte xa = 80;
byte ya = 70;

//Температура + Влажность
#define DHTPIN 6     
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);
int umiditate;
int tzeci, tunit, tzecimi, trest;
int tsemn, ttot;
int hzeci, hunit;
int previousHumidity = 0;
char umiditateChar[10];
unsigned long masurare;
unsigned long intervalmasurare = 30000;

//Установочный блок
void setup () 
{
//Инициализация термометра и дисплея
dht.begin(); 
tft.initR(INITR_BLACKTAB);
tft.fillScreen(ST7735_BLACK);

//Для Com порта
Serial.begin(9600);
Wire.begin();

//Установка выводы на In или Out
pinMode(meniu, INPUT); 
pinMode(plus, INPUT); 
pinMode(minus, INPUT); 
pinMode(alarm, INPUT); 
pinMode(buzzer, OUTPUT);
digitalWrite(meniu, HIGH);  
digitalWrite(plus, HIGH);
digitalWrite(minus, HIGH);
digitalWrite(alarm, HIGH);
digitalWrite(buzzer, LOW);

  //Предварительная печать на экране температуры и влажности, поскольку в loop они печатаются только при изменении значения или инициализации
    printText(utf8rus("ТЕМПЕРАТУРА"), ST7735_GREEN,5,90,1);  
    printText(utf8rus("ВЛАЖНОСТЬ"), ST7735_RED,5,110,1);

//Чтение параметров будильника из памяти (если значения выходят из границ допустимых значений, устанавливаются с выключенным статусом на 7.00)
hha = EEPROM.read(100);
mma = EEPROM.read(101);
al = EEPROM.read(102);
if ((hha < 0) || (hha > 23)) hha = 7;
if ((mma < 0) || (mma > 59)) mma = 0;
if ((al < 0) || (al > 1)) al = 0;

masurare = millis();
}

void loop () //Основной цикл
{
  if (nivel !=0)//Вывод в ком порт изменяемых значений при настройки
  {
Serial.print( "hour " );
Serial.println(hh1);
Serial.print( "min " );
Serial.println(mm1);
Serial.print( "day week " );
Serial.println(zz1);
Serial.print( "day " );
Serial.println(dd1);
Serial.print( "month " );
Serial.println(ll1);
Serial.print("ear ");
Serial.println(yy1);
Serial.println("");
}

  //Мигающая точка между часами и минутами                                                                        
if (nivel == 0) //Основной режим
{          
    if(round(millis()/1000  %2)){
    printText(":", ST7735_YELLOW,58,25,3);
    }
    else{
    printText(":", ST7735_BLACK,58,25,3);
    }  
    

if (((millis() - masurare) > intervalmasurare) || initial)
{
temperature = dht.readTemperature();
umiditate = dht.readHumidity();
delay(150);
masurare = millis();
} 
  readDS3231time(&ss, &mm, &hh, &zz, &dd, &ll,&yy);

    String halarma = "";
    if(hha<10)
    {
         halarma = halarma+" "+String(hha);
    }else  
    halarma = halarma+ String(hha);
    if(mma<10)
    {
        halarma = halarma+":0"+String(mma);
    }else
    {
        halarma = halarma+":"+String(mma);
    }
    halarma.toCharArray(timeChar,100);
                                                                                  
   //Будильник                                                                 
if (mma == mm & hha == hh & al%2)
{
  if(round(millis()/500  %2)){
    digitalWrite(buzzer, HIGH);
    }
    else{
    digitalWrite(buzzer, LOW);
    }  

   tft.drawCircle(xa, ya, 5, ST7735_RED); 
   tft.drawLine(xa, ya, xa, ya-5, ST7735_RED);
   tft.drawLine(xa, ya, xa+4, ya+4, ST7735_RED); 
   printText(timeChar, ST7735_RED,xa+7,ya,1);
}
else
{
if (al%2 == 0)
{
    tft.drawCircle(xa, ya, 5, ST7735_BLUE); 
    tft.drawLine(xa, ya, xa, ya-5, ST7735_BLUE);
    tft.drawLine(xa, ya, xa+4, ya+4, ST7735_BLUE); 
    digitalWrite(buzzer, LOW);
    printText(timeChar, ST7735_BLUE,xa+7,ya,1);
}
if (al%2 == 1)
{
    tft.drawCircle(xa, ya, 5, ST7735_WHITE); 
    tft.drawLine(xa, ya, xa, ya-5, ST7735_WHITE);
    tft.drawLine(xa, ya, xa+4, ya+4, ST7735_WHITE); 
  digitalWrite(buzzer, LOW);
    printText(timeChar, ST7735_WHITE,xa+7,ya,1);
}
}
 
  minuteNow = mm;
  if(minuteNow!=minutePrevious || initial) // Следующая минута
  {
    initial = 0;
    dateString = getDayOfWeek(zz)+", ";
    dateString = dateString+String(dd)+"/"+String(ll);
    dateString= dateString+"/"+ String(yy); 
    minutePrevious = minuteNow;
    String hours = "";
    if(hh<10)
    {
         hours = hours+" "+String(hh);
    }else  
    hours = hours+ String(hh);
    if(mm<10)
    {
        hours = hours+" 0"+String(mm);
    }else
    {
        hours = hours+" "+String(mm);
    }
    
    hours.toCharArray(timeChar,100);
    tft.fillRect(0,0,160,65,ST7735_BLACK);
  //  printText(timeChar, ST7735_WHITE,20,25,3);
    printText(timeChar, ST7735_YELLOW,5,25,4);
    dateString.toCharArray(dateChar,50);
    printText(dateChar, ST7735_GREEN,8,5,1);
  }
  
  if ((temperature != previousTemperature) || initial) //Смена температуры
  {
    previousTemperature = temperature;
    String temperatureString = String(temperature,1);
    temperatureString.toCharArray(temperatureChar,10);
    tft.fillRect(70,85,128,20,ST7735_BLACK);
    printText(temperatureChar, ST7735_WHITE,80,90,1);
    printText("o", ST7735_WHITE,107,87,1);
    printText("C", ST7735_WHITE,113,90,1);

  if((umiditate != previousHumidity) || initial) //Смена влажности
  {
    previousHumidity = umiditate;
    String umiditateString = String(umiditate);
    umiditateString.toCharArray(umiditateChar,10);
    tft.fillRect(64,110,128,27,ST7735_BLACK);
    printText(umiditateChar, ST7735_WHITE,80,110,1);
    printText("%RH", ST7735_WHITE,94,110,1);
  }
  }
  
  if (digitalRead(meniu) == LOW) //Нажатие кнопки SEt
  {
  nivel = nivel+1;
  hh1=hh;
  mm1=mm;
  zz1=zz;
  yy1=yy;
  ll1=ll;
  dd1=dd;
  tft.fillScreen(ST7735_BLACK);
  tft.fillRect(0,0,128,160,ST7735_BLACK);
  delay(150);
  }
  if (digitalRead(alarm) == LOW||(mma == mm & hha == hh & al%2))
   {
  if ((digitalRead(alarm) == LOW) && (!(mma == mm & hha == hh & al%2)))
  {
  nivel = nivel - 1 ;
  tft.fillScreen(ST7735_BLACK);
  tft.fillRect(0,0,128,160,ST7735_BLACK);
  delay(150);
  } 
  if ((digitalRead(alarm) == LOW) && (mma == mm & hha == hh & al%2))
  {
    al = al + 1;
    EEPROM.write(102, al%2);
    delay(500);
  }
  }    
  // end usual case (clock)
}

if (nivel == 1)   // Настройка часа
{
 checkMeniu();

printText(utf8rus("НАСТРОЙКА"), ST7735_GREEN,10,18,2);  
printText(utf8rus("ЧАСОВ:"), ST7735_GREEN,10,36,2);  
dtostrf(hh1,3, 0, chartemp); 
      //tft.fillRect(50,50,70,18,ST7735_BLACK);
      tft.fillRect(50,50,70,38,ST7735_BLACK);
      delay(50);
      printText(chartemp, ST7735_WHITE,60,60,2);
      delay(50);
      
hh1 = plusOrMinusFunc(hh1);

if (hh1 > 23) hh1 = 0;
if (hh1 < 0) hh1 = 23;       

     isAlarmButton();
}  

if (nivel == 2)   // Настройка минут
{
checkMeniu(); 

printText(utf8rus("НАСТРОЙКА"), ST7735_GREEN,10,18,2);  
tft.fillRect(10,35,70,18,ST7735_BLACK);
printText(utf8rus("МИНУТ:"), ST7735_GREEN,10,35,2); 

dtostrf(mm1,3, 0, chartemp); 
      tft.fillRect(50,50,70,30,ST7735_BLACK);
      delay(50);
      printText(chartemp, ST7735_WHITE,60,60,2);
      delay(50);
     
mm1 = plusOrMinusFunc(mm1);

if (mm1 > 59) mm1 = 0;
if (mm1 < 0) mm1 = 59;       

     isAlarmButton();
}  

if (nivel == 3)   // Настройка дня недели
{
checkMeniu();  

printText(utf8rus("НАСТРОЙКА"), ST7735_GREEN,10,18,2); 
tft.fillRect(0,35,128,25,ST7735_BLACK);
printText(utf8rus("Дня недели:"), ST7735_GREEN,10,45,1);  
//Serial.println(mm1);
dtostrf(zz1,3, 0, chartemp); 
      tft.fillRect(0,55,128,20,ST7735_BLACK);
      delay(50);
      printText(chartemp, ST7735_WHITE,60,60,2);
tft.fillRect(10,80,100,28,ST7735_BLACK);      
if (zz1 == 0) printText(utf8rus("0 - Понедельник"), ST7735_RED,10,80,1);  
if (zz1 == 1) printText(utf8rus("1 - Вторник"), ST7735_RED,10,80,1);  
if (zz1 == 2) printText(utf8rus("2 - Среда"), ST7735_RED,10,80,1);  // 
if (zz1 == 3) printText(utf8rus("3 - Четверг"), ST7735_RED,10,80,1);  // 
if (zz1 == 4) printText(utf8rus("4 - Пятница"), ST7735_RED,10,80,1);  // 
if (zz1 == 5) printText(utf8rus("5 - Суббота"), ST7735_RED,10,80,1);  // 
if (zz1 == 6) printText(utf8rus("6 - Воскресенье"), ST7735_RED,10,80,1);  //   
    delay(50);
     
zz1 = plusOrMinusFunc(zz1);

if (zz1 > 6) zz1 = 0;
if (zz1 < 0) zz1 = 6;       

     isAlarmButton();
}  

if (nivel == 4)   // Настройка года
{
checkMeniu();  

printText(utf8rus("НАСТРОЙКА"), ST7735_GREEN,10,18,2);  // 
tft.fillRect(10,35,70,18,ST7735_BLACK);
printText(utf8rus("ГОДА:"), ST7735_GREEN,10,35,2);  // 
dtostrf(yy1,3, 0, chartemp); 
      tft.fillRect(40,50,80,30,ST7735_BLACK);
      tft.fillRect(10,80,100,30,ST7735_BLACK);        delay(50);
      printText("20", ST7735_WHITE,45,60,2);
      printText(chartemp, ST7735_WHITE,60,60,2);
      delay(50);

     
yy1 = plusOrMinusFunc(yy1);

    
if (yy1 > 49) yy1 = 49;
if (yy1 < 16) yy1 = 16;       
     isAlarmButton();
}  

if (nivel == 5)   // Настройка месяца
{


printText(utf8rus("НАСТРОЙКА"), ST7735_GREEN,10,18,2);  
tft.fillRect(10,35,70,18,ST7735_BLACK);
printText(utf8rus("МЕСЯЦА:"), ST7735_GREEN,10,35,2);  
dtostrf(ll1,3, 0, chartemp); 
      tft.fillRect(40,50,80,30,ST7735_BLACK);
      tft.fillRect(10,80,100,30,ST7735_BLACK);  
      delay(50);
      printText(chartemp, ST7735_WHITE,60,60,2);
      delay(50);
     
ll1 = plusOrMinusFunc(ll1);
  
   checkMeniu(); 
if (ll1 > 12) ll1 = 1;
if (ll1 < 1) ll1 = 12;       
     isAlarmButton();
}  

if (nivel == 6)   // Настройка дня
{
checkMeniu();

printText(utf8rus("НАСТРОЙКА"), ST7735_GREEN,10,18,2);  
tft.fillRect(10,35,70,18,ST7735_BLACK);
printText(utf8rus("ДНЯ:"), ST7735_GREEN,10,35,2);  
dtostrf(dd1,3, 0, chartemp); 
      tft.fillRect(50,50,70,18,ST7735_BLACK);
      tft.fillRect(10,80,100,18,ST7735_BLACK);  
      delay(50);
      printText(chartemp, ST7735_WHITE,60,50,2);
      delay(50);
 
    dd1 = plusOrMinusFunc(dd1);
   
    
 if (ll == 4 || ll == 5 || ll == 9 || ll == 11) { //30-ти дневные месяцы
    maxday = 30;
  }
  else {
  maxday = 31; //все остальные
  }
  if (ll ==2 && yy % 4 ==0) { //Февраль отдельно
    maxday = 29;
  }
  if (ll ==2 && ll % 4 !=0) {
    maxday = 28;
  }

if (dd1 > maxday) dd1 = 1;
if (dd1 < 1) dd1 = maxday;       
     isAlarmButton();
} 

if (nivel == -1)   // Настройка статуса будильника
{
    
printText(utf8rus("СТАТУС"), ST7735_GREEN,10,18,2);  
printText(utf8rus("БУДИЛЬНИКА:"), ST7735_GREEN,10,36,1);  
               
      //tft.fillRect(50,50,70,18,ST7735_BLACK);
      tft.fillRect(0,50,128,38,ST7735_BLACK);
      delay(50);
      printText(al%2? "Activate":"Off", ST7735_WHITE,10,60,2);
      delay(50);
      
  al = plusOrMinusFunc(al);
  checkMeniuAlarm(); 
  isAlarmButton();
}

if (nivel == -2)   // Настройка Часов будильника
{
tft.fillRect(10,15,100,38,ST7735_BLACK);
printText(utf8rus("ЧАСЫ"), ST7735_GREEN,10,18,2);  
printText(utf8rus("БУДИЛЬНИКА:"), ST7735_GREEN,10,36,1); 
dtostrf(hha,3, 0, chartemp); 
      //tft.fillRect(50,50,70,18,ST7735_BLACK);
      tft.fillRect(0,50,128,38,ST7735_BLACK);
      delay(50);
      printText(chartemp, ST7735_WHITE,0,60,2);
      delay(50);
      
hha = plusOrMinusFunc(hha);
checkMeniuAlarm();
 
if (hha > 23) hha = 0;
if (hha < 0) hha = 23;       
     isAlarmButton();
}  

if (nivel == -3)   // Настройка минут будильника
{

tft.fillRect(10,15,70,18,ST7735_BLACK);
printText(utf8rus("МИНУТЫ"), ST7735_GREEN,10,18,2);  
tft.fillRect(10,35,70,18,ST7735_BLACK);
printText(utf8rus("БУДИЛЬНИКА:"), ST7735_GREEN,10,35,1);  
Serial.println(mm1);
dtostrf(mma,3, 0, chartemp); 
      tft.fillRect(0,50,128,30,ST7735_BLACK);
      delay(50);
      printText(chartemp, ST7735_WHITE,0,60,2);
      delay(50);
     
mma = plusOrMinusFunc(mma);
checkMeniuAlarm();
if (mma > 59) mma = 0;
if (mma < 0) mma = 59;       
isAlarmButton();  
}  
if (nivel >=7||nivel<=-4 ) // Возвращение на основной экран
 {
  tft.fillScreen(ST7735_BLACK);
  if (nivel >=7)
  {
  setDS3231time(0, mm1, hh1, zz1, dd1, ll1, yy1);
  }

  if (nivel <=4)
  {
  EEPROM.write(100, hha);
  EEPROM.write(101, mma);
  EEPROM.write(102, al%2);
  }
  nivel = 0;  
  previousTemperature= 0.0;
  previousHumidity= 0;
  initial = 1;
  printText(utf8rus("ТЕМПЕРАТУРА"), ST7735_GREEN,5,90,1);  
  printText(utf8rus("ВЛАЖНОСТЬ"), ST7735_RED,5,110,1);
 }
}  // Конец основного цикла

void checkMeniu() //Проверка нажатия кнопки set
{
 if (digitalRead(meniu) == LOW)
  {
  nivel = nivel+1;
   delay(150);
  tft.fillScreen(ST7735_BLACK);
  tft.fillRect(0,0,128,160,ST7735_BLACK);
  }    
}

void checkMeniuAlarm() //Проверка нажатия кнопки set для экранов будильника
{
 if (digitalRead(meniu) == LOW)
  {
  nivel = nivel-1;
   delay(150);
  tft.fillScreen(ST7735_BLACK);
  tft.fillRect(0,0,128,160,ST7735_BLACK);
  }    
}

int plusOrMinusFunc(int var) //Проверка нажатия кнопок plus или minus
{
  if (digitalRead(plus) == LOW)
    {
    var = var + 1;
    delay(150);
    }
   if(digitalRead(minus) == LOW)
    {
    var = var - 1;
    delay(150);
    }
    return var;
}


void isAlarmButton() //Проверка нажатия кнопки будильника
{
if (digitalRead(alarm) == LOW)
  {
  nivel = 0;
  tft.fillScreen(ST7735_BLACK);
  tft.fillRect(0,0,128,160,ST7735_BLACK);
  delay(150);
  previousTemperature= 0.0;
  previousHumidity= 0;
  initial = 1;
  printText(utf8rus("ТЕМПЕРАТУРА"), ST7735_GREEN,5,90,1);  
  printText(utf8rus("ВЛАЖНОСТЬ"), ST7735_RED,5,110,1);
  }
}    

void printText(String text, uint16_t color, int x, int y,int textSize) //Функция для удобства работы с текстом на дисплее
{
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.setTextSize(textSize);
  tft.setTextWrap(true);
  tft.print(text);
}

String getDayOfWeek(int i) //Функция возвращающая дни недели от их порядка
{
  switch(i)
  {
    case 0: return utf8rus("Пн");break;
    case 1: return utf8rus("Вт");break;
    case 2: return utf8rus("Ср");break;
    case 3: return utf8rus("Чт");break;
    case 4: return utf8rus("Пт");break;
    case 5: return utf8rus("Сб");break;
    case 6: return utf8rus("Вс");break;
    default: return utf8rus("Пн");break;
  }
}


void setDS3231time(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year)// установка времени на модуле DS3231
{
    // sets time and date data to DS3231
    Wire.beginTransmission(DS3231_I2C_ADDRESS);
    Wire.write(0); // set next input to start at the seconds register
    Wire.write(decToBcd(second)); // set seconds
    Wire.write(decToBcd(minute)); // set minutes
    Wire.write(decToBcd(hour)); // set hours
    Wire.write(decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
    Wire.write(decToBcd(dayOfMonth)); // set date (1 to 31)
    Wire.write(decToBcd(month)); // set month
    Wire.write(decToBcd(year)); // set year (0 to 99)
    Wire.endTransmission();
}

// Конвертация бинарного кода в десятичный 
byte bcdToDec(byte val)
{
    return ( (val / 16 * 10) + (val % 16) );
}

// Обратная конвертация
byte decToBcd(byte val)
{
    return ( (val / 10 * 16) + (val % 10) );
}

// Начало функции обработки кириллических символов
String utf8rus(String source)     // Функция для конвертации русских символов из кодировки CP1251 в UTF8
{
  int i,k;
  String target;
  unsigned char n;
  char m[2] = { '0', '\0' };
  k = source.length(); i = 0;
  while (i < k) {
    n = source[i]; i++;
 
    if (n >= 0xBF){
      switch (n) {
        case 0xD0: {
          n = source[i]; i++;
          if (n == 0x81) { n = 0xA8; break; }
          if (n >= 0x90 && n <= 0xBF) n = n + 0x2F;
          break;
        }
        case 0xD1: {
          n = source[i]; i++;
          if (n == 0x91) { n = 0xB7; break; }
          if (n >= 0x80 && n <= 0x8F) n = n + 0x6F;
          break;
        }
      }
    }
    m[0] = n; target = target + String(m);
  }
  return target;
}
// Конец функции обработки кириллических симоволов
void readDS3231time(byte *second,byte *minute, byte *hour,byte *dayOfWeek, byte *dayOfMonth, byte *month, byte *year) //Чтение времени с модуля DS3231
{
    Wire.beginTransmission(DS3231_I2C_ADDRESS);
    Wire.write(0); // set DS3231 register pointer to 00h
    Wire.endTransmission();
    Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
    // request seven bytes of data from DS3231 starting from register 00h
    *second = bcdToDec(Wire.read() & 0x7f);
    *minute = bcdToDec(Wire.read());
    *hour = bcdToDec(Wire.read() & 0x3f);
    *dayOfWeek = bcdToDec(Wire.read());
    *dayOfMonth = bcdToDec(Wire.read());
    *month = bcdToDec(Wire.read());
    *year = bcdToDec(Wire.read());
}

