#!/usr/bin/perl

use strict;
use warnings;

# Declare variables
my $name = "Alice";
my $age = 25;

# Print variables
print "Hello, my name is $name and I am $age years old.\n";

# Check if age is greater than 18
if ($age > 18) {
    print "$name is an adult.\n";
} else {
    print "$name is not an adult.\n";
}

# Use a loop to print numbers from 1 to 5
for (my $i = 1; $i <= 5; $i++) {
    print "Number: $i\n";
}

# Use a subroutine to greet
sub greet {
    my ($person) = @_;
    print "Hello, $person!\n";
}

# Call the subroutine
greet("Bob");
