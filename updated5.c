#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <ctype.h>
#include <float.h>

#define MAX_RECTANGLES 1000
#define EPSILON 0.00001f

typedef struct {
    int id;
    float width;
    float height;
    float area;
    float perimeter;
} Rectangle;

// Read an integer safely
int read_int(const char *message, int min, int max) {
    char line[100];
    long value;
    char *end;

    while (1) {
        printf("%s", message);

        if (fgets(line, sizeof(line), stdin) == NULL)
            exit(1);

        errno = 0;
        value = strtol(line, &end, 10);

        while (isspace((unsigned char)*end))
            end++;

        if (end != line && *end == '\0' &&
            errno != ERANGE && value >= min && value <= max) {
            return (int)value;
        }

        printf("Invalid input! Enter a value between %d and %d.\n",
               min, max);
    }
}

// Read a positive finite float
float read_dimension(const char *message) {
    char line[100];
    char *end;
    float value;

    while (1) {
        printf("%s", message);

        if (fgets(line, sizeof(line), stdin) == NULL)
            exit(1);

        errno = 0;
        value = strtof(line, &end);

        while (isspace((unsigned char)*end))
            end++;

        if (end != line && *end == '\0' &&
            errno != ERANGE && isfinite(value) && value > 0) {
            return value;
        }

        printf("Invalid dimension! Enter a finite positive number.\n");
    }
}

// Calculate area and perimeter
void calculate_geometry(Rectangle *r) {
    r->area = r->width * r->height;
    r->perimeter = 2 * (r->width + r->height);
}

// Find rectangle by ID
int find_rectangle(Rectangle *r, int count, int id) {
    for (int i = 0; i < count; i++) {
        if (r[i].id == id)
            return i;
    }
    return -1;
}

// Resize array dynamically
int resize_array(Rectangle **r, int *capacity) {
    if (*capacity >= MAX_RECTANGLES) {
        printf("Maximum capacity reached!\n");
        return 0;
    }

    int new_capacity = (*capacity == 0) ? 1 : (*capacity * 2);

    if (new_capacity > MAX_RECTANGLES)
        new_capacity = MAX_RECTANGLES;

    if ((size_t)new_capacity > SIZE_MAX / sizeof(Rectangle)) {
        printf("Allocation size overflow!\n");
        return 0;
    }

    Rectangle *temp = realloc(
        *r, (size_t)new_capacity * sizeof(Rectangle)
    );

    if (temp == NULL) {
        printf("Memory allocation failed!\n");
        return 0;
    }

    *r = temp;
    *capacity = new_capacity;

    return 1;
}

// Add rectangle
void add_rectangle(Rectangle **r, int *count, int *capacity) {
    if (*count == *capacity) {
        if (!resize_array(r, capacity))
            return;
    }

    int id = read_int("Enter Rectangle ID: ", 1, 2147483647);

    if (find_rectangle(*r, *count, id) != -1) {
        printf("Duplicate ID! Rectangle not added.\n");
        return;
    }

    float width = read_dimension("Enter width: ");
    float height = read_dimension("Enter height: ");

    if (width > FLT_MAX / height ||
        width + height > FLT_MAX / 2.0f) {
        printf("Dimensions are too large!\n");
        return;
    }

    Rectangle new_rectangle;

    new_rectangle.id = id;
    new_rectangle.width = width;
    new_rectangle.height = height;

    calculate_geometry(&new_rectangle);

    (*r)[*count] = new_rectangle;
    (*count)++;

    printf("Rectangle added successfully!\n");
}

// Display all rectangles
void display_all(Rectangle *r, int count) {
    if (count == 0) {
        printf("No rectangles available.\n");
        return;
    }

    printf("\nID\tWidth\tHeight\tArea\tPerimeter\n");

    for (int i = 0; i < count; i++) {
        printf("%d\t%.2f\t%.2f\t%.2f\t%.2f\n",
               r[i].id, r[i].width, r[i].height,
               r[i].area, r[i].perimeter);
    }
}

// Search rectangle
void search_rectangle(Rectangle *r, int count) {
    int id = read_int("Enter ID to search: ", 1, 2147483647);

    int index = find_rectangle(r, count, id);

    if (index == -1) {
        printf("Rectangle not found.\n");
        return;
    }

    printf("ID: %d\n", r[index].id);
    printf("Width: %.2f\n", r[index].width);
    printf("Height: %.2f\n", r[index].height);
    printf("Area: %.2f\n", r[index].area);
    printf("Perimeter: %.2f\n", r[index].perimeter);
}

// Update rectangle
void update_rectangle(Rectangle *r, int count) {
    int id = read_int("Enter ID to update: ", 1, 2147483647);

    int index = find_rectangle(r, count, id);

    if (index == -1) {
        printf("Rectangle not found.\n");
        return;
    }

    float width = read_dimension("Enter new width: ");
    float height = read_dimension("Enter new height: ");

    if (width > FLT_MAX / height ||
        width + height > FLT_MAX / 2.0f) {
        printf("Dimensions are too large!\n");
        return;
    }

    r[index].width = width;
    r[index].height = height;

    calculate_geometry(&r[index]);

    printf("Rectangle updated successfully!\n");
}

