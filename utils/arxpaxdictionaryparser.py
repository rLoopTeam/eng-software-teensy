import xml.etree.ElementTree as ET
import sys

def main(args):
    xml = ET.parse(args[1])
    root = xml.getroot()
    for subroot in root:
        for subroot2 in subroot:
            name = ""
            comment = ""
            addr = 0
            if subroot2.tag == "ParameterDescription":
                for child in subroot2:
                    if child.tag == "Address":
                        addr = int(child.text)
                    elif child.tag == "Name":
                        name = child.text.strip().replace(" ", "_")
                    elif child.tag == "Description":
                        try:
                            comment = child.text.strip()
                            if "\n" in comment:
                                comment = ""
                        except:
                            comment = ""
                for c in name:
                    if c.isalnum() == False:
                        name = name.replace("" + c, "_")
                while "__" in name:
                    name = name.replace("__", "_")
                str = "\tArxPaxRegAddr_%s = 0x%04X," % (name, addr)
                if len(comment) > 0:
                    str += " // " + comment
                print str 

if __name__ == "__main__":
    main(sys.argv)