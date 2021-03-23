#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <errno.h>

#define DEBUG 0

#define BUFFER_SIZE 4096
#define JPEG_START "\xFF\xD8"
#define JPEG_END "\xFF\xD9"

#define USAGE(string) { \
            if (string!=NULL) puts(#string); \
            printf("Use: %s input_image output_image\n", argv[0]); \
            exit(1); \
          }

#define CHECKPTR(ptr,string) { \
            if (!ptr) {puts(#string); exit(2);} \
          }

char* my_strstr(char* haystack, int haystack_len, char* needle);

int main (int argc, char** argv){

	if (argc != 3) {
		USAGE("Number of arguments is invalid\n");
		exit(1);
	}

	FILE* input_file = fopen(argv[1], "rb");
	CHECKPTR(input_file, "Could't open input file\n");
	FILE* output_file = fopen(argv[2], "wb");
	CHECKPTR(output_file, "Could't open output file\n");

	char* buffer = malloc(sizeof(char)*BUFFER_SIZE);
	CHECKPTR(buffer, "Malloc error\n");
	memset(buffer, '\0', sizeof(char)*BUFFER_SIZE);

	int found_start = 0;
	int found_end = 0;

	int buffer_count;
	while ((buffer_count = fread(buffer, sizeof(char), BUFFER_SIZE, input_file)) && !found_end){

		if (DEBUG) printf("buffer_count: %d\n", buffer_count);

		ptrdiff_t bytes_skipped_start = 0;
		ptrdiff_t bytes_skipped_end = 0;

		if (!found_start){
			char* start_position = my_strstr(buffer, BUFFER_SIZE, JPEG_START);
			if (start_position){
				found_start = 1;
				bytes_skipped_start = start_position - buffer;
				if (DEBUG) printf("bytes_skipped_start: %ld\n", bytes_skipped_start);
			}
		}

		if (found_start && !found_end){
			char* end_position = my_strstr(buffer, BUFFER_SIZE, JPEG_END);
			if (end_position){
				end_position+=2;
				found_end = 1;
				bytes_skipped_end = buffer + BUFFER_SIZE - end_position;
				if (DEBUG) printf("bytes_skipped_end: %ld\n", bytes_skipped_end);
			}
		}

		if (found_start){
			int bytes_written = fwrite(buffer + bytes_skipped_start, sizeof(char), BUFFER_SIZE - bytes_skipped_start - bytes_skipped_end, output_file);
			if (bytes_written != BUFFER_SIZE - bytes_skipped_start - bytes_skipped_end) {
				fprintf(stderr, "Uncomplete write, output file might be corrupted. Proceeding anyway\n");
			}
		}

		memset(buffer, '\0', sizeof(char)*BUFFER_SIZE);

	}

	if (DEBUG) printf("buffer_count (out of the loop): %d\n", buffer_count);

	if (fclose(input_file)) perror("fclose");
	if (fclose(output_file)) perror("fclose");

	return 0;

}

char* my_strstr(char* haystack, int haystack_len, char* needle){

	int needle_len = strlen(needle);
	int max_len = haystack_len - needle_len + 1;

	int index = 0;
	while ( index < max_len){
		if ( !strncmp(haystack, needle, needle_len) ) return haystack;
		++index;
		++haystack;
	}

	return NULL;

}