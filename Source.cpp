#include <Windows.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <algorithm>
#include <tlhelp32.h>
#include <cstdio>

using namespace std;

bool state = false;
string text = "";

string GetClipboardText() {
  OpenClipboard(nullptr);
  HANDLE hData = GetClipboardData(CF_TEXT);
  char* pszText = static_cast<char*>(GlobalLock(hData));

  std::string text(pszText);
  GlobalUnlock(hData);
  CloseClipboard();

  return text;
}

void toClipboard(HWND hwnd, const std::string& s) {
  OpenClipboard(hwnd);
  EmptyClipboard();
  HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, s.size() + 1);
  if (!hg) {
    CloseClipboard();
    return;
  }
  memcpy(GlobalLock(hg), s.c_str(), s.size() + 1);
  GlobalUnlock(hg);
  SetClipboardData(CF_TEXT, hg);
  CloseClipboard();
  GlobalFree(hg);
}

string random_string(size_t length) {
  auto randchar = []() -> char {
    const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    const size_t max_index = (sizeof(charset) - 1);
    return charset[rand() % max_index];
  };
  std::string str(length, 0);
  std::generate_n(str.begin(), length, randchar);
  return str;
}


int CALLBACK WinMain(__in HINSTANCE hInstance, __in HINSTANCE hPrevInstance,
                     __in LPSTR lpCmdLine, __in int nCmdShow) {
  srand(time(NULL));

  MessageBox(NULL,
             "ClipboardChanger успешно запущен, для подмены нажмите 'ESCAPE', "
             "для выхода из программы нажмите 'END'. Это окно вы можете "
             "спокойно закрыть.",
             "ClipboardChanger [1.1.1]", MB_OK);

  while (true) {
    if (GetAsyncKeyState(VK_ESCAPE)) {
      if (state) {
        HWND hwnd = GetDesktopWindow();
        toClipboard(hwnd, text);
      } else {
        text = GetClipboardText();
        HWND hwnd = GetDesktopWindow();
        toClipboard(hwnd, random_string(text.size()));
      }
      state = !state;
    }
    if (GetAsyncKeyState(VK_END)) {
      MessageBox(NULL, "ClipboardChanger успешно завершил свою работу..", "ClipboardChanger [1.1.1]",
                 MB_OK);
      exit(1);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  return true;
}