// reverse_list.h
#ifndef REVERSE_LIST_H
#define REVERSE_LIST_H

struct Node {
    int data;
    Node* next;
    Node(int val) : data(val), next(nullptr) {}
};

Node* reverse_list(Node* head);  // ������� ��� �������������� �������� ������
void print_list(Node* head);     // ������� ��� ������ �������� ������

#endif // REVERSE_LIST_H
