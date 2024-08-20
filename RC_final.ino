#include <SoftwareSerial.h>  // 시리얼 통신 지원
#include <Wire.h>  // I2C 통신 지원
#include <MsTimer2.h>  // 타이머 인터럽트 지원
#include <WiFiEsp.h>  // WiFi 통신 지원

#define AP_SSID "iot0"
#define AP_PASS "iot00000"
#define SERVER_NAME "10.10.141.71"
#define SERVER_PORT 5000
#define LOGID "PMH_RC"
#define PASSWD "PASSWD"

#define ARR_CNT 5  // 배열 최대 크기
#define CMD_SIZE 60  // 버퍼 사이즈

char sendBuf[CMD_SIZE];  // 데이터 담을 버퍼

#define WIFITX 1  //7:TX -->ESP8266 RX
#define WIFIRX 0  //6:RX-->ESP8266 TX

//SoftwareSerial BTSerial(2, 3);  // 블루투스 모듈 tx, rx
SoftwareSerial wifiSerial(WIFIRX, WIFITX);  // 와이파이 모듈 rx, tx
WiFiEspClient client;  // 와이파이 객체 생성

String ms = "";
int speed=100;  // 속력
int a;
int num=5;
bool up = false;
bool down = false;
bool left = false;
bool right = false;

// 이전 상태를 저장하기 위한 변수
bool prevUp = false;
bool prevDown = false;
bool prevLeft = false;
bool prevRight = false;

void setup() {
  //BTSerial.begin(9600);  // 블루투스 모듈 통신 속도 설정 및 시작
  // 모터 드라이브 6개의 선 출력 모드 설정
  pinMode(6,OUTPUT);
  pinMode(7,OUTPUT);
  pinMode(8,OUTPUT);
  pinMode(9,OUTPUT);
  pinMode(10,OUTPUT);
  pinMode(11,OUTPUT);
  // 와이파이 셋업
  wifi_Setup();

  analogWrite(11,speed);
  digitalWrite(10,HIGH);
  digitalWrite(9,HIGH);
  digitalWrite(8,HIGH);
  digitalWrite(7,HIGH);
  analogWrite(6,speed);
}

void loop() {
  // if (BTSerial.available())  // 블루투스 데이터 수신 확인
  //   bluetoothEvent();  // 이벤트 시작
  if (client.available()) {
    socketEvent();
  }
  
  // 현재 상태와 이전 상태 비교
  if (up != prevUp || down != prevDown || left != prevLeft || right != prevRight) {
    // 방향 동작의 상태가 변경되었을 때만 조건문 실행
    movement();

    // 이전 상태 갱신
    prevUp = up;
    prevDown = down;
    prevLeft = left;
    prevRight = right;
  }
}

void movement()
{
  if (up && !down)  // 전진 방향
  {
    if (left && !right)  // 좌, 전진
    {
      leftStop();
      rightGo();
      //sprintf(sendBuf, "[%s]%s@left_go\n", pArray[0], pArray[1]);
    } 
    else if (right && !left)  // 우, 전진
    {
      leftGo();
      rightStop();
      //sprintf(sendBuf, "[%s]%s@right_go\n", pArray[0], pArray[1]);
    } 
    else  // 일자 전진
    {
      leftGo();
      rightGo();
      //sprintf(sendBuf, "[%s]%s@straight_go\n", pArray[0], pArray[1]);
    }
  } 
  else if (down && !up)  // 후진 방향
  {
    if (left && !right)  // 좌측 방향 후진 
    {
      leftStop();
      rightBack();
      //sprintf(sendBuf, "[%s]%s@left_back\n", pArray[0], pArray[1]);
    }
    else if (right && !left)  // 우측 방향 후진
    {
      leftBack();
      rightStop();
      //sprintf(sendBuf, "[%s]%s@right_back\n", pArray[0], pArray[1]);
    } 
    else  // 일자 후진
    {
      leftBack();
      rightBack();
      //sprintf(sendBuf, "[%s]%s@straight_back\n", pArray[0], pArray[1]);
    }
  } 
  else  // 전진도 후진도 아닌 상태
  {
    if (left && !right)  // 좌향좌
    {
      leftBack();
      rightGo();
      //sprintf(sendBuf, "[%s]%s@strong_left\n", pArray[0], pArray[1]);
    } 
    else if (right && !left)  // 우향우
    {
      leftGo();
      rightBack();
      //sprintf(sendBuf, "[%s]%s@strong_right\n", pArray[0], pArray[1]);
    } 
    else  // 정지
    {
      leftStop();
      rightStop();
      //sprintf(sendBuf, "[%s]%s@stop\n", pArray[0], pArray[1]);
    }
  }
}

