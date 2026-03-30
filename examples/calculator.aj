// Simple REPL calculator: pick an operation, enter two integers, see the result.
// Menu: 1 +  2 -  3 *  4 /  0 = exit (no == in the language, so tests use ranges)

let go = 1;
while go > 0 {
    print "-------------------------";
    print " 1 = add       2 = subtract";
    print " 3 = multiply  4 = divide";
    print " 0 = quit";
    print "-------------------------";
    print "Choice:";
    read op;

    if op < 1 {
        go = 0;
    }

    if op > 0 {
        if op < 2 {
            print "First number:";
            read a;
            print "Second number:";
            read b;
            print a + b;
        }
    }

    if op > 1 {
        if op < 3 {
            print "First number:";
            read a;
            print "Second number:";
            read b;
            print a - b;
        }
    }

    if op > 2 {
        if op < 4 {
            print "First number:";
            read a;
            print "Second number:";
            read b;
            print a * b;
        }
    }

    if op > 3 {
        if op < 5 {
            print "First number:";
            read a;
            print "Second number:";
            read b;
            print a / b;
        }
    }
}

print "Goodbye.";
