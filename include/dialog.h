#ifndef DIALOG_H
#define DIALOG_H

typedef enum {
    rt_reply,
    rt_question
} reply_type;

typedef struct struct_dialog_reply {
    char* reply;
    char* message;
    reply_type type;
    struct struct_dialog_reply* next;
} struct_dialog_reply;

typedef struct struct_dialog_message {
    char* match;
    char* message;
    struct struct_dialog_reply* replies;
    struct struct_dialog_message* next;
} struct_dialog_message;

typedef struct struct_dialog_information {
    struct struct_dialog_reply* all_replies;
    struct struct_dialog_message* all_messages;
} struct_dialog_information;

#endif /* DIALOG_H */
