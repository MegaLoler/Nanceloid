// simple linked list implementation

// a list node
// a list as a whole is represented by its first node
typedef struct List {
    void *content;
    struct List *next;
} List;

// create a list containing one item
List *create_list(void *content);

// free a list
// does NOT free content items!
void destroy_list(List *list);

// see how long a list is
int list_length(List *list);

// retreive an item by its index
// return NULL if does not exist
void *list_get(List *list, int index);

// add a new item to the end of a list
// return the newly appended list node
List *list_append(List *list, void *content);

// delete an item from a list by its index
// return the new start node
List *list_remove(List *list, int index);
