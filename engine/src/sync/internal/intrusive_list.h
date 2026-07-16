#pragma once

typedef struct engine_list_node_s {
    struct engine_list_node_s *next;
    struct engine_list_node_s *prev;
} engine_list_node;

static inline void engine_list_init (engine_list_node *node) {
    node->next = node->prev = node;
}

static inline void engine_list_add (
    engine_list_node *node,
    engine_list_node *head
) {
    node->next = head->next;
    node->prev = head;
    head->next->prev = node;
    head->next = node;
}

static inline void engine_list_del (engine_list_node *node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
}