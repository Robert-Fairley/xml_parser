"""
    XML_PARSER
        xml_parser_py.py

    This script parses an XML file, finds each of the entries
    and prints each entry's values with a label.

    See README.md

"""
import xml.etree.ElementTree as etree

FILE_IN = "data.xml"

def main():
    with open(FILE_IN, "r+") as fin:
        tree = etree.parse(fin)
        root = tree.getroot()

        plants = root.findall("PLANT")

        print "\r\nCATALOG of PLANTS\n"

        for plant in plants:
            print " --- "
            print "COMMON NAME:    ", plant.find("COMMON").text
            print "BOTANICAL NAME: ", plant.find("BOTANICAL").text
            print "ZONE:           ", plant.find("ZONE").text
            print "LIGHT:          ", plant.find("LIGHT").text
            print "PRICE:          ", plant.find("PRICE").text
            print "AVAILABILITY:   ", plant.find("AVAILABILITY").text
        
    print " --- \r\n"


if __name__ == "__main__":
    main()