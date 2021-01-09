#include <Arduino.h>
#include <SoftwareSerial.h>
#include <DS3231.h>
#include <Wire.h>
#include <string.h>
#include <EEPROM.h> // ???

// Time variables
DS3231 Clock;

bool Century = false;
bool h12;
bool PM;
bool ADy, A12h, Apm;

byte ADay, AHour, AMinute, ASecond, ABits;
byte year, month, date, DoW, hour, minute, second;

// CONSTANTS
#define LED 10
#define firstAnalogPin 54

// ####### ALARM(PLANNED IRRIGATION) CONSTANTS #######

#define ALARM_SECOND_EEPROM 0
#define ALARM_MINUTE_EEPROM 1
#define ALARM_HOUR_EEPROM 2
#define ALARM_DAY_EEPROM 3
#define ALARM_MONTH_EEPROM 4
#define ALARM_YEAR_EEPROM 5

#define STOP_ALARM_SECOND_EEPROM 6
#define STOP_ALARM_MINUTE_EEPROM 7
#define STOP_ALARM_HOUR_EEPROM 8
#define STOP_ALARM_DAY_EEPROM 9
#define STOP_ALARM_MONTH_EEPROM 10
#define STOP_ALARM_YEAR_EEPROM 11

// ####### SETTINGS CONSTANTS #######

#define SENSORS_AMOUNT_EEPROM 12
#define IRRIGATIOM_MODE_EEPROM 13

// ####### CYCLIC IRRIGATION CONSTANTS #######
#define CYCLIC_MONDAY_EEPROM 14
#define CYCLIC_TUESDEY_EEPROM 15
#define CYCLIC_WEDNESDAY_EEPROM 16
#define CYCLIC_THURSDAY_EEPROM 17
#define CYCLIC_FRIDAY_EEPROM 18
#define CYCLIC_SATURDAY_EEPROM 19
#define CYCLIC_SUNDAY_EEPROM 20
#define CYCLIC_START_HOUR_EEPROM 21
#define CYCLIC_START_MINUTE_EEPROM 22
#define CYCLIC_STOP_HOUR_EEPROM 23
#define CYCLIC_STOP_MINUTE_EEPROM 24

int moistureSensorsAmonut = 0;
float averageMoisture;

char incomingValue;
char outcomingValue;

String dataFromPhone;

SoftwareSerial hc06(2, 3); // 2 - Rx, 3 - Tx | Arduino Rx -> HC Tx # Arduino Tx -> HC Rx by divider

// ################################ FUNCTIONS ################################

bool startCyclicIrrigation()
{
  // int second = Clock.getSecond();
  int minute = Clock.getMinute();
  int hour = Clock.getHour(h12, PM);
  int day = Clock.getDoW();

  int monday = EEPROM.read(CYCLIC_MONDAY_EEPROM);
  int tuesdey = EEPROM.read(CYCLIC_TUESDEY_EEPROM);
  int wednesday = EEPROM.read(CYCLIC_WEDNESDAY_EEPROM);
  int thursday = EEPROM.read(CYCLIC_THURSDAY_EEPROM);
  int friday = EEPROM.read(CYCLIC_FRIDAY_EEPROM);
  int saturday = EEPROM.read(CYCLIC_SATURDAY_EEPROM);
  int sunday = EEPROM.read(CYCLIC_SUNDAY_EEPROM);

  int startHour = EEPROM.read(CYCLIC_START_HOUR_EEPROM);
  int startMinute = EEPROM.read(CYCLIC_START_MINUTE_EEPROM);

  switch (day)
  {
  case 1:
    if (monday == 1 && hour == startHour && minute == startMinute)
    {
      return true;
    }
    break;

  case 2:
    if (tuesdey == 1 && hour == startHour && minute == startMinute)
    {
      return true;
    }
    break;

  case 3:
    if (wednesday == 1 && hour == startHour && minute == startMinute)
    {
      return true;
    }
    break;

  case 4:
    if (thursday == 1 && hour == startHour && minute == startMinute)
    {
      return true;
    }
    break;

  case 5:
    if (friday == 1 && hour == startHour && minute == startMinute)
    {
      return true;
    }
    break;

  case 6:
    if (saturday == 1 && hour == startHour && minute == startMinute)
    {
      return true;
    }
    break;

  case 7:
    if (sunday == 1 && hour == startHour && minute == startMinute)
    {
      return true;
    }
    break;

  default:
    return false;
    break;
  }
  return false;
}

