import os, sys

if(len(sys.argv) < 2):
    print(
        "Wrong number of arguments.\n"
        "Usage: python parseDirs.py studentInfoFile submissionsDir\n"
    )
    sys.exit(1)

elif(len(sys.argv) ==  2):
    path = "test/submissions/"

else:
    path = sys.argv[2]+"/"
    
students = {}

# tiedosto oletuksen mukaan formaattia (Moodle export):
# etunimi, sukunimi, sähköposti, opiskelijanumero
with open(sys.argv[1], "r", encoding="utf-8") as file:
    for line in file:
        elem = line.split(",")
        name = elem[0] + " " + elem[1]
        num = elem[-1][:-1]
        
        students[name] = num

for dir in next(os.walk(path))[1]:
    studentName = dir.split("_")[0]
    os.rename(path+dir, path+students[studentName])