// Delete rectangle
void delete_rectangle(Rectangle *r, int *count) {
    int id = read_int("Enter ID to delete: ", 1, 2147483647);

    int index = find_rectangle(r, *count, id);

    if (index == -1) {
        printf("Rectangle not found.\n");
        return;
    }

    for (int i = index; i < *count - 1; i++) {
        r[i] = r[i + 1];
    }

    (*count)--;

    printf("Rectangle deleted successfully!\n");
}

// Area analysis
void area_analysis(Rectangle *r, int count) {
    if (count == 0) {
        printf("No rectangles available for analysis.\n");
        return;
    }

    float total = 0;
    int largest = 0;
    int smallest = 0;

    for (int i = 0; i < count; i++) {
        total += r[i].area;

        if (r[i].area > r[largest].area)
            largest = i;

        if (r[i].area < r[smallest].area)
            smallest = i;
    }

    printf("\nLargest Area: ID %d (%.2f)\n",
           r[largest].id, r[largest].area);

    printf("Smallest Area: ID %d (%.2f)\n",
           r[smallest].id, r[smallest].area);

    printf("Total Area: %.2f\n", total);
    printf("Average Area: %.2f\n", total / count);
}

// Square classification
void classify_rectangles(Rectangle *r, int count) {
    if (count == 0) {
        printf("No rectangles available.\n");
        return;
    }

    for (int i = 0; i < count; i++) {
        float scale = fmaxf(r[i].width, r[i].height);

        if (fabsf(r[i].width - r[i].height)
            <= EPSILON * scale) {
            printf("ID %d: Square\n", r[i].id);
        } else {
            printf("ID %d: Non-square\n", r[i].id);
        }
    }
}

// Sort by area in ascending order
void sort_rectangles(Rectangle *r, int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {

            if (r[i].area > r[j].area ||
                (r[i].area == r[j].area &&
                 r[i].id > r[j].id)) {

                Rectangle temp = r[i];
                r[i] = r[j];
                r[j] = temp;
            }
        }
    }

    printf("Rectangles sorted by area.\n");
    display_all(r, count);
}

// Filter by area
void filter_rectangles(Rectangle *r, int count) {
    float threshold = read_dimension("Enter minimum area threshold: ");

    int found = 0;

    for (int i = 0; i < count; i++) {
        if (r[i].area > threshold) {
            printf("ID: %d | Area: %.2f\n",
                   r[i].id, r[i].area);
            found = 1;
        }
    }

    if (!found)
        printf("No rectangles found above threshold.\n");
}

int main(void) {
    int count = 0;

    int capacity = read_int(
        "Enter initial number of rectangles (1-1000): ",
        1, MAX_RECTANGLES
    );

    Rectangle *rectangles = malloc(
        (size_t)capacity * sizeof(Rectangle)
    );

    if (rectangles == NULL) {
        printf("Memory allocation failed!\n");
        return 1;
    }

    printf("\n--- ENTER INITIAL RECTANGLES ---\n");

    for (int i = 0; i < capacity; i++) {
        printf("\nRectangle %d:\n", i + 1);
        add_rectangle(&rectangles, &count, &capacity);
    }

    int choice;

    do {
        printf("\n===== RECTANGLE INVENTORY SYSTEM =====\n");
        printf("1. Add Rectangle\n");
        printf("2. Display All Rectangles\n");
        printf("3. Search Rectangle by ID\n");
        printf("4. Update Rectangle\n");
        printf("5. Delete Rectangle\n");
        printf("6. Area Analysis\n");
        printf("7. Square Classification\n");
        printf("8. Sort by Area\n");
        printf("9. Filter by Area\n");
        printf("10. Exit\n");

        choice = read_int("Enter your choice: ", 1, 10);

        switch (choice) {
            case 1:
                add_rectangle(&rectangles, &count, &capacity);
                break;

            case 2:
                display_all(rectangles, count);
                break;

            case 3:
                search_rectangle(rectangles, count);
                break;

            case 4:
                update_rectangle(rectangles, count);
                break;

            case 5:
                delete_rectangle(rectangles, &count);
                break;

            case 6:
                area_analysis(rectangles, count);
                break;

            case 7:
                classify_rectangles(rectangles, count);
                break;

            case 8:
                sort_rectangles(rectangles, count);
                break;

            case 9:
                filter_rectangles(rectangles, count);
                break;

            case 10:
                printf("Exiting program...\n");
                break;
        }

    } while (choice != 10);

    free(rectangles);

    printf("Memory released successfully.\n");

    return 0;
}
