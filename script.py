import sys
import paho.mqtt.client as mqtt
import time
import re
import os

config = {}
client = mqtt.Client()

def replace(filename, valueold, valuenew, newfile):
    lines = getlines(filename)
    indexTab = []
    for i, line in enumerate(lines):
        if(line[7:9] == "00"):
            indexTab.append((i,len(line[9:len(line)-3])))
    #print(indexTab)
    data = setupLinesHex(lines).upper()
    #print(data)
    hexold = asciitohex(valueold)
    hexnew = asciitohex(valuenew)
    indexFind = data.find(hexold)
    if(indexFind != -1):
        print(data[indexFind])
        if(len(hexold) >= len(hexnew)):
            #fill avec 0
            tmpindex = 0
            indexindextab = 0
            while(tmpindex <= indexFind):
                tmpindex += indexTab[indexindextab][1]
                indexindextab += 1
            changed = 0
            
            indexindextab -= 1
            indexlinetochange = indexTab[indexindextab][0]
            
            start = indexTab[indexindextab][1] - (tmpindex - indexFind) + 9
            
            while(changed<len(hexold)):
                if(start<len(lines[indexlinetochange])-3):
                    if(changed<len(hexnew)):
                        if(start+1<len(lines[indexlinetochange])-1):
                            lines[indexlinetochange] =  lines[indexlinetochange][0:start] + hexnew[changed] + lines[indexlinetochange][start+1:]
                        else:
                            lines[indexlinetochange] =  lines[indexlinetochange][0:start] + hexnew[changed]
                    else:
                        if(start+1<len(lines[indexlinetochange])-1):
                            lines[indexlinetochange] =  lines[indexlinetochange][0:start] + "0" + lines[indexlinetochange][start+1:]
                        else:
                            lines[indexlinetochange] =  lines[indexlinetochange][0:start] + "0"
                    changed += 1
                    start += 1
                else:
                    indexindextab += 1
                    indexlinetochange = indexTab[indexindextab][0]
                    start = 9
        else :
            
            #le cas chiant + recalcule des adr
            b=4
        
        resfile = open(newfile, 'w')
        resfile.writelines(lines)
        resfile.close()
        checksumFile(newfile)

def checkAdress(filename):
    lines = getlines(filename)
    out = open(filename, 'w')
    linesVerif = []
    adr = lines[1][3:7]
    prec = 0
    for line in lines:
        if (line[7:9]=="00"):
            lineAdr = hex(int(adr,16)+prec)
            prec = int(line[1:3], 16)
            adr = str(lineAdr[2:])
            if (len(adr)>=5) : adr = adr[1:]
            linesVerif.append(line[:3]+adr+line[7:])
        else:
            linesVerif.append(line)
    out.writelines(linesVerif)
    out.close()

def getlines(filename):
    file = open(filename, 'r')
    lines = file.readlines()
    file.close()
    return lines

def setupLinesHex(lines):
    filteredlines = list(filter(lambda x : x[7:9] == "00" , lines))
    mapedlines = list(map(lambda x : x[9:len(x)-3], filteredlines))
    res = []
    bloc = ""
    for i in mapedlines:
        bloc+=i
    return bloc

def setup(lines):
    filteredlines = list(filter(lambda x : x[7:9] == "00" , lines))
    mapedlines = list(map(lambda x : x[9:len(x)-3], filteredlines))
    res = []
    bloc = ""
    for i in mapedlines:
        bloc+=i
    for i in range(0, len(bloc), 2):
        res.append(int(bloc[i:i+2], 16))
    #print(res)
    return res


def decodeline(line, min):
    shouldbetopic = []
    shouldbeIp = []
    cpt = 0
    res = ""
    tmpCpt = 0
    accept = False
    while(cpt < len(line)):
        cr = chr(int(line[cpt]))
        if(cr.isprintable()):
            tmpCpt +=1
            res += cr
        else:
            if(tmpCpt >= min):
                print(res)
                if(res.count("/") >  1 and len(res) > 4):
                    shouldbetopic.append(res)
                if(res.count(".") > 0 and len(res) > 4):
                    shouldbeIp.append(res)
            tmpCpt = 0
            res = ""
        cpt += 1
    print("Topic and Ip detect :")
    print(shouldbetopic)
    print(shouldbeIp)
    #print(res)

def asciitohex(ascii):
    hexa = ""
    for ch in ascii:
            hexa += hex(ord(ch))[2:]
    print(hexa)
    return hexa.upper()

def hextoascii(hex):
    if (hex[1]=='x'):
        hex = hex[2:]
    print(bytes.fromhex(hex).decode())

