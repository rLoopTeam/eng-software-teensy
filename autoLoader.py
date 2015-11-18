#!/usr/bin/python

import sys
import os

filesInDirectory = []
teensyProcessorArg = "mk20dx256"
hex_history_file = 'last_hex.txt'
default_hex = 'teensy_flight.hex'

def read_hex_history():
    file_last_hex = open(hex_history_file,'r')
    hex_name = file_last_hex.readline()
    file_last_hex.close()
    return hex_name

def write_hex_history(hex_name):
    file_last_hex = open(hex_history_file,'w')
    file_last_hex.write(hex_name)
    file_last_hex.close()

def flash_file(hex_name):
    write_hex_history(hex_name)
    os.system("./teensy_loader_cli -mmcu=%s -vs %s" % (teensyProcessorArg, hex_name))
    print('uploading file "' + hex_name + '" ...')
    print('uploading done!\n')

def main():

    for file in os.listdir(os.getcwd()):
        if file.endswith('.hex'):
            filesInDirectory.append(file)

    if bool(len(sys.argv) - 1):
        if sys.argv[1] == '-c':
            print('INFO: current ".hex" file is "' + read_hex_history() + '"\n')
        elif sys.argv[1] == '-f':
            if os.path.exists(default_hex):
                flash_file(default_hex)
            else:
                print('ERROR: file "' + default_hex + '" does not exist!\n')
        else:
            print('ERROR: the ' + sys.argv[1] + ' parameter is not recognized!\n')
    else:
        if len(filesInDirectory) == 0:
            print("ERROR: There are no .hex files in the directory:\n%s\n" % (os.getcwd()))
        elif len(filesInDirectory) == 1:
            flash_file(filesInDirectory[0])
        else:
            count = 0
            print("INPUT: Choose File to Upload:\n")
            for i in filesInDirectory:
                print("%d: %s" % (count, i))
                count += 1
            try:
                fileToUpload = input(); print('')
                flash_file(filesInDirectory[int(fileToUpload)])
            except:
                print('ERROR: invalid input!\n')

if __name__ == '__main__':
    main()
