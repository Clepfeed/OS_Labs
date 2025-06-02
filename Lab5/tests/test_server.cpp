#include "gtest/gtest.h"
#include "common.h" // Your common.h for Employee, Request, Response etc.
#include <vector>
#include <string>
#include <windows.h> // For HANDLE, Mutex functions if directly called/verified

// Extern declarations for global variables from server.cpp
// These are defined in server.cpp and will be linked because server.cpp is compiled into TestRunner.
extern std::string filename;
extern std::vector<Employee> employees;
extern std::vector<bool> recordLocked;
extern std::vector<int> readCount;
extern std::vector<HANDLE> recordMutexes; // Be careful with direct manipulation
extern HANDLE globalMutex;                 // Be careful with direct manipulation

// Forward declaration for test helper functions from server.cpp (if not in a header)
#ifdef TEST_BUILD // Ensure these are only expected if server.cpp was compiled with TEST_BUILD
void server_test_setup(const std::vector<Employee>& initial_employees_data);
void server_test_teardown();
#endif

// Declare functions from server.cpp that you want to test
int findEmployeeIndex(int id);
// Note: Testing handleClientRequest directly is very hard due to pipe I/O and its structure.
// We will test the logic embedded within it by manipulating global state.


// Test Fixture for server tests
class ServerTest : public ::testing::Test {
protected:
    std::vector<Employee> sample_employees;

    void SetUp() override {
        sample_employees = {
            {1, "Alice", 10.0},
            {2, "Bob", 20.0},
            {3, "Charlie", 30.0}
        };
        // Initialize server state using the helper from server.cpp
        server_test_setup(sample_employees);
    }

    void TearDown() override {
        server_test_teardown();
    }

    // Helper to simulate the core logic of a READ_REQUEST case from handleClientRequest
    // This is a simplified way to test the logic without actual pipe I/O.
    // It directly manipulates and checks global server state.
    Response simulateReadRequestLogic(int employee_id_to_read) {
        Response response;
        response.success = false; // Default

        int index = findEmployeeIndex(employee_id_to_read);
        if (index == -1) {
            return response; // Employee not found
        }

        // Mimic logic from handleClientRequest for READ_REQUEST
        // Acquiring/releasing mutexes in a unit test like this is tricky and can lead to deadlocks
        // if not perfectly matched. For pure logic tests, we often assume mutexes are handled correctly
        // or test mutex behavior separately if possible.
        // Here, we'll assume the test has control and simplify.
        // WaitForSingleObject(globalMutex, INFINITE); // Cannot reliably test this line in isolation easily

        if (recordLocked[index]) {
            response.success = false;
        }
        else {
            readCount[index]++; // Simulate state change
            response.success = true;
            response.data = employees[index];
        }
        // ReleaseMutex(globalMutex); // Match WaitForSingleObject
        return response;
    }

    // Helper to simulate the initial permission check part of a WRITE_REQUEST
    Response simulateWriteRequestPermissionLogic(int employee_id_to_write) {
        Response response;
        response.success = false;

        int index = findEmployeeIndex(employee_id_to_write);
        if (index == -1) {
            return response;
        }

        // Mimic logic from handleClientRequest for WRITE_REQUEST (initial part)
        // WaitForSingleObject(recordMutexes[index], INFINITE);
        // WaitForSingleObject(globalMutex, INFINITE);
        if (recordLocked[index] || readCount[index] > 0) {
            response.success = false;
            // ReleaseMutex(globalMutex);
            // ReleaseMutex(recordMutexes[index]);
        }
        else {
            recordLocked[index] = true; // Simulate state change
            response.success = true;
            response.data = employees[index]; // Simulate sending current data
        }
        // ReleaseMutex(globalMutex); // If taken and not in 'false' branch
        // ReleaseMutex(recordMutexes[index]); // This is tricky, original code releases later
        return response;
    }

    // Helper to simulate COMPLETE_REQUEST logic
    void simulateCompleteRequestLogic(int employee_id_completed) {
        int index = findEmployeeIndex(employee_id_completed);
        if (index == -1) {
            return;
        }
        // WaitForSingleObject(globalMutex, INFINITE);
        if (readCount[index] > 0) {
            readCount[index]--;
        }
        // Original code also checks and clears recordLocked[index] here.
        // This might be for cases where a write was aborted by client sending COMPLETE.
        if (recordLocked[index]) {
            // This part of COMPLETE_REQUEST is a bit ambiguous in the original code
            // as WRITE_REQUEST itself is supposed to clear its lock.
            // For testing, we'd ensure it does what's intended.
            // recordLocked[index] = false; // If COMPLETE is meant to unlock writes too
        }
        // ReleaseMutex(globalMutex);
    }
};

