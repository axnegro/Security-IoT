import paho.mqtt.client as mqtt
import time

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    client.subscribe("isima/SECRET_99999/G9/#", qos=0)

def on_subscribe(client, userdata, mid, granted_qos):
    print("Subscribed: " + str(mid) + " " + str(granted_qos))

def on_message(client, userdata, msg):
    #if (msg.retain==True):
    print(msg.topic+" "+str(msg.payload))

client = mqtt.Client()
client.username_pw_set("SuperV", password="bateauV")
client.on_connect = on_connect
client.on_subscribe = on_subscribe
client.on_message = on_message

client.connect("172.16.32.7", 5209)

client.loop_forever()
