#include <windows.h>
#include <stdio.h>
#include <tlhelp32.h>
#include <stdlib.h>
#include "handlers.h"
#include "utils.h"
#include "logs.h"
#include "sounds.h"
#include "keys.h"
#include "global.h"

#include "automate.c"

#define MOUSE_DRIVER_NAME "lghub.exe"
#define MOUSE_DRIVER_DIR "C:\\Program Files\\LGHUB"
#define MOUSE_DRIVER_PATH MOUSE_DRIVER_DIR"\\"MOUSE_DRIVER_NAME

static void (*actFuncPtr)(void);

DWORD WINAPI
wrap(LPVOID nothing) {
  actFuncPtr();
}

DWORD WINAPI
thing(LPVOID nothing) {
  makeSound(S_PROGRAM_START);
  scriptsInit();
  HANDLE automate = NULL;
  bool exit = false;
  enum semEv_t lastEv = SE_NOTHING;

  while (!exit) {
    semEv = SE_NOTHING;
    WaitForSingleObject(semaphore, INFINITE);
    //printf("Sem event: %d\n", semEv);

    actFuncPtr = NULL;
    switch (semEv) {
      case SE_APP_FOCUSED:
        printf("App focused\n");
        focused = true;
        makeSound(S_PROGRAM_ACTIVE);
        break;
      case SE_APP_BLURED:
        printf("App blured\n");
        stop();
        focused = false;
        makeSound(S_PROGRAM_INACTIVE);
        break;
      case SE_SCRIPT_RESTART:
        if (semEv != lastEv) printf("Script time restarted\n");
        resetTime();
        break;
      case SE_SCRIPT_CANCEL:
        if (semEv != lastEv) printf("Script canceled\n");
        stop();
        break;
      case SE_SCRIPT_SPECIAL:
        if (semEv != lastEv) printf("Script special\n");
        action();
        break;
      case SE_PROGRAM_CLOSE:
        if (semEv != lastEv) printf("Close program\n");
        exit = true;
        break;
      case SE_SCRIPT_WIGGLE:
        actFuncPtr = wiggle;
        break;
      case SE_SCRIPT_STRUGGLE:
        actFuncPtr = struggle;
        break;
      case SE_SCRIPT_AUTOGEN:
        actFuncPtr = autoGen;
        break;
      case SE_SCRIPT_TOXIC:
        actFuncPtr = becomeToxic;
        break;
    }
    
    //focused = true; //DEBUG ONLY ==========================

    if (focused && actFuncPtr && !active) {
      printf("Running new action\n");
      automate = CreateThread(NULL, 0, wrap, NULL, 0, NULL);
    }
    else if (actFuncPtr) {
      printf("New action not executed because ");
      if (!focused)
        printf("app is not in focus, ");
      if (active)
        printf("another action is already active");
      printf("\n");
    }

    if (lastEv == semEv)
      printf("I");

    lastEv = semEv;
  }

  stop();
  makeSound(S_PROGRAM_CLOSE);
  Sleep(300);
  closeProgram();
  return 0;
}

void
start() {
  HANDLE seconds = CreateThread(NULL, 0, thing, NULL, 0, NULL);

  //start message loop
	MSG logit;
	GetMessage(&logit, NULL, 0, 0);
}

DWORD FindProcessId(LPCSTR processname) {
  HANDLE hProcessSnap;
  PROCESSENTRY32 pe32;
  DWORD result = 0;

  // Take a snapshot of all processes in the system.
  hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (INVALID_HANDLE_VALUE == hProcessSnap) {
    warning("Unable to get processes snapshot");
    return -1;
  }

  pe32.dwSize = sizeof(PROCESSENTRY32); // <----- IMPORTANT

  // Retrieve information about the first process,
  // and exit if unsuccessful
  if (!Process32First(hProcessSnap, &pe32)) {
      CloseHandle(hProcessSnap);          // clean the snapshot object
      warning("Unable to get snapshot info about processes");
      return -1;
  }

  do {
      if (!strcmp(processname, pe32.szExeFile))
          result = pe32.th32ProcessID;
  } while (result == 0 && Process32Next(hProcessSnap, &pe32));

  CloseHandle(hProcessSnap);

  return result;
}

void startUpMouseDriver(void) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

    BOOL launched = CreateProcessA(MOUSE_DRIVER_PATH, "", NULL, NULL,
        FALSE, 0, NULL, NULL, &si, &pi);

    if (!launched) {
      DWORD error = GetLastError();
      warning("Unable to launcher mouse drivers [%d]", error);
      if (error == 740) //command needs elavation
        logit("It seems that your mouse driver set to run only as admin."
            "Try to change it or run this program as admin.");
    }
    else
      logit("Successfully loaded mouse drivers");
}

void loadMouseDrivers(void) {
  DWORD pid = FindProcessId(MOUSE_DRIVER_NAME);
  int retryCounter = 0;

  while (pid == -1 && retryCounter < 5) {
    pid = FindProcessId(MOUSE_DRIVER_NAME);
    logit("Retrying... (%d)", ++retryCounter);
  }

  if (!pid) {
    logit("Didn't find any mouse drivers loaded. Loading...");
    startUpMouseDriver();
  }
  else if (pid == -1)
    warning("Unable to get any info about processes");
  else
    logit("Mouse drivers are already loaded");

}

int main(void) {
  loadConsole();
  //setHooks();
  loadMouseDrivers();
  //start();


  return 0;
}
