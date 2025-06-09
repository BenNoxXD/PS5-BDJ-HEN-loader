/*
 * Copyright (c) 2025 BenNox_XD
 *
 * This file is part of PS5-BDJ-HEN-loader and is licensed under the MIT License.
 * See the LICENSE file in the root of the project for full license information.
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/proc.h>
#include <sys/user.h>
#include <sys/sysctl.h>
#include <ps5/kernel.h>

#include "src/notification.h"
#include "src/sendfile.h"

const char *target_process = "SceDiscPlayer";

const char *ip = "127.0.0.1";
int port = 9021;

const char *etaHEN_1 = "/mnt/usb0/etaHEN.bin";
const char *etaHEN_2 = "/data/etaHEN.bin";
const char *etaHEN_3 = "/mnt/disc/jar-payloads/etaHEN.bin";
const char *etaHEN_filepath = NULL;
const char *friendly_etaHEN_filepath = NULL;
bool etaHEN_USB_or_data = false;

const char *kstuff_1 = "/mnt/usb0/kstuff.elf";
const char *kstuff_2 = "/data/kstuff.elf";
const char *kstuff_filepath = NULL;
const char *friendly_kstuff_filepath = NULL;

const char *no_kstuff_1 = "/mnt/usb0/no_kstuff";
const char *no_kstuff_2 = "/data/etaHEN/no_kstuff";
bool no_kstuff_file_available = false;

typedef struct app_info {
    uint32_t app_id;
    uint64_t unknown1;
    uint32_t app_type;
    char     title_id[10];
    char     unknown2[0x3c];
} app_info_t;

int sceLncUtilForceKillApp(int appId);
  
int sceKernelGetAppInfo(pid_t pid, app_info_t *info);

uint32_t get_app_id_by_command(const char *target_command) { // Largely based on John Törnblom's ps paylod: https://github.com/ps5-payload-dev/sdk/blob/master/samples/ps/main.c
  int mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PROC, 0};
  app_info_t appinfo;
  size_t buf_size;
  void *buf;

  if (sysctl(mib, 4, NULL, &buf_size, NULL, 0)) {
    perror("sysctl size");
    return 0;
  }

  if (!(buf = malloc(buf_size))) {
    perror("malloc");
    return 0;
  }

  if (sysctl(mib, 4, buf, &buf_size, NULL, 0)) {
    perror("sysctl data");
    free(buf);
    return 0;
  }

  for (void *ptr = buf; ptr < (buf + buf_size);) {
    struct kinfo_proc *ki = (struct kinfo_proc *)ptr;
    ptr += ki->ki_structsize;

    if (strcmp(ki->ki_comm, target_command) != 0)
      continue;

    if (sceKernelGetAppInfo(ki->ki_pid, &appinfo)) {
      memset(&appinfo, 0, sizeof(appinfo));
    }

    free(buf);
    return appinfo.app_id;
  }

  free(buf);
  return 0;
}

int main() {
    uint32_t app_id = get_app_id_by_command(target_process);
    char formatted[11];
  
    if (app_id) {
        snprintf(formatted, sizeof(formatted), "0x%08x", app_id);
        printf("AppId for %s: %04x\n", target_process, app_id);
        printf("AppId for %s: %s\n", target_process, formatted);
    } else {
        printf("%s not found.\n", target_process);
        send_notification("DiscPlayer isn't running - exiting");
        return 1;
    }

    // check file existence
    if (access(etaHEN_1, F_OK) == 0) {
        etaHEN_filepath = etaHEN_1;
        friendly_etaHEN_filepath = "USB";
        etaHEN_USB_or_data = true;
    } else if (access(etaHEN_2, F_OK) == 0) {
        etaHEN_filepath = etaHEN_2;
        friendly_etaHEN_filepath = "/data";
        etaHEN_USB_or_data = true;
    } else if (access(etaHEN_3, F_OK) == 0) {
        etaHEN_filepath = etaHEN_3;
        friendly_etaHEN_filepath = "Disc";
    }

    if (access(kstuff_1, F_OK) == 0) {
        kstuff_filepath = kstuff_1;
        friendly_kstuff_filepath = "USB";
    } else if (access(kstuff_2, F_OK) == 0) {
        kstuff_filepath = kstuff_2;
        friendly_kstuff_filepath = "/data";
    }

    if (access(no_kstuff_1, F_OK) == 0 || access(no_kstuff_2, F_OK) == 0) {
        no_kstuff_file_available = true;
    } else if (etaHEN_USB_or_data == true && kstuff_filepath != NULL) {
        send_notification("Found etaHEN [%s] & kstuff [%s], but the required 'no_kstuff' file is missing - exiting", friendly_etaHEN_filepath, friendly_kstuff_filepath);
        printf("Found etaHEN [%s] & kstuff [%s], but the required 'no_kstuff' file is missing - exiting\n", friendly_etaHEN_filepath, friendly_kstuff_filepath);
        return 1;
    }

    if (etaHEN_filepath == NULL && kstuff_filepath == NULL) {
        send_notification("No kstuff or etaHEN found - exiting");
        printf("No kstuff or etaHEN found - exiting\n");
        return 1;
    }

    if (etaHEN_filepath != NULL && kstuff_filepath == NULL) { // etaHEN only -> etaHEN found & no kstuff found 
        send_notification("etaHEN will be loaded from %s", friendly_etaHEN_filepath);
        printf("etaHEN will be loaded from %s\n", etaHEN_filepath);
    } else if (kstuff_filepath != NULL && etaHEN_USB_or_data == false) { // kstuff only -> kstuff found & no etaHEN on USB or data
        send_notification("Kstuff will be loaded from %s", friendly_kstuff_filepath);
        printf("kstuff will be loaded from %s\n", kstuff_filepath);
    } else if (etaHEN_USB_or_data == true && kstuff_filepath != NULL && no_kstuff_file_available == true) { // combined
        send_notification("etaHEN will be loaded from %s\nkstuff will be loaded from %s", friendly_etaHEN_filepath, friendly_kstuff_filepath);
        printf("etaHEN will be loaded from %s\n", etaHEN_filepath);
        printf("kstuff will be loaded from %s\n", kstuff_filepath);
    }

    printf("Attempting to kill %s...\n", target_process);
    send_notification("Attempting to kill DiscPlayer");

    usleep(1750000); //1.75sec

    int APP_ID = (int)strtol(formatted, NULL, 16);
    int result = sceLncUtilForceKillApp(APP_ID);
    if (result < 0) {
        printf("Failed to kill %s - aborting\n", target_process);
        send_notification("Failed to kill DiscPlayer - aborting");
        return 1;
    }
    printf("Successfully killed %s", target_process);
    send_notification("Successfully killed DiscPlayer");

    sleep(1); // wait for the process to terminate completely

    // send file
    if (etaHEN_filepath != NULL && kstuff_filepath == NULL) { // etaHEN only -> etaHEN found & no kstuff found 
        if (send_file(ip, port, etaHEN_filepath) == 0) {
            printf("Sent etaHEN successfully.\n");
        } else {
            printf("Failed to send etaHEN.\n");
            send_notification("Failed to send etaHEN");
            return 1;
        }
    } else if (kstuff_filepath != NULL && etaHEN_USB_or_data == false) { // kstuff only -> kstuff found & no etaHEN on USB or data
        if (send_file(ip, port, kstuff_filepath) == 0) {
            printf("Sent kstuff successfully.\n");
        } else {
            printf("Failed to send kstuff.\n");
            send_notification("Failed to send kstuff");
            return 1;
        }
    } else if (etaHEN_USB_or_data == true && kstuff_filepath != NULL && no_kstuff_file_available == true) { // combined
        if (send_file(ip, port, etaHEN_filepath) == 0) {
            printf("Sent etaHEN successfully.\n");
        } else {
            printf("Failed to send etaHEN.\n");
            send_notification("Failed to send etaHEN");
            return 1;
        }
        sleep(10);
        if (send_file(ip, port, kstuff_filepath) == 0) {
            printf("Sent kstuff successfully.\n");
        } else {
            printf("Failed to send kstuff.\n");
            send_notification("Failed to send kstuff");
            return 1;
        }
    }

    return 0;
}