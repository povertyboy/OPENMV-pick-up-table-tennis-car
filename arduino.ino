#include <Wire.h>
#define BAUD_RATE 19200
#define CHAR_BUF 128

#define center_l 145 //控制球中心范围,应在0~320之间  中间为320
#define center_r 175

#define ENA 6 //左
#define ENB 3//右
int in1 = 7; //左轮
int in2 = 5;
int in3 = 4; //右轮
int in4 = 2;

int inputPin = 8;// 定义超声波信号接收接口
int outputPin = 9;// 定义超声波信号发出接口

unsigned int a = 0;//记录乱走的标志量
int i = 0;//记录超声波数值
void up(int d , int l , int r) {
  digitalWrite(in1, 1);
  digitalWrite(in2, 0);
  analogWrite (ENA, l);
  digitalWrite(in3, 1);
  digitalWrite(in4, 0);
  analogWrite (ENB, r);
  delay(d);
  stay();
}

void up_ns(int d , int l , int r) {
  digitalWrite(in1, 1);
  digitalWrite(in2, 0);
  analogWrite (ENA, l);
  digitalWrite(in3, 1);
  digitalWrite(in4, 0);
  analogWrite (ENB, r);
  delay(d);
}

void back(int d , int l , int r) {
  digitalWrite(in1, 0);
  digitalWrite(in2, 1);
  analogWrite (ENA, l);
  digitalWrite(in3, 0);
  digitalWrite(in4, 1);
  analogWrite (ENB, r);
  delay(d);
  stay();
}

void left(int d , int l , int r) {
  digitalWrite(in1, 0);
  digitalWrite(in2, 1);
  analogWrite(ENA, l);
  digitalWrite(in3, 1);
  digitalWrite(in4, 0);
  analogWrite(ENB, r);
  delay(d);
  stay();
}

void right(int d , int l , int r) {
  digitalWrite(in1, 1);
  digitalWrite(in2, 0);
  analogWrite(ENA, l);
  digitalWrite(in3, 0);
  digitalWrite(in4, 1);
  analogWrite(ENB, r);
  delay(d);
  stay();
}

void stay() {
  digitalWrite(in1, 0);
  digitalWrite(in2, 0);
  analogWrite (ENA, 0);
  digitalWrite(in3, 0);
  digitalWrite(in4, 0);
  analogWrite (ENB, 0);
}


void findball(int x) { //找球程序
  unsigned int i;
  int coordinate = x;
  int distance, j;
  int coordinate_;
  i = 0;
  while ( (coordinate= receive()) > 0 && i < 210 ) { //当回坐标值大于0则循环210次调整  并且每次在while里更新数据
    i++;
    Serial.print("正在调整中");
    if ( coordinate > 0 && coordinate < center_l ) { //说明球在左边，应向左转
      Serial.print("左转\n");
      left(15, 60, 60);
    }
    else if ( coordinate >= center_l && coordinate <= center_r) {
      Serial.print("直走\n");
      j=0;
      while ( (coordinate_= receive()) >= center_l && coordinate_<=center_r && j<10 ){//    重新接受并看是否在中间
        j++;/*
        distance = csb();//走的时候防止前方有障碍
        if (distance > 10)
          up_ns(30, 95, 95);     //没有停顿的直走
        else {
          up(30, 0, 0);
        }*/
        up(50, 95, 95);;     
        delay(1);
      }
      stay();
      Serial.println("找完球*******************");
      delay(100);         //找完球之后停顿一下
      coordinate = receive();//执行完直走之后，更新原来数据
      break;              //若在中间退出循环
    }
    else if ( coordinate > center_r && coordinate <= 320) {
      Serial.print("右转\n");
      right(15, 60, 60);
    } 
    if (coordinate == 0) break; //找球过程丢失球，退出，防止一直卡在这里
  }

  if (coordinate == 0) {
    Serial.print("未找到，执行瞎走程序\n");
    xiazou();
  }

}

void xiazou() {
  /*
    switch (a) {
      case 0: a++; right(250, 70, 60); stay();delay(1000);break;
      case 1: left(340, 70, 70; stay(); a++;delay(1000); break;
      case 2: up(400, 70, 55); stay(); a++;delay(1000);break;
      case 3: left(390, 60, 65); stay(); a++;delay(1000);break;
      case 4: right(250, 55, 70); stay(); a++;delay(1000);break;
      case 5: up(310, 70, 50); stay(); a++;delay(1000);break;
      case 6: right(300, 59, 70); stay(); a++;break;
      case 7: up(310, 65, 80);a = 0; break;
    }*/
  switch (a) {
    case 0: up(300, 65, 65); a++; delay(400); break;
    case 1: up(300, 65, 65); a++; delay(400); break;
    case 2: left(400, 65, 65); a++; delay(400); break;
    case 3: right(800, 65, 65); a++; delay(400); a = 0; break;
  }
  Serial.print("瞎走结束\n");

}

unsigned int csb()
{
  unsigned int i;//求平均值 i作为标志量
  unsigned int distance;
  i = 0;

  digitalWrite(outputPin, LOW); // 使发出发出超声波信号接口低电平2ms
  delayMicroseconds(2);
  digitalWrite(outputPin, HIGH); // 使发出发出超声波信号接口高电平10ms，这里是至少10μs
  delayMicroseconds(10);
  digitalWrite(outputPin, LOW); // 保持发出超声波信号接口低电平
  distance = pulseIn(inputPin, HIGH, 65535); // 读出脉冲时间
  distance = distance / 58; // 将脉冲时间转化为距离（单位：厘米）

  Serial.print(distance);
  Serial.println("cm");//向串口监视器中输出距离信息
  
  return 100;///////////////////////////先把超声波关闭
}

int receive() {
  unsigned short int coordinate;//坐标值
  int32_t temp = 0;
  char buff[CHAR_BUF] = {0};
  Serial.println("进入接收");
  Wire.requestFrom(0x12, 2);
  if (Wire.available() == 2) { // got length?
    temp = Wire.read() | (Wire.read() << 8);
    delay(1); // Give some setup time...
    Wire.requestFrom(0x12, temp);
    if (Wire.available() == temp) { // got full message
      temp = 0;
      while (Wire.available()) buff[temp++] = Wire.read();
    } else {
      while (Wire.available()) Wire.read(); // Toss garbage bytes.
    }
  } else {
    while (Wire.available()) Wire.read(); // Toss garbage bytes.
  }

  Serial.print(atoi(buff));//把接受到的字符串转换为数字
  Serial.println("X坐标");

  delay(15);      //防止检测太快
  coordinate = atoi(buff);
  return coordinate;
}


void setup() {
  Serial.begin(BAUD_RATE);
  Wire.begin();
  pinMode(in1, 1);
  pinMode(in2, 1);
  pinMode(in3, 1);
  pinMode(in4, 1);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(inputPin, INPUT);
  pinMode(outputPin, OUTPUT);
  delay(1000); // 给OpenMV一个启动的时间
}


void loop() {
  int x;

  while ((i = csb())>0){
    if (i > 0 && i < 30) {
      Serial.println("前方有障碍");
      stay();
      back(300, 80, 80);
      left(500, 75, 90);
    }
    else if (i > 30) {
      Serial.println("前方无障碍");
      x = receive();
      findball(x);
    }
    Serial.println("\n\n\n");
  }


}




