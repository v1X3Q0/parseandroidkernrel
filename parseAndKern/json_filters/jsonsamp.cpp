#include <iostream>
#include <fstream>
#include <json.h>
#include <stdio.h>

int main()
{
    FILE* file = fopen("elf_helpers.json", "r");
    size_t filesz = 0;
    fseek(file, 0, SEEK_END);
    filesz = ftell(file);
    fseek(file, 0, SEEK_SET);

    char * jsonstring = (char*)malloc(filesz);
    fread(jsonstring, 1, filesz, file);
    fclose(file);
    json_object * jobj = json_tokener_parse(jsonstring);
    return 0;
}