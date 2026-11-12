#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>
#include <errno.h>
#include <math.h>

typedef struct {
    int roll;
    char name[50];
    int age;
    float gpa;
} Student;

// Read an integer safely
int read_int(const char *message, int min, int max) {
    char input[100];
    long value;
    char *end;

    while (1) {
        printf("%s", message);

        if (fgets(input, sizeof(input), stdin) == NULL)
            exit(0);

        errno = 0;
        value = strtol(input, &end, 10);

        while (*end == ' ' || *end == '\t')
            end++;

        if (end != input && (*end == '\n' || *end == '\0') &&
            errno != ERANGE && value >= min && value <= max)
            return (int)value;

        printf("Invalid input! Enter a number between %d and %d.\n",
               min, max);
    }
}

// Read GPA safely
float read_gpa(void) {
    char input[100];
    char *end;
    float value;

    while (1) {
        printf("Enter GPA (0.00 - 4.00): ");

        if (fgets(input, sizeof(input), stdin) == NULL)
            exit(0);

        errno = 0;
        value = strtof(input, &end);

        while (*end == ' ' || *end == '\t')
            end++;

        if (end != input && (*end == '\n' || *end == '\0') &&
            errno != ERANGE && isfinite(value) &&
            value >= 0.0f && value <= 4.0f)
            return value;

        printf("Invalid GPA! Please enter a value between 0 and 4.\n");
    }
}

// Read student name
void read_name(char name[]) {
    char input[256];

    while (1) {
        printf("Enter student name: ");

        if (fgets(input, sizeof(input), stdin) == NULL)
            exit(0);

        if (strchr(input, '\n') == NULL && !feof(stdin)) {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
            printf("Name is too long! Maximum 49 characters.\n");
            continue;
        }

        input[strcspn(input, "\n")] = '\0';

        if (strlen(input) == 0) {
            printf("Name cannot be empty.\n");
            continue;
        }

        if (strlen(input) > 49) {
            printf("Name is too long! Maximum 49 characters.\n");
            continue;
        }

        strcpy(name, input);
        return;
    }
}

// Create student list dynamically
Student *create_student_list(int capacity) {
    if (capacity <= 0 ||
        (size_t)capacity > SIZE_MAX / sizeof(Student))
        return NULL;

    return malloc((size_t)capacity * sizeof(Student));
}

// Find student by roll number
int find_student(Student *students, int count, int roll) {
    for (int i = 0; i < count; i++) {
        if (students[i].roll == roll)
            return i;
    }
    return -1;
}

// Display one student
void display_one(Student s) {
    printf("Roll Number: %d | Name: %s | Age: %d | GPA: %.2f\n",
           s.roll, s.name, s.age, s.gpa);
}

// Display all students
void display_students(Student *students, int count) {
    if (count == 0) {
        printf("\nNo student records available.\n");
        return;
    }

    printf("\n--- STUDENT RECORDS ---\n");

    for (int i = 0; i < count; i++) {
        display_one(students[i]);
    }
}

// Add student
void add_student(Student **students, int *count, int *capacity) {
    if (*count == INT_MAX) {
        printf("Maximum number of records reached.\n");
        return;
    }

    Student new_student;

    new_student.roll = read_int("Enter positive roll number: ",
                                1, INT_MAX);

    if (find_student(*students, *count, new_student.roll) != -1) {
        printf("Error! Roll number already exists.\n");
        return;
    }

    read_name(new_student.name);

    new_student.age = read_int("Enter age (16-100): ", 16, 100);
    new_student.gpa = read_gpa();

    // Increase capacity when array becomes full
    if (*count == *capacity) {
        if (*capacity > INT_MAX / 2) {
            printf("Cannot increase array capacity further.\n");
            return;
        }

        int new_capacity = (*capacity) * 2;

        if ((size_t)new_capacity > SIZE_MAX / sizeof(Student)) {
            printf("Requested memory is too large.\n");
            return;
        }

        Student *temp = realloc(
            *students,
            (size_t)new_capacity * sizeof(Student)
        );

        if (temp == NULL) {
            printf("Memory allocation failed. Record not added.\n");
            return;
        }

        *students = temp;
        *capacity = new_capacity;
    }

    (*students)[*count] = new_student;
    (*count)++;

    printf("Student added successfully!\n");
}

