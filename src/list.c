#include <malloc.h>
#include <list.h>

List* create_list(void *content) {
    List *list = (List *)malloc(sizeof(List));
    list->content = content;
    list->next = NULL;
    return list;
}

void destroy_list(List *list) {
    while (list != NULL)
        list_remove(&list, 0);
}

int list_length(List *list) {
    if (list == NULL)
        return 0;
    return 1 + list_length(list->next);
}

void *list_get(List *list, int index) {
    if (list == NULL)
        return NULL;
    else if (index == 0)
        return list->content;
    else if (list->next == NULL)
        return NULL;
    return list_get(list->next, index - 1);
}

List *list_append(List **list, void *content) {
    // TODO: rewrite this more compact lol
    List *new_node;
    if (*list == NULL) {
        new_node = create_list(content);
        *list = new_node;
    }
    else if ((*list)->next == NULL)
        new_node = (*list)->next = create_list(content);
    else
        new_node = list_append(&((*list)->next), content);
    return new_node;
}

void list_remove(List **list, int index) {
    // TODO: rewrite this more compact lol
    if (*list == NULL)
        return;
    else if (index == 0) {
        List *next = (*list)->next;
        free(*list);
        *list = next;
    }
    else if ((*list)->next != NULL)
        list_remove(&((*list)->next), index - 1);
}
