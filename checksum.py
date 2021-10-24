ligneHex = "1022F40047524f5550455f3039000000353230396C"
valeur=0

for i in range(len(ligneHex)//2-1):
        valeur += int(ligneHex[2*i:2*(i+1)],16)

ligneHex = ligneHex[:-2]+str(hex(255 - (valeur&255) + 1)[2:]).upper()
print(str(hex(255 - (valeur&255) + 1)[2:]))
print("\n:"+ligneHex)