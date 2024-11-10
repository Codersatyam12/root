#include <windows.h>
#include <shlobj.h>
#include <iostream>
#include <string>

// Define root directory
const std::wstring ROOT_DIRECTORY = L"C:\\YourRootFolder";

// Hook procedure for clipboard and file operations
LRESULT CALLBACK ClipboardProc(int code, WPARAM wParam, LPARAM lParam) {
    if (code >= 0) {
        // Capture clipboard data and check if files are from the root
        if (IsClipboardFormatAvailable(CF_HDROP)) {
            OpenClipboard(nullptr);
            HANDLE hDrop = GetClipboardData(CF_HDROP);
            if (hDrop != nullptr) {
                // Process file paths
                HDROP hDrop = (HDROP)hDrop;
                wchar_t filePath[MAX_PATH];
                DragQueryFileW(hDrop, 0, filePath, MAX_PATH);

                // Check if path starts with the root directory
                if (std::wstring(filePath).find(ROOT_DIRECTORY) == 0) {
                    // Allow copying, but set flag for restricted paste
                } else {
                    // Disallow copying/pasting outside the root
                    EmptyClipboard();
                }
                CloseClipboard();
            }
        }
    }
    return CallNextHookEx(NULL, code, wParam, lParam);
}

// Main setup function
int main() {
    // Set up system-wide hook
    HHOOK hHook = SetWindowsHookEx(WH_CALLWNDPROC, ClipboardProc, NULL, GetCurrentThreadId());
    if (!hHook) {
        std::cerr << "Failed to set up hook!" << std::endl;
        return 1;
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(hHook);
    return 0;
}

