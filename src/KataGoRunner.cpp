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
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    // 1. Create Pipes
    if (!CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0)) return false;
    if (!SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0)) return false;
    if (!CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0)) return false;
    if (!SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0)) return false;

    // 2. Prepare Command Line
    // Command: katago.exe gtp -model model.bin.gz -config cpu_config.cfg
    std::string cmdLine = exePath + " gtp -model " + modelPath + " -config " + configPath;
    
    // Windows requires a mutable char buffer for command line
    std::vector<char> cmdVec(cmdLine.begin(), cmdLine.end());
    cmdVec.push_back(0);

    STARTUPINFOA siStartInfo;
    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdError = g_hChildStd_OUT_Wr; // Redirect stderr to stdout
    siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
    siStartInfo.hStdInput = g_hChildStd_IN_Rd;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    // 3. Launch Process
    bool success = CreateProcessA(NULL, cmdVec.data(), NULL, NULL, TRUE, 0, NULL, NULL, &siStartInfo, &piProcInfo);
    
    // Close the ends of the pipes we don't need (so reading doesn't hang)
    if (success) {
        CloseHandle(g_hChildStd_OUT_Wr);
        CloseHandle(g_hChildStd_IN_Rd);
        g_hChildStd_OUT_Wr = NULL; 
        g_hChildStd_IN_Rd = NULL;
    }
    return success;
}

std::string KataGoRunner::sendCommand(std::string cmd) {
    DWORD dwWritten, dwRead;
    std::string fullCmd = cmd + "\n";
    WriteFile(g_hChildStd_IN_Wr, fullCmd.c_str(), fullCmd.length(), &dwWritten, NULL);

    std::string response = "";
    char buffer[128];
    // Read until we see the double newline (standard GTP end marker)
    while (true) {
        if (!ReadFile(g_hChildStd_OUT_Rd, buffer, 1, &dwRead, NULL) || dwRead == 0) break;
        response += buffer[0];
        // Check for end of response (double newline)
        if (response.length() >= 2 && response.substr(response.length()-2) == "\n\n") break;
    }
    return response;
}