#include <string>
#include <stdio.h>
#include <json.h>

int parseAndGetGlobals(const char *target_family_a, const char *target_device_a)
{
    int result = -1;
    std::string target_family;
    std::string target_device;

    // jsmntok_t t[128]; /* We expect no more than 128 JSON tokens */
    // jsmn_parser p;

    json_object *jobj = 0;

    FILE *json_file = 0;
    char *json_block = 0;
    size_t json_block_sz = 0;

    // parse the json and determine which sorters/generators i need to run
    if (target_family_a != 0)
    {
        target_family = target_family_a;
        json_file = fopen((target_family + ".json").c_str(), "r");

        fseek(json_file, 0, SEEK_END);
        json_block_sz = ftell(json_file);
        fseek(json_file, 0, SEEK_SET);

        json_block = (char *)malloc(json_block_sz);
        fread(json_block, 1, json_block_sz, json_file);

        jobj = json_tokener_parse(json_block);

        // jsmn_init(&p);
        // jsmn_parse(&p, json_block, json_block_sz, t, 128); // "s" is the char array holding the json content

        dlopen(("lib" + target_family + ".so").c_str(), RTLD_NOW);
    }

    if (target_device_a != 0)
    {
        target_device = target_device_a;
        fopen((target_device + ".json").c_str(), "r");
        dlopen(("lib" + target_device + ".so").c_str(), RTLD_NOW);
    }

    result = 0;
fail:
    SAFE_FREE(json_block);
    SAFE_FCLOSE(json_file);
    return result;
}
