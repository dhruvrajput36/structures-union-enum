#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 1. Enumeration with an empty state */
typedef enum {
    TYPE_EMPTY,
    INT,
    FLOAT,
    CHAR,
    STRING
} DataType;

/* Union to store one active value */
typedef union {
    int int_value;
    float float_value;
    char char_value;
    char *string_value;
} Data;

/* Structure combining type and data */
typedef struct {
    Data data;
    DataType type;
} Container;


/* Function declarations */

void container_init(Container *c);
int container_clear(Container *c);

int container_set_int(Container *c, int value);
int container_set_float(Container *c, float value);
int container_set_char(Container *c, char value);
int container_set_string(Container *c, const char *value);

int container_get_int(const Container *c, int *out_value);
int container_get_float(const Container *c, float *out_value);
int container_get_char(const Container *c, char *out_value);
int container_get_string(const Container *c, const char **out_value);

const char *container_type_name(const Container *c);
int container_print(const Container *c);

int container_copy(Container *destination,
                   const Container *source);


/* Validate container state */
static int container_is_valid(const Container *c)
{
    if (c == NULL)
        return 0;

    switch (c->type) {
        case TYPE_EMPTY:
        case INT:
        case FLOAT:
        case CHAR:
            return 1;

        case STRING:
            return c->data.string_value != NULL;

        default:
            return 0;
    }
}


/* 2. Initialize container */
void container_init(Container *c)
{
    if (c == NULL)
        return;

    memset(&c->data, 0, sizeof(c->data));
    c->type = TYPE_EMPTY;
}


/* 3. Clear container safely */
int container_clear(Container *c)
{
    if (!container_is_valid(c))
        return 0;

    if (c->type == STRING) {
        free(c->data.string_value);
    }

    memset(&c->data, 0, sizeof(c->data));
    c->type = TYPE_EMPTY;

    return 1;
}


/* 4. Set integer */
int container_set_int(Container *c, int value)
{
    if (!container_clear(c))
        return 0;

    c->data.int_value = value;
    c->type = INT;

    return 1;
}


/* Set float */
int container_set_float(Container *c, float value)
{
    if (!container_clear(c))
        return 0;

    c->data.float_value = value;
    c->type = FLOAT;

    return 1;
}


/* Set character */
int container_set_char(Container *c, char value)
{
    if (!container_clear(c))
        return 0;

    c->data.char_value = value;
    c->type = CHAR;

    return 1;
}


/* 5. Set string with independent memory allocation */
int container_set_string(Container *c, const char *value)
{
    char *new_string;
    size_t length;

    if (!container_is_valid(c) || value == NULL)
        return 0;

    length = strlen(value);

    new_string = malloc(length + 1);

    /* Preserve previous value if allocation fails */
    if (new_string == NULL)
        return 0;

    memcpy(new_string, value, length + 1);

    /* Allocate first, then release old value */
    if (!container_clear(c)) {
        free(new_string);
        return 0;
    }

    c->data.string_value = new_string;
    c->type = STRING;

    return 1;
}


/* 6. Type-safe getters */

/* Get integer */
int container_get_int(const Container *c, int *out_value)
{
    if (!container_is_valid(c) ||
        out_value == NULL || c->type != INT)
        return 0;

    *out_value = c->data.int_value;
    return 1;
}


/* Get float */
int container_get_float(const Container *c, float *out_value)
{
    if (!container_is_valid(c) ||
        out_value == NULL || c->type != FLOAT)
        return 0;

    *out_value = c->data.float_value;
    return 1;
}


/* Get character */
int container_get_char(const Container *c, char *out_value)
{
    if (!container_is_valid(c) ||
        out_value == NULL || c->type != CHAR)
        return 0;

    *out_value = c->data.char_value;
    return 1;
}


/* Get string */
int container_get_string(const Container *c,
                         const char **out_value)
{
    if (!container_is_valid(c) ||
        out_value == NULL || c->type != STRING)
        return 0;

    *out_value = c->data.string_value;
    return 1;
}


/* 7. Return readable type name */
const char *container_type_name(const Container *c)
{
    if (!container_is_valid(c))
        return "INVALID";

    switch (c->type) {
        case TYPE_EMPTY: return "EMPTY";
        case INT:        return "INT";
        case FLOAT:      return "FLOAT";
        case CHAR:       return "CHAR";
        case STRING:     return "STRING";
        default:         return "INVALID";
    }
}


/* 8. Print according to active type */
int container_print(const Container *c)
{
    if (!container_is_valid(c))
        return 0;

    switch (c->type) {

        case TYPE_EMPTY:
            printf("EMPTY");
            break;

        case INT:
            printf("%d", c->data.int_value);
            break;

        case FLOAT:
            printf("%f", c->data.float_value);
            break;

        case CHAR:
            printf("%c", c->data.char_value);
            break;

        case STRING:
            printf("%s", c->data.string_value);
            break;

        default:
            return 0;
    }

    return 1;
}