void socketEvent()
{
  int i = 0;
  int len;
  char * pToken;
  char * pArray[ARR_CNT] = {0};  // 현재, 최대 5개 배열 초기화
  char recvBuf[CMD_SIZE] = {0};  // CMD_SIZE만큼 받아올 버퍼 초기화
  sendBuf[0] = '\0';
  // 받아온 데이터의 길이 저장, 개행 문자 전까지 recvBuf에 저장하고 널문자 만나면 리턴
  // sizeof(recvBuf) - 1 이거는 마지막을 널 문자로 저장하기 위해서
  //int len = BTSerial.readBytesUntil('\n', recvBuf, sizeof(recvBuf) - 1);
  len = client.readBytesUntil('\n', recvBuf, CMD_SIZE);
  client.flush();

  // 받아온 문자열 잘라내기
  pToken = strtok(recvBuf, "[@]");
  while (pToken != NULL)
  {
    pArray[i] = pToken;
    if (++i >= ARR_CNT)  // ARR_CNT 크기 만큼만
      break;
    pToken = strtok(NULL, "[@]");
  }

  // ex) [RC]SET@100
  if (!strcmp(pArray[1], "SET")) {
    speed = atoi(pArray[2]);
    speed = map(speed,0,100,0,255);
    sprintf(sendBuf, "[%s]%s@%s\n", pArray[0], pArray[1], pArray[2]);
  }

  // ex) [RC]x@y -> [RC]240@560
  // ex) [RC]GO, [RC]GO@100
  // 수정!!!!
  // ex) [RC]GO@x@y -> [RC]GO@240@560

  /*  [0,    0] [500,    0] [1023,    0]
      [0,  500] [500,  500] [1023,  500]   + 버튼 누르면 SW값 0, 기본값 1
      [0, 1023] [500, 1023] [1023, 1023]
  */
  // if (!strcmp(pArray[1], "GO"))
  // {
  //   // 400,600일때
  //   // && 뒤에 확인
  //   up = (atoi(pArray[3]) < 1400);
  //   down = (atoi(pArray[3]) > 2800);
  //   left = (atoi(pArray[2]) < 1400);
  //   right = (atoi(pArray[2]) > 2800);

  //   sprintf(sendBuf, "[%s]%s@%s@%s\n", pArray[0], pArray[1], pArray[2], pArray[3]);
  // }
  up = (atoi(pArray[2]) < 1400);
  down = (atoi(pArray[2]) > 2800);
  left = (atoi(pArray[1]) < 1400);
  right = (atoi(pArray[1]) > 2800);

  sprintf(sendBuf, "[%s]%s@%s\n", pArray[0], pArray[1], pArray[2]);
  //BTSerial.write(sendBuf);
  client.write(sendBuf, strlen(sendBuf));
  client.flush();
  memset(sendBuf, 0, sizeof(sendBuf));  // sendBuf의 모든 값을 0으로 초기화
}

/*
  HIGH,  LOW : 전진
   LOW, HIGH : 후진
  HIGH, HIGH : 정지
*/
void leftGo()
{
  analogWrite(11,speed);
  digitalWrite(10,HIGH);
  digitalWrite(9,LOW);
}
void leftStop()
{
  analogWrite(11,speed);
  digitalWrite(10,HIGH);
  digitalWrite(9,HIGH);
}
void leftBack()
{
  analogWrite(11,speed);
  digitalWrite(10,LOW);
  digitalWrite(9,HIGH);
}
void rightGo()
{
  digitalWrite(8,LOW);
  digitalWrite(7,HIGH);
  analogWrite(6,speed);
}
void rightStop()
{
  digitalWrite(8,HIGH);
  digitalWrite(7,HIGH);
  analogWrite(6,speed);
}
void rightBack()
{
  digitalWrite(8,HIGH);
  digitalWrite(7,LOW);
  analogWrite(6,speed);
}

void wifi_Setup() {
  wifiSerial.begin(38400);
  wifi_Init();
  server_Connect();
}
void wifi_Init()
{
  do {
    WiFi.init(&wifiSerial);
    if (!(WiFi.status() == WL_NO_SHIELD))
      break;
  } while (1);

  while (WiFi.begin(AP_SSID, AP_PASS) != WL_CONNECTED) {
  }
}
int server_Connect()
{
  if (client.connect(SERVER_NAME, SERVER_PORT)) {
    client.print("["LOGID":"PASSWD"]");
  }
  else
  {
    client.print("fail..!!!");
  }
}