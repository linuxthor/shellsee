#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <time.h>
#include <sqlite3.h>
#include <sys/types.h>

#define QUOTE(...) #__VA_ARGS__

typedef pid_t (*orig_fork_type)(void);

pid_t fork(void)
{
    unsigned long *p;
    sqlite3 *db;
    char *insrt, *err_msg = 0;
    char user_id[16];
    char timestamp[64];
    int rc; 
    time_t nowtime;

    sqlite3_open(".shell.db",&db);
    if(db == NULL)
    {
        goto no_no;
    }

    const char *create_query = QUOTE(
      CREATE TABLE IF NOT EXISTS history ( cmd TEXT, user TEXT, time TEXT ));    

    rc = sqlite3_exec(db, create_query, 0, 0, &err_msg);
    if (rc != SQLITE_OK )
    {
        sqlite3_free(err_msg);
        goto no_no;
    }

    p = __builtin_frame_address(0)+8;
    void *addr = (void*)*p;
    
    nowtime = time(NULL);    
    strftime (timestamp, 64, "%H:%M:%S %d/%m/%Y", localtime (&nowtime));

    sprintf(user_id,"%d",getuid());

    insrt = sqlite3_mprintf("INSERT INTO history VALUES(%Q, %Q, %Q)",addr,user_id,timestamp);
    rc = sqlite3_exec(db, insrt, 0, 0, &err_msg);
    if (rc != SQLITE_OK )
    {
        sqlite3_free(err_msg);
    }

no_no: 
      asm("nop");      
      orig_fork_type orig_fork;
      orig_fork = (orig_fork_type)dlsym(RTLD_NEXT,"fork");
      return orig_fork();
}