/* 9. Deep copy implementation */
int container_copy(Container *destination,
                   const Container *source)
{
    Container temporary;

    if (!container_is_valid(destination) ||
        !container_is_valid(source))
        return 0;

    /* Handle self-copy */
    if (destination == source)
        return 1;

    container_init(&temporary);

    temporary.type = source->type;

    if (source->type == STRING) {

        size_t length = strlen(source->data.string_value);

        temporary.data.string_value = malloc(length + 1);

        if (temporary.data.string_value == NULL)
            return 0;

        memcpy(temporary.data.string_value,
               source->data.string_value,
               length + 1);

    } else {
        temporary.data = source->data;
    }

    /* Replace destination only after successful copy */
    if (!container_clear(destination)) {
        container_clear(&temporary);
        return 0;
    }

    *destination = temporary;

    return 1;
}


/* 10. Main demonstration and edge-case testing */
int main(void)
{
    Container items[6];
    Container copied;

    int int_value;
    float float_value;
    char char_value;
    const char *string_value;

    int i;

    /* Initialize every container */
    for (i = 0; i < 6; i++) {
        container_init(&items[i]);
    }

    container_init(&copied);

    printf("===== SAFE CONTAINER LIBRARY =====\n\n");


    /* Store mixed data */
    container_set_int(&items[0], 10);
    container_set_float(&items[1], 3.14f);
    container_set_char(&items[2], 'A');
    container_set_string(&items[3], "Hello");
    container_set_int(&items[4], 0);
    container_set_string(&items[5], "Data");


    /* Print container array */
    printf("CONTAINER ARRAY:\n");

    for (i = 0; i < 6; i++) {
        printf("Index %d | Type: %-6s | Value: ",
               i, container_type_name(&items[i]));

        container_print(&items[i]);

        printf("\n");
    }


    /* Deep copy demonstration */
    printf("\n===== DEEP COPY TEST =====\n");

    if (container_copy(&copied, &items[5])) {

        printf("Original: ");
        container_print(&items[5]);

        printf("\nCopied: ");
        container_print(&copied);

        printf("\n");

        container_set_string(&items[5], "Changed");

        printf("After changing original:\n");

        printf("Original: ");
        container_print(&items[5]);

        printf("\nCopied: ");
        container_print(&copied);

        printf("\n");
    }


    /* Type transitions */
    printf("\n===== TYPE TRANSITION TEST =====\n");

    container_set_string(&items[0], "First String");
    container_set_int(&items[0], 100);
    container_set_string(&items[0], "Second String");
    container_set_float(&items[0], 5.5f);

    printf("Final type: %s\n",
           container_type_name(&items[0]));

    printf("Final value: ");
    container_print(&items[0]);
    printf("\n");


    /* Integer zero test */
    printf("\n===== ZERO VALUE TEST =====\n");

    if (container_get_int(&items[4], &int_value)) {
        printf("Integer zero retrieved successfully: %d\n",
               int_value);
    }


    /* Wrong type getter test */
    printf("\n===== WRONG TYPE TEST =====\n");

    if (!container_get_int(&items[2], &int_value)) {
        printf("Correctly rejected integer getter for CHAR.\n");
    }


    /* Empty string test */
    printf("\n===== EMPTY STRING TEST =====\n");

    container_set_string(&items[3], "");

    if (container_get_string(&items[3], &string_value)) {
        printf("Empty string stored successfully: \"%s\"\n",
               string_value);
    }


    /* Same pointer replacement test */
    printf("\n===== SAME POINTER TEST =====\n");

    container_set_string(&items[3], "Same Pointer");

    container_get_string(&items[3], &string_value);

    if (container_set_string(&items[3], string_value)) {
        printf("Same-pointer string replacement successful: ");
        container_print(&items[3]);
        printf("\n");
    }


    /* Self-copy test */
    printf("\n===== SELF COPY TEST =====\n");

    if (container_copy(&items[3], &items[3])) {
        printf("Self-copy handled successfully.\n");
    }


    /* Empty container test */
    printf("\n===== EMPTY CONTAINER TEST =====\n");

    container_clear(&items[3]);

    printf("Type after clearing: %s\n",
           container_type_name(&items[3]));

    if (!container_get_int(&items[3], &int_value)) {
        printf("Empty container correctly rejects getter.\n");
    }


    /* Repeated cleanup test */
    printf("\n===== REPEATED CLEANUP TEST =====\n");

    container_clear(&items[3]);
    container_clear(&items[3]);

    printf("Repeated cleanup completed safely.\n");


    /* Final cleanup */
    printf("\n===== FINAL CLEANUP =====\n");

    for (i = 0; i < 6; i++) {
        container_clear(&items[i]);
    }

    container_clear(&copied);

    printf("All containers cleaned successfully.\n");

    return 0;
}
