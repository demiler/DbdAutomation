class FocuseHandler : public BasicEventHandler<HWINEVENTHOOK> {
protected:
    void hookCallback(
      HWINEVENTHOOK hWinEventHook,
      DWORD event,
      HWND hwnd,
      LONG idObject,
      LONG idChild,
      DWORD idEventThread,
      DWORD dwmsEventTime
    );

    bool focused;
    constexpr size_t MAX_LEN = 256;
    TCHAR AppName[MAX_LEN];
public:
    enum Event : unsigned { focused, blurred };

    FocuseHandler(const TCHAR *AppName);
    ~FocuseHandler();

    bool isFocused();
};
