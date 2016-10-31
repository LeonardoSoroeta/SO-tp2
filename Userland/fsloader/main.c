#include <stdio.h>
#include <stdint.h>
#include <dirent.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>

char* file_contents(char* path, uint32_t* size) {

	FILE *f;
	char *buffer;

	f = fopen(path, "rb");
	
	if (!f) {
		printf("%s did not open %d %s\n", path, errno, strerror(errno));
		exit(1);
	}

	fseek(f, 0, SEEK_END);
	*size = ftell(f);
	fseek(f, 0, SEEK_SET);

	buffer = (char*)calloc(*size+2, sizeof(char));

	fread(buffer, 1, *size, f);

	fclose(f);

	*size += 2;

	// buffer[*size++] = '\0';

	return buffer;

}

int canSkipPath(char *path) {
	return path[0] != '.';
}

int isBMP(char *name) {
	if ( strcmp(name+(strlen(name)-3), "bmp") == 0)
		return 1;
	return 0;
}

int zeroes(int n) {

	if ( n % 4 == 0 )
		return 0;

	return  (n / 4 + 1) * 4 - n;

}

typedef struct {
	uint32_t size;
	char *name;
	uint32_t offset;
} file_t;

int main(void) {

	FILE* output, *headers;
	char *buffer, *path;
	uint32_t size;
	DIR* dir;
	struct dirent* dirContent = NULL;
	uint32_t writtenFiles = 0;
	file_t files[2048];
	char aux[2048];
	uint32_t i = 0, acum = 0, aux2 = 0, j = 0;

	output = fopen("content.bin", "w+b");

	if (!output) {
		printf("output did not open\n");
		exit(1);
	}

	// buffer = file_contents("./payload/songs", &size);
	// files[0].size = size;
	// files[0].name = "songs";

	// printf("Writing %u bytes from songs\n", size);
	// fwrite(buffer, sizeof(char), size, output);
	// writtenFiles++;

	dir = opendir("./payload");

	while ( (dirContent = readdir(dir)) != NULL ) {

		if (canSkipPath(dirContent->d_name)) {
			// create string with path
			path = (char*)calloc(1, strlen("./payload/")+strlen(dirContent->d_name)+1);
			sprintf(path, "./payload/%s", dirContent->d_name);
			// read contents and get size
			buffer = file_contents(path, &size);
			// write to contents
			printf("Writing %u bytes from %s\n", size-2, dirContent->d_name);
			fwrite(buffer, sizeof(char), size, output);
			files[writtenFiles].size = size;
			files[writtenFiles++].name = strdup(dirContent->d_name);
		}
	}

	closedir(dir);
	fclose(output);

	printf("Writing headers\n");
	headers = fopen("headers.bin", "w+b");

	if (!headers) {
		printf("ouput failed to open\n");
		exit(2);
	}

 	/**
 	 *
 	 *  total_files | length_nombre | nombre
 	 * 
 	 */

 	// total files
 	fwrite(&writtenFiles, sizeof(uint32_t), 1, headers);

 	char *space = "\0";

	acum = sizeof(uint32_t);
	for (; i < writtenFiles; i++) {

		aux2 = strlen(files[i].name);
		// write length of name
		fwrite(&aux2, sizeof(uint32_t), 1, headers);
		// write name
		fwrite(files[i].name, sizeof(char), aux2, headers);
		// zero fill for alignment
		for (j = 0; j < zeroes(aux2); j++) {
			fwrite(space, sizeof(char), 1, headers);
		}
		printf("%d %d\n", aux2, zeroes(aux2));

		acum += zeroes(aux2)+aux2+sizeof(uint32_t);
	}

	
	acum += writtenFiles*2*sizeof(uint32_t);

	for (i = 0; i < writtenFiles; i++) {
		fwrite(&acum, sizeof(uint32_t), 1, headers);
		fwrite(&files[i].size, sizeof(uint32_t), 1, headers);
		acum += files[i].size;
	}

	fclose(headers);
	printf("close\n");


	return 0;
}




