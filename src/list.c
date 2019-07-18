#include <malloc.h>
#include <list.h>

List* create_list(void *content) {
    List *list = (List *)malloc(sizeof(List));
    list->content = content;
    list->next = NULL;
    return list;
}

void destroy_list(List *list) {
    List *next = list->next;
    free(list);
    if (next != NULL)
        destroy_list(next);
}

int list_length(List *list) {
    if (list->next == NULL)
        return 1;
    return 1 + list_length(list->next);
}

void *list_get(List *list, int index) {
    if (index == 0)
        return list->content;
    else if (list->next == NULL)
        return NULL;
    return list_get(list->next, index - 1);
}

List *list_append(List *list, void *content) {
    if (list->next == NULL)
        return list->next = create_list(content);
    return list_append(list->next, content);
}

List *list_remove(List *list, int index) {
    if (index == 0)
        return list->next;
    else if (list->next != NULL)
        list->next = list_remove(list->next, index - 1);
    return list;
}
