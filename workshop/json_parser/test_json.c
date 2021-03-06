#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/stat.h>
#include <assert.h>

#include "jsonparse.h"
#include "jsontree.h"

#define MAX_FILE_SIZE (1024)

#define DISPLAY_INPUT_HEX 1
#define DISPLAY_INPUT_PLAIN 1

uint8_t* read_data_from_file(const char *path, size_t *ptr_data_size)
{
	FILE *file_desc;
	int result = -1;
	uint8_t *buffer = NULL;

	if (path == NULL)
		return NULL;

	struct stat stat_str;
	result = stat(path, &stat_str);
	if (result != 0)
		return NULL;		

	file_desc = fopen(path, "r");
	if (file_desc == NULL)
		return NULL;
	size_t data_size = stat_str.st_size;

	buffer = malloc(data_size+1);
	if (buffer == NULL)
		return NULL;

	buffer[data_size] = 0;

	size_t data_size_read = fread(buffer, 1, data_size, file_desc);

	assert(data_size_read == data_size);
	*ptr_data_size = data_size;

#ifdef DISPLAY_INPUT_HEX
	for (int i=0; i<data_size+1; ++i)
		printf("%02hhX ", buffer[i] & 0xff);
	printf("\n");
#endif // #ifdef DISPLAY_INPUT_HEX

#ifdef DISPLAY_INPUT_PLAIN
 	printf("-----------------------------\n%s\n-------------------------\n", buffer);
#endif // #ifdef DISPLAY_INPUT_PLAIN

	fclose(file_desc);
	return buffer;
}

uint8_t *global_pkt_buf = NULL;
size_t global_data_size = 0;

char local_buffer[1024];

#ifdef INPUT_STDIN

int main(int argc, char** argv) 
{
	char global_pkt_buf[MAX_FILE_SIZE+1];

	ssize_t global_data_size = read(STDIN_FILENO, global_pkt_buf, MAX_FILE_SIZE);
	global_pkt_buf[global_data_size] = 0;

	printf("size of input data = %d\n", (int) global_data_size);

	printf("start parsing!!\n");

	struct jsonparse_state state;
	jsonparse_setup(&state, global_pkt_buf, global_data_size);
	printf("start parsing!!\n");
	int result = 0;

	while((result = jsonparse_next(&state)) != 0) {
		printf("result = %d\n", result);
		memset(local_buffer, 0, 1024);
		jsonparse_copy_value(&state, local_buffer, 1023);
		printf("value = %s\n", local_buffer);
	}
	printf("result = %d\n", result);
	printf("finished parsing!!\n");

	return 0;
}
	
#else

int main( int argc, char * argv[] )
{
	int i = 0;
	if (argc == 2) {
		global_pkt_buf = read_data_from_file(argv[1], &global_data_size);
	} else {
		printf("Wrong number of arguments = %d - should be 1!\n", argc-1);
		return -1;
	}

	printf("Read packet = %p\n", global_pkt_buf);
	printf("size of input data = %zu\n", global_data_size);

	if (global_data_size == 0)
	{
		printf("Data sample is empty!\n");
		return -1;
	}


	struct jsonparse_state state;
	jsonparse_setup(&state, global_pkt_buf, global_data_size);
	printf("start parsing!!\n");
	int result = 0;

	while((result = jsonparse_next(&state)) != 0) {
		printf("result = %d\n", result);
		memset(local_buffer, 0, 1024);
		jsonparse_copy_value(&state, local_buffer, 1023);
		printf("value = %s\n", local_buffer);
	}
	printf("result = %d\n", result);	

 	free(global_pkt_buf);
	return 0;
}

#endif

