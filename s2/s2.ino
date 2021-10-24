#include <SPI.h>
#include <Countdown.h>
#include <IPStack.h>
#include <MQTTClient.h>
#include <WiFiEsp.h>

// Variables => pins des input et output
const int led_D5 = 13;
const int led_D6 = 12;
const int bouton_S2 = 8;
const int bouton_S3 = 9;
bool ChangeLed_D5=false, ChangeLed_D6=false;
bool Bp_S2 =true, Bp_S3 =true;

WiFiEspClient c;
IPStack ipstack(c);
MQTT::Client<IPStack, Countdown> client = MQTT::Client<IPStack, Countdown>(ipstack);

// Noms des topics
String subLED  = "\x0e\x6a\x0e\x74\x06\x16\x13\x69\x18\x0c\x68\x7e\x7e\x16\x0b\x7c\x03\x4a\x68\x1a";
String subBP   = "\x0e\x6a\x0e\x74\x06\x16\x13\x69\x18\x0c\x68\x7e\x7e\x16\x05\x69\x34\x16\x64";
String pubBP1  = "\x0e\x6a\x0e\x74\x06\x16\x13\x69\x18\x0c\x68\x7e\x7e\x16\x05\x69\x34\x16\x05\x69\x76";
String pubBP2  = "\x0e\x6a\x0e\x74\x06\x16\x13\x69\x18\x0c\x68\x7e\x7e\x16\x05\x69\x34\x16\x05\x69\x75";
String pubLED1 = "\x0e\x6a\x0e\x74\x06\x16\x13\x69\x18\x0c\x68\x7e\x7e\x16\x0b\x7c\x03\x4a\x68\x75\x02\x7d\x76";
String pubLED2 = "\x0e\x6a\x0e\x74\x06\x16\x13\x69\x18\x0c\x68\x7e\x7e\x16\x0b\x7c\x03\x4a\x68\x75\x02\x7d\x75";

String key = "G9";
  
void CallBackBpMsg(MQTT::MessageData& md){
  MQTT::Message &message = md.message;
  MQTTString &topic = md.topicName;

  Serial.print("Message BP arrived: qos ");
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
  Serial.println("LED1_ : "+ChangeLed_D6?"true":"false");
  Serial.println("LED2_ : "+ChangeLed_D5?"true":"false");
  Serial.print("Payload ");
  Serial.println(msgPL);
  
  
  if(!strncmp(&topic.lenstring.data[topic.lenstring.len-3],"BP1",3)){
    if(!strncmp(msgPL,"ON",2)){
      ChangeLed_D6=true;
    }
  }
  if(!strncmp(&topic.lenstring.data[topic.lenstring.len-3],"BP2",3)){
    if(!strncmp(msgPL,"ON",2)){
      ChangeLed_D5=true;
    }
  }
  
  delete msgPL;
  delete topicName;
}

void CallBackLedMsg(MQTT::MessageData& md){
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
  Serial.println("LED1 : "+ChangeLed_D6?"true":"false");
  Serial.println("LED2 : "+ChangeLed_D5?"true":"false");
  if (!strncmp(&topic.lenstring.data[topic.lenstring.len-4],"LED1",4)){
    if (!strncmp(msgPL,"ON",2))
      digitalWrite(led_D6,HIGH);
    else
      digitalWrite(led_D6,LOW);
  }      
  if (!strncmp(&topic.lenstring.data[topic.lenstring.len-4],"LED2",4)){
    if (!strncmp(msgPL,"ON",2))
      digitalWrite(led_D5,HIGH);
    else
      digitalWrite(led_D5,LOW);
  }
  delete msgPL;
  delete topicName;
}

void WifiConnect(){
  Serial1.begin(9600);
  while(!Serial1);
    Serial.begin(9600);
  while(!Serial);
  WiFi.init(&Serial1);
  WiFi.begin((char*)"ZZ_HSH","WIFI_ZZ_F5");
  Serial.println(WiFi.localIP());
}

void BrokerConnect(){
  MQTTPacket_connectData configMQTT = MQTTPacket_connectData_initializer;
 
  configMQTT.clientID.cstring = (char*)"Groupe_9";
  //configMQTT.username.cstring = (char*)"Arduinolo";
  //configMQTT.password.cstring = (char*)"bateau";
  configMQTT.willFlag = 0;
  ipstack.connect((char *)"192.168.1.136",1883);
  int rc = client.connect(configMQTT);
  if(rc == 0)
    Serial.println("Connected OK");
  else
    Serial.println("Not Connected ERROR");

  Serial.println("Subscribe to LED : ");
  client.subscribe(Xor(subLED, key).c_str(),MQTT::QOS0, CallBackLedMsg);
  Serial.println("Subscribe to BP : ");
  client.subscribe(Xor(subBP, key).c_str(), MQTT::QOS0, CallBackBpMsg);
}
void PortsSetup(){
  // initialisation des broches 12 et 13 comme étant des sorties
  pinMode(led_D5, OUTPUT);
  pinMode(led_D6, OUTPUT);
  // initialisation des broches 8 et 9 comme étant des entrées
  pinMode(bouton_S2, INPUT);
  pinMode(bouton_S3, INPUT);
}
void setup() {
  WifiConnect();
  BrokerConnect();
  PortsSetup();
}

void loop() {
  client.yield(100);
  if (Bp_S2 != digitalRead(bouton_S2)){
    Bp_S2 = !Bp_S2;
    MQTT::Message message;
    message.qos = MQTT::QOS0;
    message.retained = false;
    message.payload = (void *)(Bp_S2?"OFF":"ON");
    message.payloadlen = strlen(Bp_S2?"OFF":"ON");
    client.publish(Xor(pubBP1, key).c_str(),message);
  }
  if (Bp_S3 != digitalRead(bouton_S3)){
    Bp_S3 = !Bp_S3;
    MQTT::Message message;
    message.qos = MQTT::QOS0;
    message.retained = false;
    message.payload = (void *)(Bp_S3?"OFF":"ON");
    message.payloadlen = strlen(Bp_S3?"OFF":"ON");
    client.publish(Xor(pubBP2, key).c_str(),message);
  }
  if(ChangeLed_D6){
    MQTT::Message message;
    message.qos = MQTT::QOS0;
    message.retained = true;
    message.payload = (void *)(!digitalRead(led_D6)?"ON":"OFF");
    message.payloadlen = strlen(!digitalRead(led_D6)?"ON":"OFF");
    client.publish(Xor(pubLED1, key).c_str(),message);
    ChangeLed_D6=false;
  }
  if(ChangeLed_D5){
    MQTT::Message message;
    message.qos = MQTT::QOS0;
    message.retained = true;
    message.payload = (void *)(!digitalRead(led_D5)?"ON":"OFF");
    message.payloadlen = strlen(!digitalRead(led_D5)?"ON":"OFF");
    client.publish(Xor(pubLED2, key).c_str(),message);
    ChangeLed_D5=false;
  }
}

String Xor(String in, String key) {
  String res = in;
  for(unsigned int i = 0; i < in.length(); i++){
    res[i] = (in[i] ^ key[i % key.length()]);
  }
  Serial.println("res : "+res);
  return res;
}

void serialEvent(){
  Serial1.write(Serial.read());
  digitalWrite(12,!digitalRead(12));
}
void serialEvent1(){
  Serial.write(Serial1.read());
  digitalWrite(13,!digitalRead(13));
}
