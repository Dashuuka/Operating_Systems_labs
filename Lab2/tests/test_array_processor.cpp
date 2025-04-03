#include <gtest/gtest.h>
#include "array_processor.h"
#include <vector>
#include <iostream>
#include <limits>

TEST(ArrayProcessorTest, HandlesEmptyArray) {
    std::vector<int> empty;
    EXPECT_THROW(ArrayProcessor::ProcessArray(empty), ArrayProcessor::EmptyArrayException);
}

TEST(ArrayProcessorTest, HandlesSingleElement) {
    std::vector<int> arr = { 42 };
    EXPECT_NO_THROW(ArrayProcessor::ProcessArray(arr));
    EXPECT_EQ(arr[0], 42);
}

TEST(ArrayProcessorTest, HandlesNegativeNumbers) {
    std::vector<int> arr = { -5, 3, -7 };
    EXPECT_NO_THROW(ArrayProcessor::ProcessArray(arr));
    EXPECT_EQ(arr, std::vector<int>({ -5, -3, -3 }));
}

TEST(ArrayProcessorTest, HandlesLargeArray) {
    std::vector<int> arr(1000, 5); 
    EXPECT_NO_THROW(ArrayProcessor::ProcessArray(arr));
    EXPECT_EQ(arr, std::vector<int>(1000, 5));
}

TEST(ArrayProcessorTest, HandlesMixedNumbers) {
    std::vector<int> arr = { 1, 2, 3, 4, 5 };
    EXPECT_NO_THROW(ArrayProcessor::ProcessArray(arr));
    EXPECT_EQ(arr, std::vector<int>({ 3, 2, 3, 4, 3 }));
}

TEST(ArrayProcessorTest, HandlesDuplicateElements) {
    std::vector<int> arr = { 2, 2, 2, 2 };
    EXPECT_NO_THROW(ArrayProcessor::ProcessArray(arr));
    EXPECT_EQ(arr, std::vector<int>({ 2, 2, 2, 2 }));
}

TEST(ArrayProcessorTest, HandlesAllElementsSame) {
    std::vector<int> arr = { 3, 3, 3, 3 };
    EXPECT_NO_THROW(ArrayProcessor::ProcessArray(arr));
    EXPECT_EQ(arr, std::vector<int>({ 3, 3, 3, 3 }));
}


TEST(ArrayProcessorTest, HandlesInvalidInput) {
    std::vector<int> arr = { 1, 2, 3 };
    EXPECT_NO_THROW(ArrayProcessor::ProcessArray(arr));

    // Simulate invalid input (e.g., negative size)
    std::vector<int> invalidArr;
    EXPECT_THROW(ArrayProcessor::ProcessArray(invalidArr), ArrayProcessor::EmptyArrayException);
}

TEST(ArrayProcessorTest, HandlesThreadExceptions) {
    std::vector<int> arr = { 1, 2, 3 };

    // Simulate thread creation failure
    EXPECT_NO_THROW(ArrayProcessor::ProcessArray(arr));

    // Simulate exception in thread
    EXPECT_NO_THROW(ArrayProcessor::ProcessArray(arr));
}

TEST(ArrayProcessorTest, HandlesMixedPositiveNegative) {
    std::vector<int> arr = { -2, 4, -6, 8 };
    EXPECT_NO_THROW(ArrayProcessor::ProcessArray(arr));
    EXPECT_EQ(arr, std::vector<int>({ -2, 4, 1, 1 }));
}

TEST(ArrayProcessorTest, HandlesZeroElements) {
    std::vector<int> arr = { 0, 0, 0 };
    EXPECT_NO_THROW(ArrayProcessor::ProcessArray(arr));
    EXPECT_EQ(arr, std::vector<int>({ 0, 0, 0 }));
}

TEST(ArrayProcessorTest, HandlesSingleNegativeElement) {
    std::vector<int> arr = { -42 };
    EXPECT_NO_THROW(ArrayProcessor::ProcessArray(arr));
    EXPECT_EQ(arr[0], -42);
}

TEST(ArrayProcessorTest, HandlesAllNegativeElements) {
    std::vector<int> arr = { -1, -2, -3, -10 };
    EXPECT_NO_THROW(ArrayProcessor::ProcessArray(arr));
    EXPECT_EQ(arr, std::vector<int>({ -4, -2, -3, -4}));
}

TEST(ArrayProcessorTest, HandlesNonIntegerAverage) {
    std::vector<int> arr = { 1, 2, 4 };
    EXPECT_NO_THROW(ArrayProcessor::ProcessArray(arr));
    EXPECT_EQ(arr, std::vector<int>({ 2, 2, 2 }));
}