def extract(filename, min=4):
    lines = getlines(filename)
    setuplines = setup(lines)
    c=0
    decodeline(setuplines, min)

def on_message(client, userdata, msg):
    #if (msg.retain==True):
    print(msg.topic+" "+str(msg.payload))

def readtopic(topic):
    client.on_message = on_message
    client.subscribe(topic, qos=0)
    time.sleep(10000)

def checksum(line):
    valeur=0
    if (line[0]==":"): line = line[1:]
    for i in range(len(line)//2-1):
        valeur += int(line[2*i:2*(i+1)],16)
    check = str(hex(255 - (valeur&255) + 1)[2:]).upper()
    if (len(check)==1): check = "0" + check
    line = line[:-3]+check[-2:]
    return line

def checksumFile(file):
    lines = getlines(file)
    out = open(file, 'w')
    linesVerif = []
    for line in lines:
        line = checksum(line)
        linesVerif.append(":"+line+"\n")
    out.writelines(linesVerif)
    out.close()

def loadConfig():
    file = open("config.txt")
    config.clear()
    lines = file.readlines()
    for line in lines:
        splitedline = line.split("=")
        config[splitedline[0]] = splitedline[1][0:-1]
    print("config :" )
    print(config)
    if(config["clientid"] != ""):
        client = mqtt.Client(client_id=config["clientid"])
    connect()

def connect():
    #client.on_connect = on_connect
    #client.on_subscribe = on_subscribe
    #client.on_message = on_message
    #client.on_publish = on_publish
    if(config["password"] != "" and config["username"] != ""):
        client.username_pw_set(config["username"], password=config["password"])

    client.connect(config["brokerAdr"], int(config["brokerPort"]))

    client.loop_start()

def publish(topic, payload):
    print("send on topic : " + topic)
    print("send on payload : " + payload)
    client.publish(topic=topic, payload=payload, qos=0)
    time.sleep(0.2)

def televers(com, fileinput):
    genbincmd = config["genbin"]
    genbincmd = genbincmd.replace("input", fileinput)
    genbincmd = genbincmd.replace("output", fileinput+".bin")
    televerscmd = config["televers"]
    televerscmd = televerscmd.replace("com", com)
    televerscmd = televerscmd.replace("fbin", fileinput+".bin")
    os.system(genbincmd)
    os.system(televerscmd)

def readChoice(prechoice):
    if(prechoice != ""):
        choice = prechoice
    else:

        choice = input(""" 
# -p <topic> <payload> : Publish (fait)
# -e <file> (<min len> default=4) : Extract (fait)
# -hex <ascii> : Retourne la valeur hex de l'ascii (fait)
# -ascii <hex> : Retourne la valeur ascii de l'hex (fait)
# -c <file> : Calcul Checksum et modif (fait)
# -a <file> : Calcul Adresse et modif
# -t <.hex> : Téléverser (n)
# -l <topic> : Listen topic (fait)
# -lc load config file (fait)
# -r <filesource> <val to replace> <replace value> <file dest> : remplace dans un .hex (fait)
# -a <file> : Calcul Adresse et modif
# -d : exec default
"""
    )
    splitedChoice = choice.split(" ")
    print(splitedChoice)
    if(choice.startswith("-p")):
        publish(splitedChoice[1], splitedChoice[2])
    if(choice.startswith("-e")):
        if(len(splitedChoice) > 2):
            extract(splitedChoice[1], int(splitedChoice[2]))
        else:
            extract(splitedChoice[1])
    if(choice.startswith("-hex")):
        asciitohex(splitedChoice[1])
    if(choice.startswith("-ascii")):
        hextoascii(splitedChoice[1])
    if(choice.startswith("-c")):
        checksumFile(splitedChoice[1])
    if(choice.startswith("-a")):
        checkAdress(splitedChoice[1])
    if(choice == "-d"):
        publish(config["defaulttopic"], config["defaultpayload"])
    if(splitedChoice[1] == "-r"):
        if(len(asciitohex(splitedChoice[3]))<= len(asciitohex(splitedChoice[4]))):
            replace(splitedChoice[2], splitedChoice[3], splitedChoice[4], splitedChoice[5])
        else:
            print("opération non suporté")
        
    if(splitedChoice[0] == "-lc"):
        loadConfig()
    if(splitedChoice[0] == "-l"):
        readtopic(splitedChoice[1])
    if(splitedChoice[0] == "-t"):
        readtopic(splitedChoice[1], splitedChoice[2])

loadConfig()
if(len(sys.argv) > 1):
    readChoice(' '.join(sys.argv[1:]))
else:
    while(True):
        readChoice("")