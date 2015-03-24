#!/usr/bin/env python3
import sys
from xml.etree.cElementTree import parse

class Module(object):
    def __init__(self, filename):
        self.errors = {}
        self.requests = {}
        self.events = {}

        root = parse(filename).getroot()
        if root.get("header") == "xproto":
            self.xname = "xproto"
            self.name = "xproto"
            self.is_ext = False
        else:
            self.xname = root.get("extension-xname")
            self.name = root.get("extension-name")
            self.is_ext = True

        for elt in list(root):
            tag = elt.tag
            if tag == "error" or tag == "event":
                name = elt.get("name")
                number = int(elt.get("number"))
                if tag == "error":
                    self.errors[number] = name
                else:
                    self.events[number] = name
            elif tag == "request":
                name = elt.get("name")
                opcode = int(elt.get("opcode"))
                self.requests[opcode] = name

        self.errors_table = self.handle_type("error", self.errors)
        self.requests_table = self.handle_type("request", self.requests)
        self.events_table = self.handle_type("event", self.events)

    def handle_type(self, kind, entries):
        # Do we have any entries at all?
        if not entries:
            return

        num_entries = 1 + max(num for num in entries)
        if not self.is_ext:
            num_entries = 256
        names = [ "Unknown (" + str(i) + ")" for i in range(0, num_entries)]
        for key in entries:
            if key < 0:
                print("%s: Ignoring invalid %s %s (%d)" % (self.name, kind, entries[key], key))
            else:
                names[key] = entries[key]
        return names

modules = []
def parseFile(filename):
    modules.append(Module(filename))

# Parse the xml file
output_file = sys.argv[1]
for input_file in sys.argv[2:]:
    parseFile(input_file)

output = open(output_file, "w")
output.write("#include \"errors.h\"\n")

def format_strings(name, table):
    if table is None:
        output.write("\t.num_%s = 0,\n" % name)
        output.write("\t.strings_%s = NULL,\n" % name)
    else:
        output.write("\t.num_%s = %d,\n" % (name, len(table)))
        output.write("\t.strings_%s = \"%s\\0\",\n" % (name, "\\0".join(table)))

for module in modules:
    t = ""
    if module.is_ext:
        t = "static "
    output.write("%sconst struct static_extension_info_t extension_%s_info = { // %s\n" % (t, module.name, module.xname))
    format_strings("minor", module.requests_table)
    format_strings("events", module.events_table)
    format_strings("errors", module.errors_table)
    output.write("};\n\n")

output.close()
