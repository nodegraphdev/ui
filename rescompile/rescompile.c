
// Purpose of this is to intake resource files and embed them into a large C file 
//
// rescompile <output.c> <input_1.ext> <input_2.ext> <input_3.ext>
//
//
// TODO:
//   Prevent variable name overlap
//   Maybe preprocess shaders for include?
//
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

int main(int argc, char** args)
{
	if (argc < 2)
	{
		printf("rescompile: Missing arguments! Expected use:\n\trescompile output.c input_1.ext input_2.ext ...\n");
		return 1;
	}

	// Open up the file for writing
	FILE* out_c = fopen(args[1], "wb");
	if (!out_c)
	{
		printf("rescompile: Failed to open file %s!\n", args[1]);
		return 1;
	}
	printf("rescompile! %s\n", args[1]);

	time_t t = time(NULL);
	struct tm lt = *localtime(&t);
	
	fprintf(out_c, "// Rescompile autogenerated file\n// Compiler date: " __DATE__ " " __TIME__ "\n// Generated: %d/%d/%d %02d:%02d:%02d\n", 1900 + lt.tm_year, 1 + lt.tm_mon, lt.tm_mday, lt.tm_hour, lt.tm_min, lt.tm_sec);
	fprintf(out_c, "// Output:\n// \t%s\n// Inputs:\n", args[1]);
	for (int i = 2; i < argc; i++)
		fprintf(out_c, "// \t%s\n", args[i]);
	fprintf(out_c, "//\n\n\n");


	// Hex character map for converting bytes to hex
	const char* hex_chars = "0123456789ABCDEF";

	// Little buffer for working on strings
	char working[512];

	// Loop over all files
	for (int i = 2; i < argc; i++)
	{
		// First read the file's contents to a buffer
		const char* path = args[i];
		FILE* f = fopen(path, "rb");
		if (!f)
		{
			printf("rescompile: Failed to open file %s!\n", path);
			return 1;
		}


		// Copy all data out of the file
		fseek(f, 0, SEEK_END);
		size_t sz = ftell(f);
		fseek(f, 0, SEEK_SET);

		unsigned char* m = calloc(sz + 1, 1);
		fread(m, 1, sz, f);

		// Don't need the file anymore
		fclose(f);


		// Clip the file name off from the rest of the path
		char* filename = strrchr(path, '/');
		if (!filename)
			filename = strrchr(path, '\\');
		if (filename) 
			filename++;
		else
			filename = path;
		strncpy(working, filename, 512);
		size_t len = strlen(working);

		// Convert the file name into a variable name

		// No numbers as the first character
		if (!isalpha(working[0]))
			working[0] = '_';
		// Replace all non alphanumeric with _
		for (size_t k = 1; k < len; k++)
			if(!isalnum(working[k]))
				working[k] = '_';
		
		// Emit the file path
		fprintf(out_c, "// Input: %s\n", path);

		// Emit the array length variable
		fwrite("const unsigned long long ", 1, 25, out_c);
		fwrite(working, 1, len, out_c);
		fprintf(out_c, "_length = %zu;\n", sz);

		// Emit the array variable name
		fwrite("const unsigned char ", 1, 20, out_c);
		fwrite(working, 1, len, out_c);
		fwrite("[] = {\n", 1, 7, out_c);


		// Copy in a template
		strcpy(working, "0x00, ");

		if (sz == 0)
		{
			// Need at least something so we can compile
			fwrite("0 /*file empty*/", 1, 16, out_c);
		}
		else
		{

			// Convert the bytes to hex and pour them into our output
			for (size_t k = 0; k < sz; k++)
			{
				unsigned char byte = m[k];

				working[2] = hex_chars[byte / 16];
				working[3] = hex_chars[byte % 16];
				working[5] = k % 16 == 15 ? '\n' : ' ';
				fwrite(working, 1, 6, out_c);

			}

			// FIXME: For now we're null terminating all files, as we're only using rescompile for text at the moment
			fwrite("0", 1, 1, out_c);
		}

		// End of variable
		fwrite("\n};\n\n", 1, 5, out_c);

		// Free the buffer
		free(m);
	}
	
	// Done!
	fclose(out_c);

	return 0;
}

