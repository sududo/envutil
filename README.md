## envutil
# A small c library for loading .env files.

This library can be used both to load a .env file into your c program's environment
and to lookup a singular value in a file.

# Usage

Just add a *#include "envutil.h"* and call either *env_load* or *env_lookup*.

# env_load

> enum env_load_err env_load(const char *restrict filePath, const bool overwrite);

*env_load* takes as arguments a pointer to a string holding the .env file's path (*filePath*),
and a boolean (*overwrite*) that controls whether a found environment variable should overwrite an already
present variable.

If *filePath* is a NULL pointer, the function will default to the path *"./.env"* (a file named '.env'
in the current directory).

This function modifies the current program's environment, which means you can simply call *getenv* from *stdlib.h*
to get the values of any loaded environment variables.

In case of an error, this function will imediately stop parsing, and return an apropriate *env_load_err* enum.
Those are the values of this enum:
-ENV_LOAD_OK : The file was succesfully parsed, and all of it's values were loaded into the environment
-ENV_LOAD_ERR_NO_FILE : The given file path was invalid (in case of passing in NULL, a '.env' file was not
 present in the current directory)
-ENV_LOAD_ERR_LINE_EXCEED_CAPAC : The found file has a line which contains more characters then the buffer's
 capacity (look below)
-ENV_LOAD_ERR_SETENV_ERR : An error occured while trying to set the environment
-ENV_LOAD_ERR_PARSE_ERR : A file was found, but it's structure is invalid
-ENV_LOAD_ERR_FILE_READ_ERR : An error occured while trying to read from the file

Certain buffers used during parsing have pre-defined size limits,
> #define ENV_UTIL_MAX_BUFF_SIZE 4096
> #define ENV_UTIL_BUFF_LOAD_SIZE 64
> #define ENV_UTIL_KEY_BUFF_SIZE 1024
> #define ENV_UTIL_VALUE_BUFF_SIZE 2048
though theese can be overwritten by defining *ENV_UTIL_OVERRIDE_SIZES* and the 4 macros above

# env_lookup

> enum env_lookup_err env_lookup(const char *restrict filePath, const char \*restrict matchKey, char *restrict outValue, const size_t outValueSize);

*env_lookup* takes as arguments a pointer to a string holding the .env file's path (*filePath*),
a pointer to a string holding the key of the value that you're trying to lookup (*matchKey*), a pointer to a
buffer into which the found value will be copied(*outValue*, and the size of that buffer (*outValueSize*)

If *filePath* is a NULL pointer, the function will default to the path *"./.env"* (a file named '.env'
in the current directory).

If *matchKey* is a NULL pointer, the function will return an apropriate error.
If *outValue* is a NULL pointer, the function will return an apropriate error.
If *outValueSize* is 0, the function will return an apropriate error.

This function will at first try to find the specified key in the given file, and if it is not found, it will
look through the current environment.

In case of an error, this function will imediately stop parsing, and return an apropriate *env_lookup_err* enum.
Those are the values of this enum:
-ENV_LOOK_OK_FILE : The value was succesfully found in the specified file
-ENV_LOOK_OK_ENV : The value was succesfully found in the environment
-ENV_LOOK_ERR_NO_FILE : The given file path was invalid (in case of passing in NULL, a '.env' file was not
 present in the current directory)
-ENV_LOOK_ERR_LINE_EXCEED_CAPAC : The found file has a line which contains more characters then the buffer's
 capacity (look above)
-ENV_LOOK_ERR_PARSE_ERR : A file was found, but it's structure is invalid
-ENV_LOOK_ERR_FILE_READ_ERR : An error occured while trying to read from the file
-ENV_LOOK_ERR_KEY_NOT_FOUND : The specified key was neither found in the specified file nor the environment
-ENV_LOOK_ERR_VALUE_EXCEED_CAPAC : The value was found, but it's lenght was bigger than *outValueSize*
-ENV_LOOK_ERR_MATCH_KEY_NULL : *matchKey* is NULL
-ENV_LOOK_ERR_OUT_VALUE_NULL : *outValue* is NULL
-ENV_LOOK_ERR_OUT_CAPAC_ZERO : *outValueSize* is 0