TEST_F(ServerTest, FindEmployeeIndexFound) {
    ASSERT_EQ(findEmployeeIndex(1), 0);
    ASSERT_EQ(findEmployeeIndex(3), 2);
}

TEST_F(ServerTest, FindEmployeeIndexNotFound) {
    ASSERT_EQ(findEmployeeIndex(99), -1);
}

TEST_F(ServerTest, FindEmployeeIndexEmpty) {
    server_test_setup({}); // Setup with no employees
    ASSERT_EQ(findEmployeeIndex(1), -1);
}

TEST_F(ServerTest, HandleReadRequestSuccess) {
    int employee_idx_in_vector = 0; // Targeting Alice (ID 1)
    recordLocked[employee_idx_in_vector] = false; // Ensure not locked
    int initialReadCount = readCount[employee_idx_in_vector];

    Response res = simulateReadRequestLogic(1); // ID 1

    ASSERT_TRUE(res.success);
    ASSERT_EQ(res.data.num, 1);
    ASSERT_STREQ(res.data.name, "Alice");
    ASSERT_EQ(readCount[employee_idx_in_vector], initialReadCount + 1);
}

TEST_F(ServerTest, HandleReadRequestRecordLocked) {
    int employee_idx_in_vector = 1; // Targeting Bob (ID 2)
    recordLocked[employee_idx_in_vector] = true; // Lock Bob's record
    int initialReadCount = readCount[employee_idx_in_vector];

    Response res = simulateReadRequestLogic(2); // ID 2

    ASSERT_FALSE(res.success);
    ASSERT_EQ(readCount[employee_idx_in_vector], initialReadCount); // Read count should not change
}

TEST_F(ServerTest, HandleWriteRequestPermissionSuccess) {
    int employee_idx_in_vector = 0; // Targeting Alice (ID 1)
    recordLocked[employee_idx_in_vector] = false;
    readCount[employee_idx_in_vector] = 0;

    Response res = simulateWriteRequestPermissionLogic(1); // ID 1

    ASSERT_TRUE(res.success);
    ASSERT_TRUE(recordLocked[employee_idx_in_vector]); // Record should now be locked
    ASSERT_EQ(res.data.num, 1); // Should send current data
}

TEST_F(ServerTest, HandleWriteRequestPermissionFailIfAlreadyLocked) {
    int employee_idx_in_vector = 1; // Targeting Bob (ID 2)
    recordLocked[employee_idx_in_vector] = true; // Simulate it's already locked

    Response res = simulateWriteRequestPermissionLogic(2); // ID 2

    ASSERT_FALSE(res.success);
    ASSERT_TRUE(recordLocked[employee_idx_in_vector]); // State should remain locked by prior lock
}

TEST_F(ServerTest, HandleWriteRequestPermissionFailIfBeingRead) {
    int employee_idx_in_vector = 2; // Targeting Charlie (ID 3)
    readCount[employee_idx_in_vector] = 1; // Simulate it's being read

    Response res = simulateWriteRequestPermissionLogic(3); // ID 3

    ASSERT_FALSE(res.success);
    ASSERT_FALSE(recordLocked[employee_idx_in_vector]); // Should not become locked
}

TEST_F(ServerTest, HandleCompleteRequestDecrementsReadCount) {
    int employee_idx_in_vector = 0; // Alice
    readCount[employee_idx_in_vector] = 3;

    simulateCompleteRequestLogic(1); // ID 1

    ASSERT_EQ(readCount[employee_idx_in_vector], 2);
}

TEST_F(ServerTest, HandleCompleteRequestWithZeroReadCount) {
    int employee_idx_in_vector = 0; // Alice
    readCount[employee_idx_in_vector] = 0;

    simulateCompleteRequestLogic(1); // ID 1

    ASSERT_EQ(readCount[employee_idx_in_vector], 0); // Should not go below zero
}

// NOTE: The direct testing of mutex locking/unlocking (WaitForSingleObject, ReleaseMutex)
// within these simulated logic functions is omitted because it's complex to do correctly
// in a unit test without actual threading or sophisticated mocking. These tests focus
// on the state changes (recordLocked, readCount, response data) assuming mutexes
// protect these changes as intended in the full handleClientRequest function.
// Testing the file writing part of WRITE_REQUEST is also omitted as it's heavy I/O.