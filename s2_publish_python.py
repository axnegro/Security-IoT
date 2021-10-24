import paho.mqtt.client as mqtt
import time

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    client.subscribe("isima/SECRET_99999/G9/#", qos=0)

def on_subscribe(client, userdata, mid, granted_qos):
    print("Subscribed: " + str(mid) + " " + str(granted_qos))
    client.will_set(topic="isima/SECRET_99999/G9", payload="J'ai fini", qos=0, retain=False)

def on_message(client, userdata, msg):
    print(msg.topic+" "+str(msg.payload))

def on_publish(client, userdata, mid):
    print("Publish: " + str(mid))

def publication():
    for i in range(0,10):
        (rc, mid) = client.publish(topic="isima/SECRET_99999/G9/LEDs/LED1", payload="ON", qos=0)
        time.sleep(1)
        (rc, mid) = client.publish(topic="isima/SECRET_99999/G9/LEDs/LED1", payload="OFF", qos=0)
        time.sleep(1)
#isima/G8/LEDs/LED1/MAXIKEANYTEST
#           /BPs/BP1
client = mqtt.Client()
client.username_pw_set("SuperV", password="bateauV")
client.on_connect = on_connect
client.on_subscribe = on_subscribe
#client.on_message = on_message
client.on_publish = on_publish

client.connect("172.16.32.7", 5209)

client.loop_start()

try:
    publication()
except:
    print("Impossible de publier")