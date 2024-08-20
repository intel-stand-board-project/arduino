 #include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3);  // 블루투스 모듈 tx, rx

String ms = "";
int sp=100;  // 속력
int a;
int num=5;

void setup() {
  mySerial.begin(9600);  // 블루투스 모듈 속도
  // 모터 드라이브 6개의 선
  pinMode(6,OUTPUT);
  pinMode(7,OUTPUT);
  pinMode(8,OUTPUT);
  pinMode(9,OUTPUT);
  pinMode(10,OUTPUT);
  pinMode(11,OUTPUT);
}

void loop() {
  if(mySerial.available()){  // 블루투스 신호
    ms = mySerial.readStringUntil('c');  // c 앞까지 읽음
    a=ms.toInt();
    if(a>9){
      sp=a;
    }else{  // 1~9는 버튼 신호로 인식
      num=a;
    }
    // 11,10, 9 왼쪽 모터
    //  8, 7, 6 오른쪽 모터
    if(num==1){  // L : STOP, R : GO => 왼쪽 턴
      analogWrite(11,sp);
      digitalWrite(10,HIGH); // 10,9 둘 다 HIGH이므로 동작 안함, +,+로 같아서 전류가 흐르지 않음
      digitalWrite(9,HIGH);
      digitalWrite(8,HIGH);
      digitalWrite(7,LOW);
      analogWrite(6,sp);
    }else if(num==2){  // L : GO, R : GO => 전진
      analogWrite(11,sp);
      digitalWrite(10,HIGH);
      digitalWrite(9,LOW);
      digitalWrite(8,HIGH);
      digitalWrite(7,LOW);
      analogWrite(6,sp);
    }else if(num==3){  // L : GO, R : STOP => 오른쪽 턴
      analogWrite(11,sp);
      digitalWrite(10,HIGH);
      digitalWrite(9,LOW);
      digitalWrite(8,HIGH);
      digitalWrite(7,HIGH);
      analogWrite(6,sp);
    }else if(num==4){  // L : BACK, R : GO => 강한 왼쪽 턴
      analogWrite(11,sp);
      digitalWrite(10,LOW);  // 10 LOW, 9 HIGH 이므로 바퀴가 뒤로 돈다 
      digitalWrite(9,HIGH);
      digitalWrite(8,HIGH);
      digitalWrite(7,LOW);
      analogWrite(6,sp);
    }else if(num==5){  // L : STOP, R : STOP => 정지
      analogWrite(11,sp);
      digitalWrite(10,HIGH);
      digitalWrite(9,HIGH);
      digitalWrite(8,HIGH);
      digitalWrite(7,HIGH);
      analogWrite(6,sp);
    }else if(num==6){  // L : GO, R : BACK => 강한 오른쪽 턴
      analogWrite(11,sp);
      digitalWrite(10,HIGH);
      digitalWrite(9,LOW);
      digitalWrite(8,LOW);
      digitalWrite(7,HIGH);
      analogWrite(6,sp);
    }else if(num==7){  // L : STOP, R : BACK => 왼쪽 후진
      analogWrite(11,sp);
      digitalWrite(10,HIGH);
      digitalWrite(9,HIGH);
      digitalWrite(8,LOW);
      digitalWrite(7,HIGH);
      analogWrite(6,sp);
    }else if(num==8){  // L : BACK, R : BACK => 후진
      analogWrite(11,sp);
      digitalWrite(10,LOW);
      digitalWrite(9,HIGH);
      digitalWrite(8,LOW);
      digitalWrite(7,HIGH);
      analogWrite(6,sp);
    }else if(num==9){  // L : BACK, R : STOP => 오른쪽 후진
      analogWrite(11,sp);
      digitalWrite(10,LOW);
      digitalWrite(9,HIGH);
      digitalWrite(8,HIGH);
      digitalWrite(7,HIGH);
      analogWrite(6,sp);
    }
  }
}