bool stopCyclicIrrigation()
{
  // int second = Clock.getSecond();
  int minute = Clock.getMinute();
  int hour = Clock.getHour(h12, PM);
  int day = Clock.getDoW();

  int monday = EEPROM.read(CYCLIC_MONDAY_EEPROM);
  int tuesdey = EEPROM.read(CYCLIC_TUESDEY_EEPROM);
  int wednesday = EEPROM.read(CYCLIC_WEDNESDAY_EEPROM);
  int thursday = EEPROM.read(CYCLIC_THURSDAY_EEPROM);
  int friday = EEPROM.read(CYCLIC_FRIDAY_EEPROM);
  int saturday = EEPROM.read(CYCLIC_SATURDAY_EEPROM);
  int sunday = EEPROM.read(CYCLIC_SUNDAY_EEPROM);

  int stopHour = EEPROM.read(CYCLIC_STOP_HOUR_EEPROM);
  int stopMinute = EEPROM.read(CYCLIC_STOP_MINUTE_EEPROM);

  switch (day)
  {
  case 1:
    if (monday == 1 && hour == stopHour && minute == stopMinute)
    {
      return true;
    }
    break;

  case 2:
    if (tuesdey == 1 && hour == stopHour && minute == stopMinute)
    {
      return true;
    }
    break;

  case 3:
    if (wednesday == 1 && hour == stopHour && minute == stopMinute)
    {
      return true;
    }
    break;

  case 4:
    if (thursday == 1 && hour == stopHour && minute == stopMinute)
    {
      return true;
    }
    break;

  case 5:
    if (friday == 1 && hour == stopHour && minute == stopMinute)
    {
      return true;
    }
    break;

  case 6:
    if (saturday == 1 && hour == stopHour && minute == stopMinute)
    {
      return true;
    }
    break;

  case 7:
    if (sunday == 1 && hour == stopHour && minute == stopMinute)
    {
      return true;
    }
    break;

  default:
    return false;
    break;
  }
  return false;
}

int mode()
{
  int irrigationMode = EEPROM.read(IRRIGATIOM_MODE_EEPROM);

  if (irrigationMode == 1)
  {
    return 1;
  }
  else if (irrigationMode == 2)
  {
    return 2;
  }
  else if (irrigationMode == 3) 
  {
    return 3;
  }
  return 1;
}

bool isFirstAlarm()
{
  int secondEEPROM, minuteEEPROM, hourEEPROM, dayEEPROM, monthEEPROM, yearEEPROM, second, minute, hour, day, month, year;

  second = Clock.getSecond();
  minute = Clock.getMinute();
  hour = Clock.getHour(h12, PM);
  day = Clock.getDate();
  month = Clock.getMonth(Century);
  year = Clock.getYear();

  secondEEPROM = EEPROM.read(ALARM_SECOND_EEPROM);
  minuteEEPROM = EEPROM.read(ALARM_MINUTE_EEPROM);
  hourEEPROM = EEPROM.read(ALARM_HOUR_EEPROM);
  dayEEPROM = EEPROM.read(ALARM_DAY_EEPROM);
  monthEEPROM = EEPROM.read(ALARM_MONTH_EEPROM);
  yearEEPROM = EEPROM.read(ALARM_YEAR_EEPROM);

  //second == secondEEPROM &&

  if (minute == minuteEEPROM && hour == hourEEPROM && day == dayEEPROM && month == monthEEPROM && year == yearEEPROM)
  {
    return true;
  }
  return false;
}

bool isStopFirstAlarm()
{
  int secondEEPROM, minuteEEPROM, hourEEPROM, dayEEPROM, monthEEPROM, yearEEPROM, second, minute, hour, day, month, year;

  second = Clock.getSecond();
  minute = Clock.getMinute();
  hour = Clock.getHour(h12, PM);
  day = Clock.getDate();
  month = Clock.getMonth(Century);
  year = Clock.getYear();

  secondEEPROM = EEPROM.read(STOP_ALARM_SECOND_EEPROM);
  minuteEEPROM = EEPROM.read(STOP_ALARM_MINUTE_EEPROM);
  hourEEPROM = EEPROM.read(STOP_ALARM_HOUR_EEPROM);
  dayEEPROM = EEPROM.read(STOP_ALARM_DAY_EEPROM);
  monthEEPROM = EEPROM.read(STOP_ALARM_MONTH_EEPROM);
  yearEEPROM = EEPROM.read(STOP_ALARM_YEAR_EEPROM);

  //second == secondEEPROM &&

  if (minute == minuteEEPROM && hour == hourEEPROM && day == dayEEPROM && month == monthEEPROM && year == yearEEPROM)
  {
    return true;
  }
  return false;
}

