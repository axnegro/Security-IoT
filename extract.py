def getlines(filename):
    file = open(filename, 'r')
    lines = file.readlines()
    return lines

def setup(lines):
    filteredlines = list(filter(lambda x : x[7:9] == "00" , lines))
    mapedlines = list(map(lambda x : x[9:len(x)-3], filteredlines))
    bloc = ""
    for i in mapedlines:
        bloc+=i
    return bloc

def decodeline(line):
    a = bytes.fromhex(line)
    caracs = str(a).split('\\')
    for i in caracs:
        if(str.isprintable(str(i)) and len(str(i)) > 4 ):
            print(str(i)[3:])

def test(filename):
    lines = getlines(filename)
    setuplines = setup(lines)
    c=0
    decodeline(setuplines)

test("firmwareX.hex")