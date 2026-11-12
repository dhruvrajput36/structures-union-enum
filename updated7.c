#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <errno.h>
#include <stdint.h>

#define DESC_SIZE 100
#define COMMAND_SIZE 32

/* Task status enumeration */
typedef enum {
    PENDING,
    DONE
} TaskStatus;

/* Command enumeration */
typedef enum {
    CMD_ADD,
    CMD_DELETE,
    CMD_VIEW,
    CMD_UPDATE,
    CMD_COMPLETE,
    CMD_FIND,
    CMD_HELP,
    CMD_EXIT,
    CMD_UNKNOWN
} CommandType;

/* Task structure */
typedef struct {
    int id;
    char description[DESC_SIZE];
    TaskStatus status;
} Task;


/* Convert command into uppercase */
void convert_uppercase(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        str[i] = (char)toupper((unsigned char)str[i]);
    }
}


/* Convert string into command enumeration */
CommandType get_command_type(const char *command) {

    char temp[COMMAND_SIZE];

    if (strlen(command) >= sizeof(temp))
        return CMD_UNKNOWN;

    strcpy(temp, command);
    convert_uppercase(temp);

    if (strcmp(temp, "ADD") == 0) return CMD_ADD;
    if (strcmp(temp, "DELETE") == 0) return CMD_DELETE;
    if (strcmp(temp, "VIEW") == 0) return CMD_VIEW;
    if (strcmp(temp, "UPDATE") == 0) return CMD_UPDATE;
    if (strcmp(temp, "COMPLETE") == 0) return CMD_COMPLETE;
    if (strcmp(temp, "FIND") == 0) return CMD_FIND;
    if (strcmp(temp, "HELP") == 0) return CMD_HELP;
    if (strcmp(temp, "EXIT") == 0) return CMD_EXIT;

    return CMD_UNKNOWN;
}


/* Read a bounded line */
int read_line(const char *prompt, char *buffer, size_t size) {

    printf("%s", prompt);

    if (fgets(buffer, size, stdin) == NULL)
        return 0;

    size_t len = strlen(buffer);

    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[--len] = '\0';
    }
    else if (!feof(stdin)) {
        int ch;

        while ((ch = getchar()) != '\n' && ch != EOF);

        printf("Input is too long. Try again.\n");
        return -1;
    }

    if (len > 0 && buffer[len - 1] == '\r')
        buffer[len - 1] = '\0';

    return 1;
}


/* Remove extra spaces */
void trim_spaces(char *str) {

    char *start = str;

    while (isspace((unsigned char)*start))
        start++;

    if (start != str)
        memmove(str, start, strlen(start) + 1);

    size_t len = strlen(str);

    while (len > 0 && isspace((unsigned char)str[len - 1]))
        str[--len] = '\0';
}


/* Read and validate positive task ID */
int read_task_id(int *id) {

    char input[64];
    char *end;

    int result = read_line("Enter Task ID: ", input, sizeof(input));

    if (result <= 0)
        return result;

    trim_spaces(input);

    errno = 0;
    long value = strtol(input, &end, 10);

    while (isspace((unsigned char)*end))
        end++;

    if (input[0] == '\0' || *end != '\0' ||
        errno == ERANGE || value <= 0 || value > INT_MAX) {

        printf("Invalid ID! Enter a positive integer.\n");
        return -1;
    }

    *id = (int)value;
    return 1;
}


/* Read task description */
int read_description(char *description) {

    int result = read_line(
        "Enter task description: ",
        description,
        DESC_SIZE
    );

    if (result <= 0)
        return result;

    trim_spaces(description);

    if (strlen(description) == 0) {
        printf("Description cannot be empty.\n");
        return -1;
    }

    return 1;
}


/* Find task index by ID */
int find_task_index(Task *tasks, int count, int id) {

    for (int i = 0; i < count; i++) {

        if (tasks[i].id == id)
            return i;
    }

    return -1;
}


/* Display one task */
void display_task(Task task) {

    printf("\nTask ID: %d\n", task.id);
    printf("Description: %s\n", task.description);

    printf("Status: %s\n",
           task.status == DONE ? "DONE" : "PENDING");
}


/* Display all tasks */
void view_tasks(Task *tasks, int count) {

    if (count == 0) {
        printf("\nNo tasks available.\n");
        return;
    }

    printf("\n========== ALL TASKS ==========\n");

    for (int i = 0; i < count; i++) {
        display_task(tasks[i]);
    }

    printf("\nTotal tasks: %d\n", count);
}