void readTime()
{
  int second, minute, hour, date, month, year, temperature;
  second = Clock.getSecond();
  minute = Clock.getMinute();
  hour = Clock.getHour(h12, PM);
  date = Clock.getDate();
  month = Clock.getMonth(Century);
  year = Clock.getYear();

  temperature = Clock.getTemperature();

  hc06.print("T");
  hc06.print("20");
  hc06.print(year, DEC);
  hc06.print('.');
  if (month < 10)
    hc06.print("0");
  hc06.print(month, DEC);
  hc06.print('.');
  if (date < 10)
    hc06.print("0");
  hc06.print(date, DEC);
  hc06.print(' ');
  if (hour < 10)
    hc06.print("0");
  hc06.print(hour, DEC);
  hc06.print(':');
  if (minute < 10)
    hc06.print("0");
  hc06.print(minute, DEC);
  hc06.print(':');
  if (second < 10)
    hc06.print("0");
  hc06.print(second, DEC);
  hc06.print('\n');
  hc06.print("Temperatura = ");
  if (temperature < 10)
    hc06.print("0");
  hc06.print(temperature);
  hc06.print("°C");
  hc06.print('\n');
}

void initAnalogs(int moistureSensorsAmonut)
{
  if (moistureSensorsAmonut > 0)
  {
    int endIteration = firstAnalogPin + moistureSensorsAmonut;

    for (int i = firstAnalogPin; i < endIteration; i++)
    {
      pinMode(i, INPUT);
    }
  }
}

float toAverage(int moistureSensorsAmonut)
{
  float avg = 0;

  if (moistureSensorsAmonut > 0)
  {
    float sum;

    int endIteration = firstAnalogPin + moistureSensorsAmonut;

    for (int i = firstAnalogPin; i < endIteration; i++)
    {
      sum += analogRead(uint8_t(i));
    }

    avg = sum / moistureSensorsAmonut;
  }
  return avg;
}

void setTime(int second, int minute, int hour, int dayOfWeek, int day, int month, int year)
{
  Clock.setSecond(second);
  Clock.setMinute(minute);
  Clock.setHour(hour);
  Clock.setDoW(dayOfWeek);
  Clock.setDate(day);
  Clock.setMonth(month);
  Clock.setYear(year);
}

int *readDataFromBT(String dataFromPhone)
{
  static int dataArray[14];

  char data[dataFromPhone.length()];
  for (size_t i = 0; i < sizeof(data); i++)
  {
    data[i] = dataFromPhone[i];
  }

  int iterator = 0;
  char *token = strtok(data, ",");

  while (token != NULL)
  {
    if (iterator > 0)
    {
      dataArray[iterator] = atoi(token);
    }
    token = strtok(NULL, ",");
    iterator++;
  }

  return dataArray;
}

void setup()
{
  Wire.begin();
  Serial.begin(9600);
  hc06.begin(9600);

  initAnalogs(moistureSensorsAmonut);
  pinMode(LED, OUTPUT); // simple LED
}

