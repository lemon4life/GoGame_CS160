#pragma once
#include <string>
#include <windows.h>

class KataGoRunner {
private:
    HANDLE g_hChildStd_IN_Rd = NULL;
    HANDLE g_hChildStd_IN_Wr = NULL;
    HANDLE g_hChildStd_OUT_Rd = NULL;
    HANDLE g_hChildStd_OUT_Wr = NULL;
    PROCESS_INFORMATION piProcInfo;

public:
    KataGoRunner();
    ~KataGoRunner();

    // Starts the AI engine
    bool startEngine(std::string exePath, std::string modelPath, std::string configPath);

    // Sends a command (e.g., "play B Q16") and returns the raw response
    std::string sendCommand(std::string cmd);

    // Helper to extract the move from the response (removes the "=")
    std::string getMove(std::string cmd);
};