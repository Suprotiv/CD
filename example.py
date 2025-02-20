# Variables
name = "John"
age = 25
is_student = True

# Array (List)
courses = ["Math", "Physics", "Computer Science"]

# Function to greet the user
def greet_user (name ) :
    print (f"Hello, {name}! Welcome to the system.")

# Function to display courses
def display_courses (course_list ) :
    print("Enrolled Courses:")
    for course in course_list:
        print (f"- {course}")

# Function to introduce the student
def introduce_student (name, age, courses ) :
    print(f"My name is {name}, and I am {age} years old.")
    display_courses(courses)

# Function to check if the student is enrolled in a specific course
def is_enrolled_in (courses, course_name ) :
    return course_name in courses

# Main program logic

greet_user ( name ) 
    
    # Introduce the student
introduce_student ( name, age, courses ) 
    
    # Check if the student is enrolled in "Math"
course_to_check = "Math"
if is_enrolled_in (courses, course_to_check):
        print (f"Yes, {name} is enrolled in {course_to_check}.")
else:
        print (f"No, {name} is not enrolled in {course_to_check}.")
