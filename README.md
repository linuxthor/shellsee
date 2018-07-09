# shellsee

LD_PRELOAD hack to save shell commands in an sqlite database. 

Hooks calls to fork() in order to extract the command being run and saves it to a database (along with userid and timestamp.)

Build it with:

```
gcc -Wall -o shell_hook.so shell_hook.c -shared -ldl -lsqlite3
```

Load it with:
```
LD_PRELOAD=$(pwd)/shell_hook.so /bin/bash
```
