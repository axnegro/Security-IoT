from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives.asymmetric import ec
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric.x25519 import X25519PrivateKey
from cryptography.hazmat.primitives.asymmetric.x25519 import X25519PublicKey

import paho.mqtt.client as mqtt
import time

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    client.subscribe("/ISIMA/S7_DH/GROUPE_1337/PublicKeyServer", qos=0)
    client.publish(topic="/ISIMA/S7_DH/GROUPE_1337/PublicKeyIoT", payload=genPubKey(), qos=0)

def on_subscribe(client, userdata, mid, granted_qos):
    print("Subscribed: " + str(mid) + " " + str(granted_qos))
    #client.will_set(topic="isima/GX", payload="J'ai fini", qos=0, retain=False)

def on_message(client, userdata, msg):
    print("\t - Shared Key compute by Alice   : ", computeSharedKey(msg.payload).hex())

def on_publish(client, userdata, mid):
    print("Publish: " + str(mid))

def genPubKey():
    global p_key
    # Generate a private key for use in the exchange.
    Alice_private_key = X25519PrivateKey.generate()
    Alice_public_key = Alice_private_key.public_key()

    A_public_key  = Alice_public_key.public_bytes(encoding=serialization.Encoding.Raw,format=serialization.PublicFormat.Raw)
    A_private_key = Alice_private_key.private_bytes(encoding=serialization.Encoding.Raw,format=serialization.PrivateFormat.Raw,encryption_algorithm=serialization.NoEncryption())

    # Display Public and Private information from Alice
    print("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%")
    print("Public Information from Alice and send to Bob : ")
    print("------------------------------------------------")
    print("\t - Public Key (A)   : \n", A_public_key.hex())
    print("Private Information from Alice : ")
    print("------------------------------------------------")
    print("\t - Private Key (a)  : \n", A_private_key.hex())
    p_key = Alice_private_key
    return A_public_key

def computeSharedKey(B_public_key):
    Alice_Shared_Key = p_key.exchange(X25519PublicKey.from_public_bytes(B_public_key))
    return Alice_Shared_Key

client = mqtt.Client()
client.on_connect = on_connect
client.on_subscribe = on_subscribe
client.on_message = on_message
client.on_publish = on_publish

client.connect("172.16.32.7", 1883)

client.loop_forever()



