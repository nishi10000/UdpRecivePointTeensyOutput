/*
  UdpRecivePointOutput
  UDP_Receive_Laser
  UDPコマンドを受けて動作を行う。Galvoの電圧制御,LaserのON/OFFを制御する。
  フレームの管理は行わない。※送信側に依存する。

  The circuit:
  * input pinに接続されているコンポーネントのリスト
  * pin12 MISO
  * output pinに接続されているコンポーネントのリスト
  * pin9 RSTn
  * pin10 SCNn
  * pin11 MOSI
  * pin13 SCLK
  * pin2 laserOn
  * pinA22 Y_Galvo
  * pinA21 X_Galvo

  Created 2020/04/24
  By Yu Nishihara
*/
#include <SPI.h>          // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EthernetUdp.h>  // UDP library from: bjoern@cs.stanford.edu 12/30/2008
#include <string.h>
#include <Arduino.h>

#define RSTn 9  //WIZ850io resetpin
#define SCNn 10  //WIZ850io SCNn
#define ANALOG_WRITE_RESOLUTION_MAX 4095
#define LASER_OUTPUT_PIN 2  //pwmpin
#define LASER_ON_VAL 255
#define LASER_OFF_VAL 0
#define LASER_ON "laser_on"
#define LASER_OFF "laser_off"
#define IMAGE_SCALE_X 480
#define IMAGE_SCALE_Y 480
#define X_GAlVO A21
#define Y_GAlVO A22

//#define DEBUG //デバッグ時に記載コメントアウトする事によってifdef解除

/*グローバル変数*/
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168,11,177);

unsigned int localPort = 8888;              // local port to listen on

// buffers for receiving and sending data
char PacketBuffer[UDP_TX_PACKET_MAX_SIZE];  //buffer to hold incoming packet,
char ReplyBuffer[] = "acknowledged";       // a string to send back

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

void setupToWIZ850io(){
  pinMode(RSTn, OUTPUT);
  digitalWrite(RSTn, LOW);    // begin reset the WIZ850io
  pinMode(SCNn, OUTPUT);
  digitalWrite(SCNn, HIGH);  // de-select WIZ850io
  digitalWrite(RSTn, HIGH);   // end reset pulse
}

void setup() {
  setupToWIZ850io();
  analogWriteResolution(8);  //pwmの分解能が0～255に変更される。
  // start the Ethernet and UDP:
  Ethernet.begin(mac,ip);
  Udp.begin(localPort);
  #ifdef DEBUG
    Serial.begin(9600);
  #endif
  // put your setup code here, to run once:
}

//Udp通信にて受信を行い、PacketBufferに受信したstringを格納する
void udpReciver(){
  int packetSize = Udp.parsePacket();
  for(unsigned i=0; i<sizeof(PacketBuffer);i++){
      PacketBuffer[i]='\0';  //グローバル変数のBufferを空ににする。
  };
  if(packetSize)
  {
    #ifdef DEBUG
      Serial.print("Received packet of size ");
      Serial.println(packetSize);
      Serial.print("From ");
      IPAddress remote = Udp.remoteIP();
    #endif
    for (int i =0; i < 4; i++)
    {
      #ifdef DEBUG
        Serial.print(remote[i], DEC);
        if (i < 3)
        {
          Serial.print(".");
        }
      #endif
    }
    #ifdef DEBUG
      Serial.print(", port ");
      Serial.println(Udp.remotePort());
    #endif
    // read the packet into packetBufffer
    Udp.read(PacketBuffer,UDP_TX_PACKET_MAX_SIZE);
    #ifdef DEBUG
      Serial.println("Contents:");
      Serial.println(PacketBuffer);
    #endif
    // send a reply, to the IP address and port that sent us the packet we received
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(ReplyBuffer);
    Udp.endPacket();
  }
}

//PacketBufferに基づきGalvos電圧の調整、LaserのON/OFFを行う。
void bufferOperation(){
  if(strlen(PacketBuffer)==0){
    return;
  }
  int brightness=int(PacketBuffer[2]);
  int xCoordinate=int(PacketBuffer[0]);
  int yCoordinate=int(PacketBuffer[1]);
  #ifdef DEBUG
    Serial.print("xCoordinate:");
    Serial.println(xCoordinate);
    Serial.print("yCoordinate:");
    Serial.println(yCoordinate);
    Serial.print("brightness:");
    Serial.println(brightness);
  #endif

  analogWrite(X_GAlVO,xCoordinate);
  analogWrite(Y_GAlVO,yCoordinate);
  analogWrite(LASER_OUTPUT_PIN,brightness);


}

void loop() {
  udpReciver();  //PacketBufferにUDPのデータを格納する。
  bufferOperation();//bufferを読み込む。
  //Teensyから電圧を出力する。

}