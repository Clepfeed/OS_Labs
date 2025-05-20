#pragma once

#include <Windows.h>
#include <string>
#include <iostream>

constexpr int MAX_MESSAGE_LENGTH = 20;

struct Message {
    char text[MAX_MESSAGE_LENGTH + 1] = { 0 };
    bool isEmpty = true;
};

const std::wstring SENDER_READY_EVENT_PREFIX = L"SenderReadyEvent_";
const std::wstring FILE_MUTEX_NAME = L"MessageFileMutex";
const std::wstring EMPTY_SLOTS_SEMAPHORE_NAME = L"EmptySlotsSemaphore";
const std::wstring FILLED_SLOTS_SEMAPHORE_NAME = L"FilledSlotsSemaphore";