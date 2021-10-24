from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives.asymmetric import ec
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric.x25519 import X25519PrivateKey
from cryptography.hazmat.primitives.asymmetric.x25519 import X25519PublicKey

import paho.mqtt.client as mqtt
import time

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    client.subscribe("/ISIMA/S7_DH/GROUPE_1337/PublicKeyIoT", qos=0)

def on_subscribe(client, userdata, mid, granted_qos):
    print("Subscribed: " + str(mid) + " " + str(granted_qos))

def on_message(client, userdata, msg):
    client.publish(topic="/ISIMA/S7_DH/GROUPE_1337/PublicKeyServer", payload=genPubKey(), qos=0)
    print("\t - Shared Key compute by Bob   : ", computeSharedKey(msg.payload).hex())

def on_publish(client, userdata, mid):
    print("Publish: " + str(mid))

def genPubKey():
    global p_key
    Bob_private_key = X25519PrivateKey.generate() # a new instance of DHPrivateKey
    Bob_public_key  = Bob_private_key.public_key()          # a new instance of DHPublicKey

    B_public_key  = Bob_public_key.public_bytes(encoding=serialization.Encoding.Raw,format=serialization.PublicFormat.Raw)
    B_private_key = Bob_private_key.private_bytes(encoding=serialization.Encoding.Raw,format=serialization.PrivateFormat.Raw,encryption_algorithm=serialization.NoEncryption())

    # # Display Public and Private information from Alice
    print("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%")
    print("Public Information from Bob and send to Alice : ")
    print("------------------------------------------------")
    print("\t - Public Key (B)   : \n", B_public_key.hex())
    print("Private Information from Bob : ")
    print("------------------------------------------------")
    print("\t - Private Key (b)  : \n", B_private_key.hex())
    p_key = Bob_private_key
    return B_public_key

def computeSharedKey(A_public_key):
    try:
        Bob_Shared_Key = p_key.exchange(X25519PublicKey.from_public_bytes(A_public_key))
    except BaseException as err :
        print(err)
    return Bob_Shared_Key

client = mqtt.Client()
client.on_connect = on_connect
client.on_subscribe = on_subscribe
client.on_message = on_message
client.on_publish = on_publish

client.connect("172.16.32.7", 1883)

client.loop_forever()