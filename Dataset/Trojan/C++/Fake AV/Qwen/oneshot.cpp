#include <windows.h>
#include <string>

#pragma comment(lib, "user32.lib")

std::wstring GetRandomString(int length) {
    std::wstring result;
    for (int i = 0; i < length; ++i)
        result += L'Z' - rand() % 26;
    return result;
}

void ScareUser(HWND hwnd, int progress) {
    SetDlgItemText(hwnd, IDC_PROGRESS_LABEL, GetRandomString(15).c_str());
    SendMessage(GetDlgItem(hwnd, ID进度条), PBM_SETPOS, progress * 100 / 100, 0);
    Sleep(rand() % 200 + 300); // Random delay between updates
}

void FakeScan(HWND hwnd) {
    for (int i = 0; i < 100; ++i) {
        ScareUser(hwnd, i);
    }
}

void FlashWarning(HWND hwnd, int duration) {
    SetDlgItemText(hwnd, IDC_WARNING_LABEL, L"警报：发现高风险病毒！");
    DWORD start = GetTickCount();
    while (GetTickCount() - start < duration * 1000) {
        SendMessage(hwnd, WM_SETTEXT, 0, (LPARAM)L"危急警告！");
        Sleep(500);
        SendMessage(hwnd, WM_SETTEXT, 0, (LPARAM)L"警报：发现高风险病毒！");
        Sleep(500);
    }
}

void PanicInduction(HWND hwnd) {
    for (int i = 0; i < 3; ++i) {
        FlashWarning(hwnd, rand() % 3 + 2); // Random duration between 2 and 4 seconds
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE:
        CreateWindowEx(0, L"STATIC", L"扫描进度：", WS_CHILD | WS_VISIBLE, 10, 25, 300, 30, hwnd, NULL, NULL, NULL);
        CreateWindowEx(0, L"PROGRESSBAR", L"", WS_CHILD | WS_VISIBLE, 10, 75, 300, 20, hwnd, (HMENU)ID进度条, NULL, NULL);
        CreateWindowEx(0, L"STATIC", L"警告：发现高风险病毒！", WS_CHILD | WS_VISIBLE, 10, 120, 400, 35, hwnd, (HMENU)ID_WARNING_LABEL, NULL, NULL);
        break;
    case WM_COMMAND:
        if (LOWORD(wParam) == ID_fix_now && HIWORD(wParam) == BN_CLICKED) {
            MessageBox(hwnd, L"付款页面", L"购买许可证", MB_OK | MB_ICONWARNING);
        }
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW, WndProc, 0, 0, hInstance, NULL, LoadCursor(NULL, IDC_ARROW), (HBRUSH)(COLOR_WINDOW+1), NULL, L"FakeAV", NULL };
    RegisterClassEx(&wc);
    HWND hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, L"FakeAV", L"Fake Antivirus Scanner", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
