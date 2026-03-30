// Example AJLANG script
import module.aj

let x = 5;
let y = x + 3;
print y;  // 8

if y > 0 {
    print "positive";  // Note: Prints as string, but evaluator needs extension for strings in v0.2
}

while x < 10 {
    x = x + 1;
    print x;  // 6 7 8 9 10
   
}

print 10+3;
