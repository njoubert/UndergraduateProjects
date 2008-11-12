import sys
filename = sys.argv[1]
split = filename.split(".")
name = split[0]
ext = split[1]
z = 0
maxFile = 0
while True:
    try:
        curFile = file(name + str(maxFile) + "." + ext, "r")
        curFile.close()
    except:
        break
    maxFile += 1
output = "Graphics3D[{EdgeForm[],{{"
while True:
    try:
        curFile = file(name + str(z) + "." + ext, "r")
        lines = curFile.readlines()
        curFile.close()
        x = 0
        for line in lines:
            y = 0
            for entry in line.split():
                if entry == "1":
                    colorPiece = 1.0 * z / maxFile
                    r = colorPiece
                    g = colorPiece
                    b = 1-colorPiece
                    first = "{FaceForm[RGBColor["+str(r)+", "+str(g)+", "+str(b)+"], RGBColor["+str(r)+", "+str(g)+", "+str(b)+"]],"
                    output += first
                    output += "Polygon[{{"
                    output += str(x) + ", " + str(y) + ", " + str(z)
                    output += "},{"
                    output += str(x+1) + ", " + str(y) + ", " + str(z)
                    output += "},{"
                    output += str(x+1) + ", " + str(y+1) + ", " + str(z)
                    output += "},{"
                    output += str(x) + ", " + str(y+1) + ", " + str(z)
                    output += "}}]},"
                    output += first
                    output += "Polygon[{{"
                    output += str(x) + ", " + str(y) + ", " + str(z)
                    output += "},{"
                    output += str(x+1) + ", " + str(y) + ", " + str(z)
                    output += "},{"
                    output += str(x+1) + ", " + str(y) + ", " + str(z+1)
                    output += "},{"
                    output += str(x) + ", " + str(y) + ", " + str(z+1)
                    output += "}}]},"
                    output += first
                    output += "Polygon[{{"
                    output += str(x) + ", " + str(y) + ", " + str(z)
                    output += "},{"
                    output += str(x) + ", " + str(y) + ", " + str(z+1)
                    output += "},{"
                    output += str(x) + ", " + str(y+1) + ", " + str(z+1)
                    output += "},{"
                    output += str(x) + ", " + str(y+1) + ", " + str(z)
                    output += "}}]},"
                    output += first
                    output += "Polygon[{{"
                    output += str(x) + ", " + str(y) + ", " + str(z+1)
                    output += "},{"
                    output += str(x+1) + ", " + str(y) + ", " + str(z+1)
                    output += "},{"
                    output += str(x+1) + ", " + str(y+1) + ", " + str(z+1)
                    output += "},{"
                    output += str(x) + ", " + str(y+1) + ", " + str(z+1)
                    output += "}}]},"
                    output += first
                    output += "Polygon[{{"
                    output += str(x) + ", " + str(y+1) + ", " + str(z)
                    output += "},{"
                    output += str(x+1) + ", " + str(y+1) + ", " + str(z)
                    output += "},{"
                    output += str(x+1) + ", " + str(y+1) + ", " + str(z+1)
                    output += "},{"
                    output += str(x) + ", " + str(y+1) + ", " + str(z+1)
                    output += "}}]},"
                    output += first
                    output += "Polygon[{{"
                    output += str(x+1) + ", " + str(y) + ", " + str(z)
                    output += "},{"
                    output += str(x+1) + ", " + str(y) + ", " + str(z+1)
                    output += "},{"
                    output += str(x+1) + ", " + str(y+1) + ", " + str(z+1)
                    output += "},{"
                    output += str(x+1) + ", " + str(y+1) + ", " + str(z)
                    output += "}}]},"
                    output += "\r\n"
                y += 1
            x += 1
    except:
        z += 1
        break
    z += 1
output = output[:-3]
output += "}}},{Lighting -> False, Background -> GrayLevel[0]}]"
outFile = file("graphed.m", "w")
outFile.write(output)
outFile.close()
