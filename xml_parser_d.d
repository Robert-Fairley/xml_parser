/*****************************************
 *  XML Parser
 *  A simple example of an XML parsing program
 *
 *  The program builds a struct following the schema of the associated XML document
 *  using it to disassemble, make changes, and reassemble the document. In this case,
 *  the program currently prints out the result. The program will be modified to rewrite
 *  to the file and optionally log changes.
 *
 *  Note: The parser reads from a local XML file called `data.xml`. The relevant data can
 *        be found at: https://www.xmlfiles.com/examples/plant-catalog/
 *
 *  See also: https://dlang.org/phobos/std_xml.html
 *
 *  TBD ...
 *  Authors: ...
 *  Bugs: ...
 *  Date: January 8, 2018
 *  License: MIT
 *  Copyright: 2018 Rogers Media
 */

import std.xml;
import std.stdio;
import std.string;
import std.file;

struct Plant
{
   string common_name;      // Represents the element `<COMMON></COMMON>`
   string botanical_name;   // Represents the element `<BOTANICAL></BOTANICAL>`
   string zone;             // Represents the element `<ZONE></ZONE>``
   string light;            // Represents the element `<LIGHT></LIGHT>`
   string price;            // Represents the element `<PRICE></PRICE>`
   string availability;     // Represents the element `<AVAILABILITY></AVAILABILITY>`
}

void main()
{
    string s = cast(string) std.file.read("data.xml");

    // Check for proper form
    check(s);

    // Take it apart
    Plant[] plants;

    auto xml = new DocumentParser(s);
    xml.onStartTag["PLANT"] = (ElementParser xml)
    {
        Plant plant;
        xml.onEndTag["COMMON"]       = (in Element e) { plant.common_name     = e.text(); };
        xml.onEndTag["BOTANICAL"]    = (in Element e) { plant.botanical_name  = e.text(); };
        xml.onEndTag["ZONE"]         = (in Element e) { plant.zone            = e.text(); };
        xml.onEndTag["LIGHT"]        = (in Element e) { plant.light           = e.text(); };
        xml.onEndTag["PRICE"]        = (in Element e) { plant.price           = e.text(); };
        xml.onEndTag["AVAILABILITY"] = (in Element e) { plant.availability    = e.text(); };

        xml.parse();

        plants ~= plant;
    };
    xml.parse();

    // Put it back together again
    auto doc = new Document(new Tag("CATALOG"));

    foreach (plant;plants)
    {
        auto element = new Element("plant");
        
        /** Note that the elements beneath <CATALOG> or root are now in lowercase.
            This is just to illustrate the change being made. */
        element ~= new Element("common",       plant.common_name);
        element ~= new Element("botanical",    plant.botanical_name);
        element ~= new Element("zone",         plant.zone);
        element ~= new Element("light",        plant.light);
        element ~= new Element("price",        plant.price);
        element ~= new Element("availability", plant.availability);

        doc ~= element;
    }

    // Pretty print it
    writefln(join(doc.pretty(3), "\n"));
}