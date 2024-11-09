#include <stdio.h>
#include <stdlib.h>

#include <curl/curl.h>
#include <threads.h>
#include <time.h>

#ifdef _WIN32
    #include <windows.h>
    #include <lmcons.h>
#else
    #include <unistd.h>
    #include <pwd.h>
    #include <sys/types.h>
#endif

char* get_username() {
    static char username[256];

    #ifdef _WIN32
        DWORD username_len = UNLEN + 1;
        if (GetUserNameA(username, &username_len)) {
            return username;
        }
    #else
        struct passwd *pw = getpwuid(geteuid());
        if (pw != NULL) {
            snprintf(username, sizeof(username), "%s", pw->pw_name);
            return username;
        }
    #endif

    return NULL;
}

int sendNotification() {
    CURL *curl = curl_easy_init();

    if (!curl){
        printf("Could not initialize curl.");
        return(1);
    }

    curl_easy_setopt(curl, CURLOPT_URL, "https://ntfy.sh/skdev");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, get_username());

    int result = curl_easy_perform(curl);

    curl_easy_cleanup(curl);
    return result;
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

			struct timespec duration;
			duration.tv_sec = wait_time;
			thrd_sleep(&duration, NULL);

			retry_count++;
		}
	}
	printf("Max retries reached. Operation failed.\n");
}

int main() {

	exponential_backoff(9999, 1, 32);


	return 0;
}
