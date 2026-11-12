#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <math.h>

/* Union declaration */
typedef union {
    int i;
    float f;
    char str[20];
} UnionData;

/* Structure declaration */
typedef struct {
    int i;
    float f;
    char str[20];
} StructData;

/* Enumeration to identify active union member */
typedef enum {
    VALUE_INT,
    VALUE_FLOAT,
    VALUE_TEXT
} ValueType;

/* Tagged union structure */
typedef struct {
    UnionData value;
    ValueType type;
} TaggedRecord;


/* Function to set integer */
void set_integer(TaggedRecord *record, int value) {
    record->value.i = value;
    record->type = VALUE_INT;
}

/* Function to set float */
void set_float(TaggedRecord *record, float value) {
    if (!isfinite(value)) {
        printf("Invalid float value!\n");
        return;
    }

    record->value.f = value;
    record->type = VALUE_FLOAT;
}

/* Function to set string */
int set_text(TaggedRecord *record, const char *text) {
    if (strlen(text) >= sizeof(record->value.str)) {
        printf("String too long! Maximum 19 characters allowed.\n");
        return 0;
    }

    strcpy(record->value.str, text);
    record->type = VALUE_TEXT;
    return 1;
}

/* Function to display active union member safely */
void display_record(TaggedRecord record) {
    switch (record.type) {

        case VALUE_INT:
            printf("Integer: %d\n", record.value.i);
            break;

        case VALUE_FLOAT:
            printf("Float: %.2f\n", record.value.f);
            break;

        case VALUE_TEXT:
            printf("String: %s\n", record.value.str);
            break;

        default:
            printf("Invalid record type!\n");
    }
}


/* Main function */
int main() {

    UnionData udata;
    StructData sdata = {0};

    printf("===== STRUCTURE AND UNION MEMORY ANALYSIS =====\n\n");

    /* Structure values */
    sdata.i = 10;
    sdata.f = 220.5;
    strcpy(sdata.str, "Struct Programming");

    /* Union values: one member at a time */
    udata.i = 10;
    printf("Union Integer: %d\n", udata.i);

    strcpy(udata.str, "Union Programming");
    printf("Union String: %s\n", udata.str);

    udata.f = 220.5;
    printf("Union Float: %.2f\n", udata.f);


    /* Memory addresses of union members */
    printf("\n===== UNION MEMBER ADDRESSES =====\n");

    printf("Address of integer: %p\n", (void *)&udata.i);
    printf("Address of float:   %p\n", (void *)&udata.f);
    printf("Address of string:  %p\n", (void *)udata.str);


    /* Memory addresses of structure members */
    printf("\n===== STRUCTURE MEMBER ADDRESSES =====\n");

    printf("Address of integer: %p\n", (void *)&sdata.i);
    printf("Address of float:   %p\n", (void *)&sdata.f);
    printf("Address of string:  %p\n", (void *)sdata.str);


    /* Size comparison */
    printf("\n===== SIZE COMPARISON =====\n");

    printf("Size of UnionData: %zu bytes\n", sizeof(UnionData));
    printf("Size of StructData: %zu bytes\n", sizeof(StructData));


    /* Alignment comparison */
    printf("\n===== ALIGNMENT =====\n");

    printf("Union alignment: %zu bytes\n", _Alignof(UnionData));
    printf("Structure alignment: %zu bytes\n", _Alignof(StructData));


    /* Structure field offsets */
    printf("\n===== STRUCTURE FIELD OFFSETS =====\n");

    printf("Offset of integer: %zu\n", offsetof(StructData, i));
    printf("Offset of float:   %zu\n", offsetof(StructData, f));
    printf("Offset of string:  %zu\n", offsetof(StructData, str));


    /* Memory required for 100 records */
    printf("\n===== MEMORY FOR 100 RECORDS =====\n");

    printf("100 Union records: %zu bytes\n",
           100 * sizeof(UnionData));

    printf("100 Structure records: %zu bytes\n",
           100 * sizeof(StructData));


    /* Tagged union records */
    printf("\n===== TAGGED UNION RECORDS =====\n");

    TaggedRecord records[5];

    set_integer(&records[0], 10);
    set_float(&records[1], 220.5);
    set_text(&records[2], "Union Programming");
    set_integer(&records[3], 50);
    set_text(&records[4], "C Language");

    for (int i = 0; i < 5; i++) {
        printf("\nRecord %d: ", i + 1);
        display_record(records[i]);
    }


    /* Maximum length string test */
    printf("\n===== STRING VALIDATION =====\n");

    TaggedRecord test;

    if (set_text(&test, "1234567890123456789")) {
        printf("Maximum length string accepted: ");
        display_record(test);
    }

    set_text(&test, "This string is too long");


    /* Tagged union memory comparison */
    printf("\n===== TAGGED RECORD SIZE =====\n");

    printf("Plain Union size: %zu bytes\n", sizeof(UnionData));
    printf("Plain Structure size: %zu bytes\n", sizeof(StructData));
    printf("Tagged Record size: %zu bytes\n", sizeof(TaggedRecord));


    printf("\n===== MEMORY OBSERVATION =====\n");

    printf("Union members share the same memory location.\n");
    printf("Structure members have separate memory locations.\n");
    printf("Tagged unions use an enum to identify the active member.\n");
    printf("Union memory is useful when only one value is needed at a time.\n");


    return 0;
}
