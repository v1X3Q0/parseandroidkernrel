#include <stdio.h>
#include <dirent.h>
#include <vector>
#include <string>
#include <libgen.h>
#include <string.h>

#include <localUtil.h>
#include "iterate_dir.h"

int get_libmodules(const char* libmod_path, std::vector<std::string>* driverNames)
{
    struct dirent *de; // Pointer for directory entry
    int result = -1;
    std::string tmp_file;
    char libmod_pathCpy[PATH_MAX] = { 0 };
    std::string targ_dir;
    
    strcpy(libmod_pathCpy, libmod_path);
    targ_dir = std::string(dirname((char*)libmod_pathCpy));
    strcpy(libmod_pathCpy, libmod_path);
    targ_dir += "/" + std::string(basename((char*)libmod_path));

    // opendir() returns a pointer of DIR type.
    DIR *dr = opendir(libmod_path);

    SAFE_BAIL(dr == NULL);

    // Refer http://pubs.opengroup.org/onlinepubs/7990989775/xsh/readdir.html
    // for readdir()
    while ((de = readdir(dr)) != NULL)
    {
        tmp_file = std::string((const char*)(de->d_name));
        if ((tmp_file == ".") || (tmp_file == ".."))
        {
            continue;
        }
        driverNames->push_back(targ_dir + "/" + tmp_file);
        // printf("%s\n", de->d_name);
    }

    closedir(dr);

    result = 0;
fail:
    return result;
}
