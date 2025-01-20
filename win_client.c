#include <stdio.h>
#include <Windows.h>

static void print_error(const char* s) {
    DWORD error_code = GetLastError();
    char buffer[256];
    DWORD size = FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_MAX_WIDTH_MASK,
        NULL,
        error_code,
        MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
        buffer,
        sizeof(buffer),
        NULL);
    fprintf(stderr, "%s: %s\n", s, buffer);
}

int main(void) {
    HANDLE port;
    DCB state = { 0 };
    const char *buf = "Hello\n";
    size_t nbyte, recv_idx = 0;
    DWORD res;
    char received[256];

    port = CreateFileA("\\\\.\\COM3",
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    if (port == INVALID_HANDLE_VALUE) {
        print_error("\\\\.\\COM3");
        return -1;
    }

    if (!FlushFileBuffers(port)) {
        print_error("FlushFileBuffers");
        CloseHandle(port);
        return -1;
    }

    // set timeout
    /*COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = 0;
    timeouts.ReadTotalTimeoutConstant = 100;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 100;
    timeouts.WriteTotalTimeoutMultiplier = 0;
    SetCommTimeouts(port, &timeouts);*/

    state.DCBlength = sizeof(DCB);
    state.BaudRate = 9600;
    state.ByteSize = 8;
    state.StopBits = ONESTOPBIT;
    state.Parity = NOPARITY;
    if (!SetCommState(port, &state)) {
        print_error("SetCommState");
        CloseHandle(port);
        return -1;
    }

    nbyte = strlen(buf) + 1;
    if (!WriteFile(port, buf, (DWORD)nbyte, &res, NULL)) {
        print_error("WriteFile");
        CloseHandle(port);
        return -1;
    }
    if (res != (DWORD)nbyte) {
        fprintf(stderr, "Failed to write all bytes to port");
        CloseHandle(port);
        return -1;
    }

    Sleep(2000); // ReadFile isn't blocking for some reason and 1 second isn't always enough
    do {
        if (recv_idx == 1 && received[recv_idx - 1] == '\0') { // weird case
            recv_idx = 0;
        }
        if (!ReadFile(port, received + recv_idx, 1, &res, NULL)) {
            print_error("WriteFile");
            CloseHandle(port);
            return -1;
        }
        if (res != 1) {
            fprintf(stderr, "Failed to read byte from port");
            CloseHandle(port);
            return -1;
        }
        ++recv_idx;
    } while (recv_idx < sizeof(received) && received[recv_idx - 1] != '\n');
    received[recv_idx - 1] = '\0';
    puts(received);

    return 0;
}
