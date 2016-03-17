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

/* Function takes a watch discriptor, finds the path key with that discriptor and removes it
 * return -1 on error */
int rm_path_key(int wd)
{
	struct pathkey *pk;
	for(pk = root; pk->next != NULL; pk = pk->next)
		if(pk->wd == wd){

			/*If previous and next are null the root node is the only entry
			 * free resources and set root to NULL*/
			if(pk->previous == NULL & pk->next == NULL){
				free(pk->path);
				free(pk);
				root = NULL;
				return 0;
			}
			/*If previous is null and pk->next is NOT null we are removing the root node
			 * free resources in root and set root to root->next*/
			if(pk->previous == NULL & pk->next != NULL){
				struct pathkey *tmp = root;
				root = root->next;
				root->previous == NULL;
				free(tmp->path);
				free(tmp);
				return 0;
			}

			/*If we get here we are removing a node in the middle of two others.
			 * set pk->prev->next to pk->next and pk->next->prev pk->prv*/
			struct pathkey *tmp = pk;
			pk->previous->next = pk->next;
			pk->next->previous = pk->previous;
			free(tmp->path);
			free(tmp);
			return 0;

		}

	return -1;


}

	
	






