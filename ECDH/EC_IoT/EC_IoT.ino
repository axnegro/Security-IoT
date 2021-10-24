#include <Countdown.h>
#include <IPStack.h>
#include <MQTTClient.h>

#include <WiFiEsp.h>

#include <Curve25519.h>

#define MY_DEBUG

#define WIFI_SSID (char*)"ZZ_HSH"
#define WIFI_PWD "WIFI_ZZ_F5"

#define MQTT_SERVER (char*)"192.168.1.136"
#define MQTT_PORT 1883
#define MQTT_CLIENTID (char*)"GROUPE_1337"

const int MAX_MQTT_PACKET_SIZE = 500;

// Variables => input and output pins
const int led_D5    = 13;
const int led_D6    = 12;
const int bouton_S2 = 8;
const int bouton_S3 = 9;

// Topic Names
String subIoT  = "/ISIMA/S7_DH/GROUPE_1337/PublicKeyIoT";
String subServer  = "/ISIMA/S7_DH/GROUPE_1337/PublicKeyServer";

// Global parameters for events on device
bool ChangeLed_D5=false, ChangeLed_D6=false;
bool Bp_S2 =true, Bp_S3 =true;

//Elliptic Curve Algo
uint8_t private_key[32];
uint8_t public_key[32];

// IP Stack and MQTT Client for communicate with broker
WiFiEspClient c;
IPStack ipstack(c);
MQTT::Client<IPStack, Countdown, MAX_MQTT_PACKET_SIZE > client = MQTT::Client<IPStack, Countdown, MAX_MQTT_PACKET_SIZE >(ipstack);

// Initialisation for Serial connexions (USB connexion between Arduino and PC => serial monitor of IDE)
void SerialConnect()
{
  #ifdef MY_DEBUG
    Serial.begin(9600);
    while(!Serial);
    Serial.println("--- Serial Initialisation (USB): OK ---");
  #endif 
}

// Initialisation for Wifi connexion ( Serial1 )
void WifiConnect()
{
  Serial1.begin(9600);
  while(!Serial1); 
  
  #ifdef MY_DEBUG 
   Serial.println("--- Serial1 Initialisation (ESP8266): OK ---");
  #endif    

  WiFi.init(&Serial1);
  WiFi.begin(WIFI_SSID,WIFI_PWD);
     
  #ifdef MY_DEBUG 
   Serial.println("--- Wifi Connexion on gateway: OK ---");
  #endif   
}

// Connexion to MQTT Broker
void BrokerConnect()
{
  ipstack.connect(MQTT_SERVER,MQTT_PORT);

  MQTTPacket_connectData configMQTT = MQTTPacket_connectData_initializer;
  configMQTT.clientID.cstring = MQTT_CLIENTID;  
  
  int rc = client.connect(configMQTT);
  
  #ifdef MY_DEBUG
   if (rc==0)
    Serial.println("--- MQTT Broker Connexion: OK ---");
   else
   {
    Serial.print("--- MQTT Broker Connexion: KO ---");
    Serial.print("Error connexion: ");Serial.println(rc);    
   }
  #endif  

  client.subscribe(subServer.c_str(), MQTT::QOS0, CallBackBpMsg);
 }

 
// Arduino ports setup
void PortsSetup()
{
  // initialisation des broches 12 et 13 comme étant des sorties
  pinMode(led_D5, OUTPUT);
  pinMode(led_D6, OUTPUT);
  // initialisation des broches 8 et 9 comme étant des entrées
  pinMode(bouton_S2, INPUT);
  pinMode(bouton_S3, INPUT);

  #ifdef MY_DEBUG 
   Serial.println("--- GPIO Initilisation: OK ---");
  #endif     
}

// Callback for event on buttons
void CallBackBpMsg(MQTT::MessageData& md)
{
  MQTT::Message &message = md.message;
  MQTTString &topic      = md.topicName;
  
  #ifdef MY_DEBUG
   Serial.println("--- MQTT Callback for button events ---");
   Serial.print(  "Quality of Service ( QoS ): "); Serial.println(message.qos); 
   Serial.print(  "Retained message          : "); Serial.println(message.retained);
   Serial.print(  "Duplicate                 : "); Serial.println(message.dup);
   Serial.print(  "Message Identifier        : "); Serial.println(message.id);
  #endif 

  char* topicName = new char[topic.lenstring.len+1]();
  memcpy(topicName,topic.lenstring.data,topic.lenstring.len);
  
  #ifdef MY_DEBUG
   Serial.print(  "Topic name                : "); Serial.println(topicName);  
  #endif 

  Print_Debug_HEX("payload               : ",(byte*) message.payload, 32); 
  uint8_t tmp[32];
  memcpy(tmp, message.payload, 32);
  uint8_t p_key[32];
  memcpy(p_key, private_key, 32);
  
  if (!Curve25519::dh2(tmp, p_key)){
    Print_Debug_HEX("Erreur Key               : ",(byte*) tmp, 32);  
  }else{   
    Print_Debug_HEX("Shared Key               : ",(byte*) tmp, 32);  
  }
  
  delete[] topicName;
}

// Debug function => print value in HEX format
void Print_Debug_HEX(String title, byte* data, int data_length)
{
 #ifdef MY_DEBUG
  Serial.print(title) ;
  for(int i = 0; i< data_length; ++i)
  {
   byte val = data[i];
   Serial.print (val>>4, HEX) ; Serial.print (val&15, HEX) ; Serial.print (" ") ; 
  }
  Serial.println() ;
 #endif  
}

void setup() {
  // put your setup code here, to run once:
  SerialConnect();
  WifiConnect();
  BrokerConnect();
  PortsSetup();

  // Generate the private_key and the public_key.
  Curve25519::dh1(public_key, private_key);
}

void loop() {
  // put your main code here, to run repeatedly:
  client.yield(100);

  if (Bp_S2 != digitalRead(bouton_S2))
  {    
    Bp_S2 = !Bp_S2;

    MQTT::Message message;
    message.qos = MQTT::QOS0;
    message.retained = false;
    message.payload = (void *) public_key;
    message.payloadlen = 32;
    int err = client.publish(subIoT.c_str(),message);
    #ifdef MY_DEBUG
     Serial.println("--- MQTT Publish Message ---");
     Serial.print(  "Return code : "); Serial.print(err); Serial.println(" (BUFFER_OVERFLOW = -2, FAILURE = -1, SUCCESS = 0)");  
     Serial.println( "");  
    #endif    
  }
}
