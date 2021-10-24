#include <Base64.h>

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
String subLED  = "isima/TP_5/G9/LEDs/#";
String subBP   = "isima/TP_5/G9/BPs/#";
String pubBP1  = "isima/TP_5/G9/BPs/BP1";
String pubBP2  = "isima/TP_5/G9/BPs/BP2";
String pubLED1 = "isima/TP_5/G9/LEDs/LED1";
String pubLED2 = "isima/TP_5/G9/LEDs/LED2";

String key = "A_DEFINIR";

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
  Serial.print("Payload ");
  Serial.println(msgPL);

  char buff[64];
  base64_decode(buff, msgPL, strlen(msgPL));

  const char * res = Xor(buff, key).c_str();
  
  if(!strncmp(&topic.lenstring.data[topic.lenstring.len-3],"BP1",3)){
    if(!strncmp(res,"ON",2)){
      ChangeLed_D6=true;
    }
  }
  if(!strncmp(&topic.lenstring.data[topic.lenstring.len-3],"BP2",3)){
    if(!strncmp(res,"ON",2)){
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

  char buff[64];
  base64_decode(buff, msgPL, strlen(msgPL));

  const char * res = Xor(buff, key).c_str();
  
  if (!strncmp(&topic.lenstring.data[topic.lenstring.len-4],"LED1",4)){
    if (!strncmp(res,"ON",2))
      digitalWrite(led_D6,HIGH);
    else
      digitalWrite(led_D6,LOW);
  }
  if (!strncmp(&topic.lenstring.data[topic.lenstring.len-4],"LED2",4)){
    if (!strncmp(res,"ON",2))
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
  
  configMQTT.clientID.cstring = (char*)"GROUPE_09";
  //configMQTT.username.cstring = (char*)"GX";
  //configMQTT.password.cstring = (char*)"...";
  configMQTT.willFlag = 0;
  ipstack.connect((char *)"192.168.1.136",1883);
  int rc = client.connect(configMQTT);
  if(rc == 0)
    Serial.println("Connected OK");
  else
    Serial.println("Not Connected ERROR");
  client.subscribe(subBP.c_str(), MQTT::QOS0, CallBackBpMsg);
  client.subscribe(subLED.c_str(),MQTT::QOS0, CallBackLedMsg);
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

    //Encode
    char buff[64];

    int inputLen = strlen(Bp_S2?"ON":"OFF");
    int encode = base64_encode(buff,(char *)(Bp_S2?Xor("ON", key):Xor("OFF", key)).c_str(), inputLen);
    
    message.payload = (void *)(buff);
    message.payloadlen = encode;
    client.publish(pubBP1.c_str(),message);
  }
  if (Bp_S3 != digitalRead(bouton_S3)){
    Bp_S3 = !Bp_S3;
    MQTT::Message message;
    message.qos = MQTT::QOS0;
    message.retained = false;

    //Encode
    char buff[64];
    int inputLen = strlen(Bp_S3?"ON":"OFF");
    int encode = base64_encode(buff,(char *)(Bp_S3?Xor("ON", key):Xor("OFF", key)).c_str(), inputLen);
    
    message.payload = (void *)(buff);
    message.payloadlen = encode;
    
    client.publish(pubBP2.c_str(),message);
  }
  if(ChangeLed_D6){
    MQTT::Message message;
    message.qos = MQTT::QOS0;
    message.retained = true;

    //Encode
    char buff[64];
    int inputLen = strlen(!digitalRead(led_D6)?"ON":"OFF");
    int encode = base64_encode(buff,(char *)(!digitalRead(led_D6)?Xor("ON", key):Xor("OFF", key)).c_str(), inputLen);
    
    message.payload = (void *)(buff);
    message.payloadlen = encode;
    client.publish(pubLED1.c_str(),message);
    ChangeLed_D6=false;
  }
  if(ChangeLed_D5){
    MQTT::Message message;
    message.qos = MQTT::QOS0;
    message.retained = true;
    
    //Encode
    char buff[64];
    int inputLen = strlen(!digitalRead(led_D5)?"ON":"OFF");
    int encode = base64_encode(buff,(char *)(!digitalRead(led_D5)?Xor("ON", key):Xor("OFF", key)).c_str(), inputLen);
    
    message.payload = (void *)(buff);
    message.payloadlen = encode;
    client.publish(pubLED2.c_str(),message);
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
