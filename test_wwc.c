#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_OUTPUT 256
#define _POSIX_C_SOURCE 200809L

void run_test(const char* command, const char* expected_output, const char* test_name) {
    char output[MAX_OUTPUT];
    printf("Running command: %s\n", command);
    FILE* fp = popen(command, "r");
    if (fp == NULL) {
        perror("Failed to run command");
        exit(1);
    }

    if (fgets(output, sizeof(output), fp) != NULL) {
        output[strcspn(output, "\n")] = 0;  // Remove newline
        printf("Command output: %s\n", output);
        if (strcmp(output, expected_output) == 0) {
            printf("%s: PASSED\n", test_name);
        } else {
            printf("%s: FAILED\n", test_name);
            printf("Expected: %s\n", expected_output);
            printf("Got: %s\n", output);
        }
    } else {
        printf("No output from command\n");
    }

    int status = pclose(fp);
    if (status == -1) {
        perror("pclose");
    } else if (WIFEXITED(status)) {
        printf("Command exited with status %d\n", WEXITSTATUS(status));
    } else {
        printf("Command did not exit normally\n");
    }
}

int main() {
    // Ensure test.txt exists and has the correct content
    system("echo 'This is a test file.\nIt has multiple lines.\nAnd some words.' > test.txt");

    // Step One: -c option
    run_test("./wwc -c test.txt", "63 test.txt", "Test -c option");

    // Step Two: -l option
    run_test("./wwc -l test.txt", "3 test.txt", "Test -l option");

    // Step Three: -w option
    run_test("./wwc -w test.txt", "13 test.txt", "Test -w option");

    // Step Four: -m option
    // Note: This might be the same as -c if locale doesn't support multibyte characters
    run_test("./wwc -m test.txt", "73 test.txt", "Test -m option");

    // Step Five: default option (equivalent to -c, -l, -w, -m)
    run_test("./wwc test.txt", "3 13 73 73 test.txt", "Test default option");

    // Additional tests
    run_test("./wwc -w -l test.txt", "3 13 test.txt", "Test multiple options");
    run_test("./wwc -clw test.txt", "3 13 73 test.txt", "Test combined options");
    
    // Test invalid option
    FILE* fp = popen("./wwc -x test.txt 2>&1", "r");
    char output[MAX_OUTPUT];
    if (fgets(output, sizeof(output), fp) != NULL) {
        if (strstr(output, "Invalid flags") != NULL) {
            printf("Test invalid option: PASSED\n");
        } else {
            printf("Test invalid option: FAILED\n");
            printf("Expected error message, got: %s", output);
        }
    }
    pclose(fp);

    // Test non-existent file
    fp = popen("./wwc nonexistent.txt 2>&1", "r");
    if (fgets(output, sizeof(output), fp) != NULL) {
        if (strstr(output, "Error opening file") != NULL) {
            printf("Test non-existent file: PASSED\n");
        } else {
            printf("Test non-existent file: FAILED\n");
            printf("Expected error message, got: %s", output);
        }
    }
    pclose(fp);

    return 0;
}
