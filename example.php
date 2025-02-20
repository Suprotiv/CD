<?php
// Defining variables
$age = 25;
$name = "John";
$is_student = true;
$subjects = ["Math", "Physics", "Computer Science"]; // Array of subjects

// Function to print a greeting
function printGreeting($name, $age)
{
    print "Hello, my name is $name and I am $age years old.<br>";
}

// Function to check if the person is a student
function checkIfStudent($is_student)
{
    if ($is_student) {
        print "I am a student.<br>";
    } else {
        print "I am not a student.<br>";
    }
}

// Function to check age and print if the person is an adult or minor
function checkAge($age)
{
    if ($age > 18) {
        print "I am an adult.<br>";
    } else {
        print "I am a minor.<br>";
    }
}

// Function to print subjects
function printSubjects($subjects)
{
    print "I study the following subjects:<br>";
    foreach ($subjects as $subject) {
        print "- $subject<br>";
    }
}

// Calling the functions
printGreeting($name, $age);       // Greet the user
checkIfStudent($is_student);      // Check if the user is a student
checkAge($age);                   // Check if the user is an adult or minor
printSubjects($subjects);         // Print the subjects from the array

?>
