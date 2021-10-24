import paho.mqtt.client as mqtt
import time

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    client.subscribe("isima/GX", qos=0)

def on_subscribe(client, userdata, mid, granted_qos):
    print("Subscribed: " + str(mid) + " " + str(granted_qos))
    client.will_set(topic="isima/GX", payload="J'ai fini", qos=0, retain=False)

def on_message(client, userdata, msg):
    print(msg.topic+" "+str(msg.payload))

def on_publish(client, userdata, mid):
    print("Publish: " + str(mid))

def publication():
    for i in range(0,10):
        (rc, mid) = client.publish(topic="isima/GX", payload="TEST_GX "+str(i+1), qos=0, retain=i%2)
        time.sleep(1)


client = mqtt.Client()
client.on_connect = on_connect
client.on_subscribe = on_subscribe
#client.on_message = on_message
client.on_publish = on_publish

client.connect("localhost", 1883)

client.loop_start()

try:
    publication()
except:
    print("Impossible de publier")