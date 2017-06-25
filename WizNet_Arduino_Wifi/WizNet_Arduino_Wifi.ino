#include "WizFi310.h"
#include "WizNet.h"

#include <TM1637Display.h> // 4Bit 시계 FND를 사용하기 위한 라이브러리 
#define CLK 2 // TM1637 클럭 연결 포트(D2)
#define DIO 3 // TM1637 데이터 연결 포트(D3)
boolean dotShow = true;

// DS3231 시간측정을 위한 Wire Lib Include
#include <Wire.h> // I2C 통신을 위한 라이브러리
#define DS3231_I2C_ADDRESS 104

byte seconds, minutes, hours, day, date, month, year;
char weekDay[4];

byte tMSB, tLSB;
float temp3231;

int reqNotCount = 0 ; // 서버 전송에 실패한 시각 카운트

unsigned long lastConnectionTime = 0;         // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 5 * 1000; // 업데이트 시간 설정, 5(*1000) 초

bool showdot = false;

// Initialize the WiFi client object
WiFiClient client;

//*********************************
// PMS7003을 위한 값 선언부
#ifndef    MAX_FRAME_LEN
#define    MAX_FRAME_LEN   64
#endif

#define TRIGGER_PIN   0
#define  DEBUG        1
#define  MEAN_NUMBER  10
#define  MAX_PM       0
#define  MIN_PM       32767
#define  VER          20161025

#ifndef    MAX_FRAME_LEN
#define    MAX_FRAME_LEN   64
#endif

#undef max
#define max(a,b) ((a)>(b)?(a):(b))
#undef min
#define min(a,b) ((a)>(b)?(b):(a))

#define PM01_0 0
#define PM10_0 1
#define PM25_0 2

// PMS7003 데이터 프레임 구조 선언
struct PM7003_framestruct {
  byte  frameHeader[2];
  unsigned int  frameLen = MAX_FRAME_LEN;
  unsigned int  concPM1_0_CF1;
  unsigned int  concPM2_5_CF1;
  unsigned int  concPM10_0_CF1;
  unsigned int  checksum;
} PM7003S;

String pmName[] = {"PM_1", "PM10", "PM25"};
int intPmName[] = {1, 10, 25};

int pmValue[] = {0, 0, 0} ; // pm1_0 = 0, pm2_5 = 0, pm10_0 = 0;
int dispPmValue[] = {0, 0, 0} ; // pm1_0 = 0, pm2_5 = 0, pm10_0 = 0;

unsigned int tmp_max[] = {0, 0, 0} ; // tmp_max_pm1_0, tmp_max_pm2_5, tmp_max_pm10_0;
unsigned int tmp_min[] = {0, 0, 0} ; // tmp_min_pm1_0, tmp_min_pm2_5, tmp_min_pm10_0;
byte readCount = 0;

bool ledState = LOW;

TM1637Display display(CLK, DIO);

void setup() {

  // display clear
  display.write(3, 3);
  display.write(2, 2);
  display.write(1, 1);
  display.write(0, 0);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(TRIGGER_PIN, INPUT);
  Serial.begin(9600);

  Wire.begin(); // RTC 통신포트 Open

  // Wifi 초기화
  wifi_init();

}

int dispCount = 0 ;
String reqString = "";
void loop() {
  display.dotShow(digitalRead(LED_BUILTIN));
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

  // 데이터 수신이 있을때만
  if ( digitalRead(TRIGGER_PIN) == LOW ) {
    //  Serial.print("Sensor Read...");
    //  Serial.println(readCount + 1);
    if (readCount == 0) {
      for (int k = 0 ; k < 3 ; k++) {
        tmp_max[k] =  MAX_PM;
        tmp_min[k] = MIN_PM;
      }
    }

    //  Serial.println("PM Read");
    if (PM7003_read()) {
      tmp_max[PM01_0] = max(PM7003S.concPM1_0_CF1, tmp_max[PM01_0]);
      tmp_max[PM10_0] = max(PM7003S.concPM10_0_CF1, tmp_max[PM10_0]);
      tmp_max[PM25_0] = max(PM7003S.concPM2_5_CF1, tmp_max[PM25_0]);

      tmp_min[PM01_0] = min(PM7003S.concPM1_0_CF1, tmp_min[PM01_0]);
      tmp_min[PM10_0] = min(PM7003S.concPM10_0_CF1, tmp_min[PM10_0]);
      tmp_min[PM25_0] = min(PM7003S.concPM2_5_CF1, tmp_min[PM25_0]);

      pmValue[PM01_0] += PM7003S.concPM1_0_CF1;
      pmValue[PM10_0] += PM7003S.concPM10_0_CF1;
      pmValue[PM25_0] += PM7003S.concPM2_5_CF1;

      display.write(dispPmValue[readCount % 3] % 10, 3);
      display.write(dispPmValue[readCount % 3] / 10, 2);
      display.write(intPmName[readCount % 3] % 10, 1);
      display.write(intPmName[readCount % 3] / 10, 0);


      readCount++;
    }


    if (readCount % MEAN_NUMBER == 0) {

      for (int k = 0 ; k < 3 ; k++) {
        pmValue[k] = ((pmValue[k] - tmp_max[k] - tmp_min[k]) / (MEAN_NUMBER - 2));
      }

      delay(1000);
      readCount = 0 ;
      for (int k = 0 ; k < 3; k++) {
        //        delay(1000);
        dispPmValue[k] = pmValue[k] ;
        pmValue[k] = 0;
      }
    }
    serverUpdate();
  }
  delay(5000);
}

void serverUpdate() {


  // 서버로 전송시간을 체크하기 위해 먼저 초(seconds)값을 문자열로 변환 후 숫자로 변환
  int curSecond = String(seconds, DEC).toInt();

  if (millis() > lastConnectionTime + postingInterval) {

    get3231Date();
    // 시간변수로 부터 시간 값 가져와서 문자열로 변환
    String pDate = "20" + String(year, DEC) + "-"  ;

    String tmpString = String(month, DEC);
    pDate += (tmpString.length() < 2 ? "0" + tmpString : tmpString) + "-" ;
    pDate += String(date, DEC);
    //
    tmpString = String(hours, DEC);
    String pTime = (tmpString.length() < 2 ? "0" + tmpString : tmpString) + ":";

    tmpString = String(minutes, DEC);
    pTime += (tmpString.length() < 2 ? "0" + tmpString : tmpString) + ":"  ;

    tmpString = String(seconds, DEC);
    pTime += (tmpString.length() < 2 ? "0" + tmpString : tmpString) ;

    /*
      PM25_NO : String,
      PM25_DATE : String,
      PM25_TIME : String,
      PM25_01 : Number,
      PM25_10 : Number,
      PM25_25 : Number
    */

    reqString = "/pm25/P001/"
                + pDate
                + "/" + pTime
                + "/" + dispPmValue[0]
                + "/" + dispPmValue[1]
                + "/" + dispPmValue[2]
                + "/" + String(get3231Temp(), 2) ;

    /*
      Serial.print(reqString);
      Serial.print("\t");
      Serial.print(get3231Temp(), DEC);
      Serial.println();
    */

    if (reqNotCount < 10) {
      if (!httpRequest(reqString)) {
        reqNotCount ++ ; // 서버 전송에 실패하면 reqNotCount 1 증가
        display.write(0, 3);
        display.write(0, 2);
        display.write(0, 1);
        display.write(0, 0);
      }
    } else {
      // 만약 서버전송이 10회 이상 실피하면 다시 Wifi 초기화
      wifi_init();
      reqNotCount = 0 ;
    }
  }
}


