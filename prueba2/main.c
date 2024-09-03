#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

int main(int argc, char *argv[]) {
    FILE *inputFile, *outputFile;
    char buffer[1024];
    size_t bytesRead;

    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input_unicode_file> <output_utf8_file>\n", argv[0]);
        return 1;
    }

    // Set locale to UTF-8
    setlocale(LC_ALL, "en_US.UTF-8");

    // Open the input file in binary mode
    inputFile = fopen(argv[1], "rb");
    if (!inputFile) {
        perror("Error opening input file");
        return 1;
    }

    // Open the output file in binary mode
    outputFile = fopen(argv[2], "wb");
    if (!outputFile) {
        perror("Error opening output file");
        fclose(inputFile);
        return 1;
    }

    // Read from the input file and write to the output file
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), inputFile)) > 0) {
        fwrite(buffer, 1, bytesRead, outputFile);
    }

    fclose(inputFile);
    fclose(outputFile);

    printf("File converted to UTF-8 encoding successfully.\n");

    return 0;
}