// Search student
void search_student(Student *students, int count) {
    int roll = read_int("Enter roll number to search: ", 1, INT_MAX);

    int index = find_student(students, count, roll);

    if (index == -1) {
        printf("Student not found.\n");
    } else {
        printf("\nStudent found:\n");
        display_one(students[index]);
    }
}

// Update student
void update_student(Student *students, int count) {
    int roll = read_int("Enter roll number to update: ", 1, INT_MAX);

    int index = find_student(students, count, roll);

    if (index == -1) {
        printf("Student not found.\n");
        return;
    }

    printf("Enter updated details:\n");

    read_name(students[index].name);
    students[index].age = read_int("Enter age (16-100): ", 16, 100);
    students[index].gpa = read_gpa();

    printf("Student updated successfully!\n");
}

// Delete student
void delete_student(Student *students, int *count) {
    int roll = read_int("Enter roll number to delete: ", 1, INT_MAX);

    int index = find_student(students, *count, roll);

    if (index == -1) {
        printf("Student not found.\n");
        return;
    }

    // Shift remaining records to the left
    for (int i = index; i < *count - 1; i++) {
        students[i] = students[i + 1];
    }

    (*count)--;

    printf("Student deleted successfully!\n");
}

// Display GPA summary
void gpa_summary(Student *students, int count) {
    if (count == 0) {
        printf("No records available for GPA analysis.\n");
        return;
    }

    int highest = 0;
    int lowest = 0;
    float total = 0;

    for (int i = 0; i < count; i++) {
        total += students[i].gpa;

        if (students[i].gpa > students[highest].gpa)
            highest = i;

        if (students[i].gpa < students[lowest].gpa)
            lowest = i;
    }

    printf("\n--- GPA ANALYSIS ---\n");

    printf("Highest GPA:\n");
    display_one(students[highest]);

    printf("\nLowest GPA:\n");
    display_one(students[lowest]);

    printf("\nAverage GPA: %.2f\n", total / count);
}

// Sort students by GPA in descending order
void sort_students(Student *students, int count) {
    if (count == 0) {
        printf("No records available for sorting.\n");
        return;
    }

    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {

            if (students[j].gpa < students[j + 1].gpa ||
                (students[j].gpa == students[j + 1].gpa &&
                 students[j].roll > students[j + 1].roll)) {

                Student temp = students[j];
                students[j] = students[j + 1];
                students[j + 1] = temp;
            }
        }
    }

    printf("\nStudents sorted by GPA (highest to lowest):\n");
    display_students(students, count);
}

int main(void) {
    int capacity = read_int(
        "Enter initial student capacity: ", 1, INT_MAX
    );

    Student *students = create_student_list(capacity);

    if (students == NULL) {
        printf("Memory allocation failed!\n");
        return 1;
    }

    int count = 0;
    int choice;

    do {
        printf("\n===== STUDENT RECORDS SYSTEM =====\n");
        printf("1. Add Student\n");
        printf("2. Display All Students\n");
        printf("3. Search Student\n");
        printf("4. Update Student\n");
        printf("5. Delete Student\n");
        printf("6. GPA Analysis\n");
        printf("7. Sort Students by GPA\n");
        printf("8. Exit\n");

        choice = read_int("Enter your choice: ", 1, 8);

        switch (choice) {
            case 1:
                add_student(&students, &count, &capacity);
                break;

            case 2:
                display_students(students, count);
                break;

            case 3:
                search_student(students, count);
                break;

            case 4:
                update_student(students, count);
                break;

            case 5:
                delete_student(students, &count);
                break;

            case 6:
                gpa_summary(students, count);
                break;

            case 7:
                sort_students(students, count);
                break;

            case 8:
                printf("Exiting program...\n");
                break;
        }

    } while (choice != 8);

    free(students);

    printf("Memory released successfully.\n");

    return 0;
}
