#include <stdlib.h>
#include <string.h>
#include "ispy.h"

struct pathkey *root = NULL;

/*If root is null this is the use of the list bootstrap it and return a pointer to the pathkey
 * if root is not null increment to the end of the list, add the new path key and return a pointer to it
 * */
struct pathkey *add_path_key(const char *path, const int wd)
{
	/* if root is null bootstrap the list */
	if(root == NULL){
                struct pathkey *pk = malloc(sizeof(struct pathkey));
		root = pk;
		root->wd = wd;
		root->path = malloc(strlen(path));
		root->path = strcpy(root->path, path);
		root->previous = NULL;
		root->next = NULL;
		return pk;
	}

	/*If we get here root is not null, add new path key to the end of the list */
	struct pathkey *p;
	struct pathkey *pk;
	for(p = root; p->next != NULL; p = p->next)
		;;
	
	pk = malloc(sizeof(struct pathkey));
	p->next = pk;
	pk->path = malloc(strlen(path));
	pk->path = strcpy(pk->path, path);
	pk->wd = wd;
	pk->previous = p;
	pk->next = NULL;

	return pk;
}


/* search the list for a pk with matching wd's and return a pointer to it
 * if it is not found return NULL */
struct pathkey *search_path_key(int wd)
{
	struct pathkey *p;

	for(p = root; p->next != NULL; p = p->next)
		if(p->wd == wd)
			return p;
	return NULL;
	
}

	
	
