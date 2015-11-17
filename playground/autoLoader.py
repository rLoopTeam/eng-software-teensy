import os

filesInDirectory  = []

teensyProcessorArg = "mk20dx256"

for file in os.listdir(os.getcwd()):
    if file.endswith('.hex'):
        filesInDirectory.append(file)

if len(filesInDirectory) == 0:
    print("ERROR: There are no .hex files in the directory:\n%s" % (os.getcwd()))

elif len(filesInDirectory) == 0:
    os.system("./teensy_loader_cli -mmcu=%s -vs %s" % (teensyProcessorArg, filesInDirectory[0]))

else:
    count = 0
    print("Choose File to Upload:\n")
    for i in filesInDirectory:
        print("%d: %s" % (count, i))
	count += 1
    fileToUpload = input()
    os.system("./teensy_loader_cli -mmcu=%s -vs %s" % (teensyProcessorArg, filesInDirectory[int(fileToUpload)]))
    
    
        
        