void loop()
{

  if (hc06.available() > 0)
  {
    // incomingValue = hc06.read();
    // Serial.write(incomingValue);
    dataFromPhone = hc06.readString();
    Serial.print(dataFromPhone);
    if (dataFromPhone[0] == 'R' && dataFromPhone[1] == 'T' && dataFromPhone[2] == 'C')
    {
      hc06.print(dataFromPhone);

      int *timeFromBT;
      timeFromBT = readDataFromBT(dataFromPhone);

      int timeData[7];

      for (int i = 1; i < 8; i++)
      {
        timeData[i] = *(timeFromBT + i);
      }

      setTime(timeData[1], timeData[2], timeData[3], timeData[4], timeData[5], timeData[6], timeData[7]);
      readTime();
    }
    else if (dataFromPhone[0] == 'A' && dataFromPhone[1] == 'T' && dataFromPhone[2] == 'S')
    {
      int *timeFromBT;
      timeFromBT = readDataFromBT(dataFromPhone);

      int timeData[14];
      for (int i = 1; i < 15; i++)
      {
        timeData[i] = *(timeFromBT + i);
      }

      EEPROM.write(ALARM_SECOND_EEPROM, timeData[1]);
      delay(3.3);
      EEPROM.write(ALARM_MINUTE_EEPROM, timeData[2]);
      delay(3.3);
      EEPROM.write(ALARM_HOUR_EEPROM, timeData[3]);
      delay(3.3);
      EEPROM.write(ALARM_DAY_EEPROM, timeData[5]);
      delay(3.3);
      EEPROM.write(ALARM_MONTH_EEPROM, timeData[6]);
      delay(3.3);
      EEPROM.write(ALARM_YEAR_EEPROM, timeData[7]);
      delay(3.3);
      EEPROM.write(STOP_ALARM_SECOND_EEPROM, timeData[8]);
      delay(3.3);
      EEPROM.write(STOP_ALARM_MINUTE_EEPROM, timeData[9]);
      delay(3.3);
      EEPROM.write(STOP_ALARM_HOUR_EEPROM, timeData[10]);
      delay(3.3);
      EEPROM.write(STOP_ALARM_DAY_EEPROM, timeData[12]);
      delay(3.3);
      EEPROM.write(STOP_ALARM_MONTH_EEPROM, timeData[13]);
      delay(3.3);
      EEPROM.write(STOP_ALARM_YEAR_EEPROM, timeData[14]);
    }
    else if (dataFromPhone[0] == 'S' && dataFromPhone[1] == 'E' && dataFromPhone[2] == 'T')
    {
      int *settingsFromBT;
      settingsFromBT = readDataFromBT(dataFromPhone);

      int timeData[3];
      for (int i = 1; i < 4; i++)
      {
        timeData[i] = *(settingsFromBT + i);
      }

      EEPROM.write(SENSORS_AMOUNT_EEPROM, timeData[1]);
      delay(3.3);
      EEPROM.write(IRRIGATIOM_MODE_EEPROM, timeData[2]);
    }
    else if (dataFromPhone[0] == 'S' && dataFromPhone[1] == 'C' && dataFromPhone[2] == 'I')
    {
      int *timeFromBT;
      timeFromBT = readDataFromBT(dataFromPhone);

      int timeData[14];
      for (int i = 1; i < 12; i++)
      {
        timeData[i] = *(timeFromBT + i);
      }

      EEPROM.write(CYCLIC_MONDAY_EEPROM, timeData[1]);
      delay(3.3);
      EEPROM.write(CYCLIC_TUESDEY_EEPROM, timeData[2]);
      delay(3.3);
      EEPROM.write(CYCLIC_WEDNESDAY_EEPROM, timeData[3]);
      delay(3.3);
      EEPROM.write(CYCLIC_THURSDAY_EEPROM, timeData[4]);
      delay(3.3);
      EEPROM.write(CYCLIC_FRIDAY_EEPROM, timeData[5]);
      delay(3.3);
      EEPROM.write(CYCLIC_SATURDAY_EEPROM, timeData[6]);
      delay(3.3);
      EEPROM.write(CYCLIC_SUNDAY_EEPROM, timeData[7]);
      delay(3.3);
      EEPROM.write(CYCLIC_START_HOUR_EEPROM, timeData[8]);
      delay(3.3);
      EEPROM.write(CYCLIC_START_MINUTE_EEPROM, timeData[9]);
      delay(3.3);
      EEPROM.write(CYCLIC_STOP_HOUR_EEPROM, timeData[10]);
      delay(3.3);
      EEPROM.write(CYCLIC_STOP_MINUTE_EEPROM, timeData[11]);
    }
    else if (dataFromPhone[0] == 'p')
    {
      if (digitalRead(LED) == HIGH)
      {
        digitalWrite(LED, LOW);
      }
      else
      {
        digitalWrite(LED, HIGH);
      }
    }
  }

  //Write from Serial Monitor to HC06
  if (Serial.available() > 0)
  {
    hc06.write(Serial.read());
  }

  if (isFirstAlarm() && mode() == 2)
  {
    // TO DO: irrigate()
    digitalWrite(LED, HIGH);
  }
  if (isStopFirstAlarm() && mode() == 2)
  {
    // TO DO: stopIrrigate()
    digitalWrite(LED, LOW);
  }

  if (startCyclicIrrigation() && mode() == 3)
  {
    digitalWrite(LED, HIGH);
  }
  
  if(stopCyclicIrrigation() && mode() == 3) {
    digitalWrite(LED, LOW);
  }


  // if (!mode())
  // {
  //   // TO DO: stopIrrigate()
  //   digitalWrite(LED, LOW);
  // }
}