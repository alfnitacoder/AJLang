// Example: ./ajlang examples/check.aj -i eth0 -l list.text
//
// After the script path, every token is a string argument:
//   argc  = how many (here 4)
//   arg0  = "-i", arg1 = "eth0", arg2 = "-l", arg3 = "list.text"
//
// There is no string == yet; use positional knowledge or exec/sh to test flags.

print "Argument count (argc):";
print argc;

if argc > 0 {
    print "arg0:";
    print arg0;
}
if argc > 1 {
    print "arg1:";
    print arg1;
}
if argc > 2 {
    print "arg2:";
    print arg2;
}
if argc > 3 {
    print "arg3:";
    print arg3;
}

// Example: treat -i <iface> -l <file> when argc is 4
if argc > 3 {
    let iface = arg1;
    let listfile = arg3;
    print "iface (from arg1):";
    print iface;
    print "list file (from arg3):";
    print listfile;
}
