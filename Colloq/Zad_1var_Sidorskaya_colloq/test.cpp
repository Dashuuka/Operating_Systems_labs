// test.cpp
#include <gtest/gtest.h>
#include <stdexcept>
#include "factorial.h"
#include "reverse_list.h"

// Тесты для функции factorial
TEST(FactorialTest, PositiveNumbers) {
    EXPECT_EQ(factorial(1), 1);
    EXPECT_EQ(factorial(2), 2);
    EXPECT_EQ(factorial(3), 6);
    EXPECT_EQ(factorial(5), 120);
}

TEST(FactorialTest, InvalidInput) {
    EXPECT_THROW(factorial(-1), std::invalid_argument);
}

// Тесты для удаления дубликатов
TEST(RemoveDuplicatesTest, BasicTest) {
    std::vector<int> input = { 1, 2, 2, 3, 4, 4, 5 };
    std::vector<int> result = remove_duplicates(input);
    EXPECT_EQ(result.size(), 5);
    EXPECT_EQ(result[0], 1);
    EXPECT_EQ(result[1], 2);
    EXPECT_EQ(result[2], 3);
    EXPECT_EQ(result[3], 4);
    EXPECT_EQ(result[4], 5);
}

// Тесты для реверсирования списка
TEST(ReverseListTest, BasicTest) {
    Node* head = new Node(1);
    head->next = new Node(2);
    head->next->next = new Node(3);
    head->next->next->next = new Node(4);

    Node* reversed = reverse_list(head);

    EXPECT_EQ(reversed->data, 4);
    EXPECT_EQ(reversed->next->data, 3);
    EXPECT_EQ(reversed->next->next->data, 2);
    EXPECT_EQ(reversed->next->next->next->data, 1);
}
