#include <stdio.h>
#include "libusb-1.0/libusb.h"

#define VENDOR_ID       0x093a
#define PRODUCT_ID      0x2510
#define TRANSFER_SIZE   8

int main(int argc, char **argv) {
    struct libusb_device_handle *device_handle = NULL; // Our ADU's USB device handle
    char value_str[8]; // 8-byte buffer to store string values read from device (7 byte string + NULL terminating character)

    // Initialize libusb
    int result = libusb_init(NULL);
    if (result < 0) {
        printf("Error initializing libusb: %s\n", libusb_error_name(result));
        exit(-1);
    }

    // Set debugging output to max level
    libusb_set_option(NULL, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_WARNING);

    device_handle = libusb_open_device_with_vid_pid(NULL, VENDOR_ID, PRODUCT_ID);
    if (!device_handle) {
        printf("Error finding USB device\n");
        libusb_exit(NULL);
        exit(-2);
    }
    libusb_set_auto_detach_kernel_driver(device_handle, 1);

    // Claim interface 0 on the device
    result = libusb_claim_interface(device_handle, 0);
    if (result < 0) {
        printf("Error claiming interface: %s\n", libusb_error_name(result));
        if (device_handle) {
            libusb_close(device_handle);
        }

        libusb_exit(NULL);
        exit(-3);
    }
    return 0;
}