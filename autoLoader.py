#!/usr/bin/python

import sys
import os

hex_history_file = 'last_hex.txt'

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
    print('uploading file "' + hex_name + '" ...')
    print('uploading done!\n')

def main():
    if bool(len(sys.argv) - 1):
        if sys.argv[1] == '-c':
            print('current ".hex" file is: "' + read_hex_history() + '"\n')
        elif sys.argv[1] == '-f':
            flash_file('teensy_flight.hex')
        else:
            if os.path.exists(sys.argv[1]):
                flash_file(sys.argv[1])
            else:
                print('file "' + sys.argv[1] + '" does not exist!')
                print('example: autoloader.py teensy_flight_test.hex\n')
    else:
        hex_name = raw_input('please provide the name of the hex file: \n')
        if os.path.exists(hex_name):
            flash_file(hex_name)
        else:
            print('file "' + hex_name + '" does not exist!')
            print('example: autoloader.py teensy_flight_test.hex\n')

if __name__ == '__main__':
    main()
