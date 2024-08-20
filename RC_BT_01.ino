#include <SoftwareSerial.h>  // 시리얼 통신 지원
#include <Wire.h>  // I2C 통신 지원
#include <MsTimer2.h>  // 타이머 인터럽트 지원

#define ARR_CNT 5  // 배열 최대 크기
#define CMD_SIZE 60  // 버퍼 사이즈

char sendBuf[CMD_SIZE];  // 데이터 담을 버퍼
char recvId[10] = "PMH_LIN";  // SQL 저장 클라이이언트 ID

SoftwareSerial BTSerial(2, 3);  // 블루투스 모듈 tx, rx

String ms = "";
int speed=100;  // 속력
int a;
int num=5;
bool up = false;
bool down = false;
bool left = false;
bool right = false;

void setup() {
  BTSerial.begin(9600);  // 블루투스 모듈 통신 속도 설정 및 시작
  // 모터 드라이브 6개의 선 출력 모드 설정
  pinMode(6,OUTPUT);
  pinMode(7,OUTPUT);
  pinMode(8,OUTPUT);
  pinMode(9,OUTPUT);
  pinMode(10,OUTPUT);
  pinMode(11,OUTPUT);
}

void loop() {
  if (BTSerial.available())  // 블루투스 데이터 수신 확인
    bluetoothEvent();  // 이벤트 시작

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

void bluetoothEvent()
{
  int i = 0;
  char * pToken;
  char * pArray[ARR_CNT] = {0};  // 현재, 최대 5개 배열 초기화
  char recvBuf[CMD_SIZE] = {0};  // CMD_SIZE만큼 받아올 버퍼 초기화
  // 받아온 데이터의 길이 저장, 개행 문자 전까지 recvBuf에 저장하고 널문자 만나면 리턴
  // sizeof(recvBuf) - 1 이거는 마지막을 널 문자로 저장하기 위해서
  int len = BTSerial.readBytesUntil('\n', recvBuf, sizeof(recvBuf) - 1);

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
  if (!strcmp(pArray[1], "GO"))
  {
    // 400,600일때
    // && 뒤에 확인
    up = (atoi(pArray[3]) < 400);
    down = (atoi(pArray[3]) > 600);
    left = (atoi(pArray[2]) < 400);
    right = (atoi(pArray[2]) > 600);

    sprintf(sendBuf, "[%s]%s@%s@%s\n", pArray[0], pArray[1], pArray[2], pArray[3]);
  }
  BTSerial.write(sendBuf);
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