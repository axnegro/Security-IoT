#include <Countdown.h>
#include <IPStack.h>
#include <MQTTClient.h>

#include <WiFiEsp.h>
#include <WiFiEspClient.h>
#include <WiFiEspServer.h>
#include <WiFiEspUdp.h>

const int boutonS2 = 8;
const int boutonS3 = 9;
const int ledD5 = 13;
const int ledD6 = 12;

String subLED  = "\x0e\x6a\x0e\x74\x06\x16\x13\x69\x18\x0c\x68\x7e\x7e\x16\x0b\x7c\x03\x4a\x68\x1a";
String pubLED2 = "\x0e\x6a\x0e\x74\x06\x16\x13\x69\x18\x0c\x68\x7e\x7e\x16\x0b\x7c\x03\x4a\x68\x75\x02\x7d\x75";

String key = "G9";
int keySize = 2;

WiFiEspClient c;
IPStack ipstack(c);
MQTT::Client<IPStack, Countdown> client = MQTT::Client<IPStack, Countdown>(ipstack);

void setup() {
  Serial.begin(9600);
  while(!Serial);
  pinMode (LED_BUILTIN,OUTPUT);
  pinMode(boutonS2,INPUT);
  pinMode(boutonS3,INPUT);
  pinMode(ledD5,OUTPUT);
  pinMode(ledD6,OUTPUT);
  WifiConnect();
  BrokerConnect("192.168.1.136", 1883);
}
void loop() {
}


void WifiConnect()
{
  Serial1.begin(9600);
  while(!Serial1);
  WiFi.init(&Serial1);
  WiFi.begin((char*)"ZZ_HSH","WIFI_ZZ_F5");
}

void BrokerConnect(char* url, int port){
  MQTTPacket_connectData configMQTT = MQTTPacket_connectData_initializer;
  configMQTT.clientID.cstring = (char*)"Groupe_9";
  //configMQTT.username.cstring = (char*)"Arduinolo";
  //configMQTT.password.cstring = (char*)"bateau";
  configMQTT.willFlag = 0;
  
  //Connection ip
  ipstack.connect(url, port);
  
  int rc = client.connect(configMQTT);
  if(rc == 0)
    Serial.println("Connected OK");
  else
    Serial.println("Not Connected ERROR");
  String topic = Xor(subLED, subLED.length(), key, keySize);
  client.subscribe(topic, 0, on_message_led);
}

void on_message_led(MQTT::MessageData& md) {
  MQTT::Message &message  = md.message;
  MQTTString &topic = md.topicName;

  Serial.print("Message LED arrived: qos ");
  Serial.print(message.qos);
  Serial.print(", retained ");
  Serial.print(message.retained);
  Serial.print(", dup ");
  Serial.print(message.dup);
  Serial.print(", packetid ");
  Serial.println(message.id);
  char* topicName = new char[topic.lenstring.len+1]();
  memcpy(topicName,topic.lenstring.data,topic.lenstring.len);
  Serial.print(", topic ");
  Serial.println(topicName);
    char* msgPL = new char[message.payloadlen+1]();
  memcpy(msgPL,message.payload,message.payloadlen);
  Serial.print("Payload ");
  Serial.println(msgPL);

  //LEDs
  if (!strncmp(&topic.lenstring.data[topic.lenstring.len-4],"LED1",4)){
    if (!strncmp(msgPL,"ON",2))
      digitalWrite(ledD6,HIGH);
    else
      digitalWrite(ledD6,LOW);
  }      
  if (!strncmp(&topic.lenstring.data[topic.lenstring.len-4],"LED2",4)){
    if (!strncmp(msgPL,"ON",2))
      digitalWrite(ledD5,HIGH);
    else
      digitalWrite(ledD5,LOW);
  }
  delete msgPL;
  delete topicName;
}

String Xor(String in, int inSize, String key, int keySize) {
  int kcpt = 0;
  String out = "";
    for(int i = 0; i < inSize; i++){
        out += in[i] ^ key[kcpt];
        kcpt ++;
        if(kcpt > keySize-1){
            kcpt = 0;
        }
    }
  return out;
}
