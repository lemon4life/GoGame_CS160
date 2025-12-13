#include "../include/KataGoRunner.h"
#include <iostream>
#include <vector>

KataGoRunner::KataGoRunner() {
    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
}

KataGoRunner::~KataGoRunner() {
    // Clean up handles when class is destroyed
    CloseHandle(piProcInfo.hProcess);
    CloseHandle(piProcInfo.hThread);
    CloseHandle(g_hChildStd_OUT_Wr);
    CloseHandle(g_hChildStd_IN_Rd);
}

bool KataGoRunner::startEngine(std::string exePath, std::string modelPath, std::string configPath) {
    // 1. DEBUG: Verify files exist before trying to run
    // This stops the crash by catching the error BEFORE Windows gets involved.
    FILE* f = fopen(exePath.c_str(), "r");
    if (f) { fclose(f); }
    else {
        std::cerr << "--- [ERROR] FILE NOT FOUND: " << exePath << " ---" << std::endl;
        std::cerr << "Current Working Directory is WRONG." << std::endl;
        return false; // Stop safely, don't crash
    }

    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    // Create Pipes
    if (!CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0)) return false;
    if (!SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0)) return false;
    if (!CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0)) return false;
    if (!SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0)) return false;

    // 2. Build Command Line (WITH QUOTES for safety)
    // We manually wrap in quotes \"...\" to handle spaces in "GitHub Projects"
    std::string cmdLine = "\"" + exePath + "\" gtp -model \"" + modelPath + "\" -config \"" + configPath + "\"";

    // DEBUG: Print the exact command we are trying to run
    std::cout << "--- [DEBUG] Executing: " << cmdLine << " ---" << std::endl;

    std::vector<char> cmdVec(cmdLine.begin(), cmdLine.end());
    cmdVec.push_back(0);

    STARTUPINFOA siStartInfo;
    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdError = g_hChildStd_OUT_Wr;
    siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
    siStartInfo.hStdInput = g_hChildStd_IN_Rd;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    // 3. Launch
    bool success = CreateProcessA(NULL, cmdVec.data(), NULL, NULL, TRUE, 0, NULL, NULL, &siStartInfo, &piProcInfo);

    if (success) {
        // Close write end of output pipe, read end of input pipe
        CloseHandle(g_hChildStd_OUT_Wr);
        CloseHandle(g_hChildStd_IN_Rd);
        g_hChildStd_OUT_Wr = NULL;
        g_hChildStd_IN_Rd = NULL;
    } else {
        std::cerr << "--- [ERROR] CreateProcess failed. Error Code: " << GetLastError() << " ---" << std::endl;
    }
    return success;
}

std::string KataGoRunner::sendCommand(std::string cmd) {
    DWORD dwWritten, dwRead;
    std::string fullCmd = cmd + "\n";

    WriteFile(g_hChildStd_IN_Wr, fullCmd.c_str(), fullCmd.length(), &dwWritten, NULL);

    std::string response = "";
    char buffer[1];

    while (true) {
        if (!ReadFile(g_hChildStd_OUT_Rd, buffer, 1, &dwRead, NULL) || dwRead == 0) break;

        // 1. Print live to console (Visual feedback)
        std::cout << buffer[0] << std::flush;

        // 2. SKIP '\r' (The Fix!)
        if (buffer[0] == '\r') continue;

        // 3. Add to response
        response += buffer[0];

        // 4. Now "\n\n" will be detected correctly because we removed the '\r'
        if (response.length() >= 2 && response.substr(response.length()-2) == "\n\n") break;
    }
    return response;
}