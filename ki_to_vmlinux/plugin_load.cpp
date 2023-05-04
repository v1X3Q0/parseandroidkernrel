#include <string>
#include <stdio.h>
#include <json.h>
#include <localUtil.h>

int pull_target_parameters(const char* target_a)
{
    int result = -1;

    // jsmntok_t t[128]; /* We expect no more than 128 JSON tokens */
    // jsmn_parser p;

    json_object *jobj = 0;

    char *json_block = 0;
    size_t json_block_sz = 0;
    lh_entry* tmpent = 0;
    lh_table* tmptab = 0;

    if (target_a != 0)
    {
        SAFE_BAIL(block_grab(target_a, (void**)&json_block, &json_block_sz) != 0);

        json_tokener_parse(json_block);

        // populate segments
        json_object *parsed_segments = json_object_object_get(jobj, "segments");
        if (parsed_segments != 0)
        {
            tmptab = json_object_get_object(parsed_segments);
            if(tmptab != 0)
            {
                tmpent = tmptab->head;
            }
        }


        // popoulate sections

        json_object *parsed_sections = json_object_object_get(jobj, "sections");
    }

    result = 0;
fail:
    SAFE_FREE(json_block);
    return result;
}

// void thing()
// {
//         // parse the json and determine which sorters/generators i need to run
//     if (target_family_a != 0)
//     {
//         target_family = target_family_a;
//         json_file = fopen((target_family + ".json").c_str(), "r");

//         fseek(json_file, 0, SEEK_END);
//         json_block_sz = ftell(json_file);
//         fseek(json_file, 0, SEEK_SET);

//         json_block = (char *)malloc(json_block_sz);
//         fread(json_block, 1, json_block_sz, json_file);

//         jobj = json_tokener_parse(json_block);

//         // jsmn_init(&p);
//         // jsmn_parse(&p, json_block, json_block_sz, t, 128); // "s" is the char array holding the json content

//         dlopen(("lib" + target_family + ".so").c_str(), RTLD_NOW);
//     }

//     if (target_device_a != 0)
//     {
//         target_device = target_device_a;
//         fopen((target_device + ".json").c_str(), "r");
//         dlopen(("lib" + target_device + ".so").c_str(), RTLD_NOW);
//     }

// }