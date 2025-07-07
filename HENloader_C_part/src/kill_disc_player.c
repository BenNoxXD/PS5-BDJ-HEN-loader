/*
 * Copyright (c) 2025 BenNox_XD
 *
 * This file is part of PS5-BDJ-HEN-loader and is licensed under the MIT License.
 * See the LICENSE file in the root of the project for full license information.
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <ps5/kernel.h>

#include "notification.h"
#include "lib_get_pid.h"
#include "kill_disc_player.h"

#define TITLE_ID_LEN 10

int sceLncUtilGetAppIdOfRunningBigApp();
int sceLncUtilGetAppTitleId(uint32_t app_id, char *title_id);
int sceLncUtilSuspendApp(uint32_t app_id);
int sceLncUtilKillApp(uint32_t app_id);

int kill_disc_player(const char *target_process, const char *target_title_id, const char *target_friendly_name) {    
    // Getting App ID of target_title_id
    uint32_t app_id = sceLncUtilGetAppIdOfRunningBigApp();
    printf("sceLncUtilGetAppIdOfRunningBigApp returned: 0x%08x\n", app_id);

    if (app_id == 0xffffffff) {
        printf("No App is running!\nExiting...\n");
        return 1;
    }

    char title_id[TITLE_ID_LEN] = {0};

    if (sceLncUtilGetAppTitleId(app_id, title_id) != 0) {
        strncpy(title_id, "UNKNOWN", TITLE_ID_LEN);
    }
    printf("App ID: 0x%08x\nTitle ID: %s\n", app_id, title_id);

    // Title ID check
    if (strcmp(title_id, target_title_id) != 0) {
        printf("Title ID mismatch: expected %s, got %s\n", target_title_id, title_id);
        return 1;
    }

    send_notification("Attempting to kill %s", target_friendly_name);
    
    // Attempt to suspend the app
    if (sceLncUtilSuspendApp(app_id) != 0) {
        printf("Failed to suspend app with ID: 0x%08x\n", app_id);
        return 1;
    }

    printf("Successfully suspended app: %s\n", target_title_id);

    // Waiting for 3 seconds to ensure being on the home screen
    printf("Waiting for 3 seconds to ensure being on the home screen...\n");
    sleep(3);

    // Sending SIGTERM to target_process
    printf("Sending SIGTERM to %s\n", target_process);
    pid_t pid = get_pid(target_process);
    if (pid == -1) {
        printf("%s not found.\n", target_process);
        return 1;
    }

    printf("%s has PID: %d\n", target_process, pid);
    if (kill(pid, SIGTERM) == -1) {
        perror("kill failed");
    } else {
        printf("Signal SIGTERM sent to process %d\n", pid);
    }

    sleep(1);

    printf("Attempting to kill: %s\n", target_process);
    int result = sceLncUtilKillApp(app_id);
    printf("result of sceLncUtilKillApp: %d\n", result);
    if (result != 0) {
        printf("The app %s has probably crashed or the kill failed, investigating...\n", title_id);
        send_notification("The app %s has probably crashed or the kill failed, investigating...", target_friendly_name);
        
        // Check if the app has crashed and is "terminated"; this is ok-ish
        if (sceLncUtilGetAppIdOfRunningBigApp() != 0xffffffff) {
            printf("Failed to kill %s, app is still running\n", target_process);
            send_notification("Failed to kill %s, app is still running", target_friendly_name);
            return 1;
        } else {
            printf("The app %s has crashed, this is considered as \"closed\", continuing...\n", target_process);
            send_notification("The app %s has crashed, this is considered as \"closed\", continuing...", target_friendly_name);
        }

    } else {
        printf("Successfully killed %s\n", title_id);
        send_notification("Successfully killed %s", target_friendly_name);
    }

    return 0;
}