/* Add a task */
int add_task(Task **tasks, int *count, size_t *capacity) {

    int id;
    char description[DESC_SIZE];

    int result = read_task_id(&id);

    if (result == 0) return 0;
    if (result == -1) return 1;

    if (find_task_index(*tasks, *count, id) != -1) {
        printf("Task ID already exists!\n");
        return 1;
    }

    result = read_description(description);

    if (result == 0) return 0;
    if (result == -1) return 1;

    /* Increase capacity when required */
    if ((size_t)*count == *capacity) {

        size_t new_capacity = (*capacity == 0) ? 2 : *capacity * 2;

        if (new_capacity < *capacity ||
            new_capacity > SIZE_MAX / sizeof(Task)) {

            printf("Memory capacity limit reached.\n");
            return 1;
        }

        Task *temp = realloc(
            *tasks,
            new_capacity * sizeof(Task)
        );

        if (temp == NULL) {
            printf("Memory allocation failed!\n");
            return 1;
        }

        *tasks = temp;
        *capacity = new_capacity;
    }

    (*tasks)[*count].id = id;

    strcpy((*tasks)[*count].description, description);

    (*tasks)[*count].status = PENDING;

    (*count)++;

    printf("Task %d added successfully!\n", id);

    return 1;
}


/* Delete a task */
int delete_task(Task *tasks, int *count) {

    int id;

    int result = read_task_id(&id);

    if (result == 0) return 0;
    if (result == -1) return 1;

    int index = find_task_index(tasks, *count, id);

    if (index == -1) {
        printf("Task not found!\n");
        return 1;
    }

    for (int i = index; i < *count - 1; i++) {
        tasks[i] = tasks[i + 1];
    }

    (*count)--;

    printf("Task %d deleted successfully!\n", id);

    return 1;
}


/* Update task description */
int update_task(Task *tasks, int count) {

    int id;
    char description[DESC_SIZE];

    int result = read_task_id(&id);

    if (result == 0) return 0;
    if (result == -1) return 1;

    int index = find_task_index(tasks, count, id);

    if (index == -1) {
        printf("Task not found!\n");
        return 1;
    }

    result = read_description(description);

    if (result == 0) return 0;
    if (result == -1) return 1;

    strcpy(tasks[index].description, description);

    printf("Task %d updated successfully!\n", id);

    return 1;
}


/* Mark task as completed */
int complete_task(Task *tasks, int count) {

    int id;

    int result = read_task_id(&id);

    if (result == 0) return 0;
    if (result == -1) return 1;

    int index = find_task_index(tasks, count, id);

    if (index == -1) {
        printf("Task not found!\n");
        return 1;
    }

    if (tasks[index].status == DONE) {
        printf("Task %d is already completed.\n", id);
        return 1;
    }

    tasks[index].status = DONE;

    printf("Task %d marked as DONE!\n", id);

    return 1;
}


/* Search task by ID */
int search_task(Task *tasks, int count) {

    int id;

    int result = read_task_id(&id);

    if (result == 0) return 0;
    if (result == -1) return 1;

    int index = find_task_index(tasks, count, id);

    if (index == -1) {
        printf("Task not found!\n");
    }
    else {
        printf("\nTask found:\n");
        display_task(tasks[index]);
    }

    return 1;
}


/* Display help */
void show_help(void) {

    printf("\n========== AVAILABLE COMMANDS ==========\n");

    printf("ADD      - Add a new task\n");
    printf("DELETE   - Delete a task by ID\n");
    printf("VIEW     - Display all tasks\n");
    printf("UPDATE   - Update task description\n");
    printf("COMPLETE - Mark a task as DONE\n");
    printf("FIND     - Search task by ID\n");
    printf("HELP     - Display available commands\n");
    printf("EXIT     - Exit the program\n");
}


/* Main function */
int main() {

    Task *tasks = NULL;

    int count = 0;

    size_t capacity = 0;

    char input[COMMAND_SIZE];

    CommandType command;

    printf("====================================\n");
    printf("       TASK MANAGEMENT SYSTEM\n");
    printf("====================================\n");

    show_help();

    while (1) {

        printf("\n");

        int result = read_line(
            "Enter command: ",
            input,
            sizeof(input)
        );

        if (result == 0) {
            printf("\nInput ended. Exiting program.\n");
            break;
        }

        if (result == -1)
            continue;

        trim_spaces(input);

        if (strlen(input) == 0) {
            printf("Please enter a command.\n");
            continue;
        }

        command = get_command_type(input);

        switch (command) {

            case CMD_ADD:

                if (!add_task(&tasks, &count, &capacity))
                    goto cleanup;

                break;

            case CMD_DELETE:

                if (!delete_task(tasks, &count))
                    goto cleanup;

                break;

            case CMD_VIEW:

                view_tasks(tasks, count);

                break;

            case CMD_UPDATE:

                if (!update_task(tasks, count))
                    goto cleanup;

                break;

            case CMD_COMPLETE:

                if (!complete_task(tasks, count))
                    goto cleanup;

                break;

            case CMD_FIND:

                if (!search_task(tasks, count))
                    goto cleanup;

                break;

            case CMD_HELP:

                show_help();

                break;

            case CMD_EXIT:

                printf("Exiting program...\n");
                goto cleanup;

            default:

                printf("Unknown command! Type HELP for commands.\n");
        }
    }

cleanup:

    free(tasks);

    printf("Memory released successfully.\n");

    return 0;
}
