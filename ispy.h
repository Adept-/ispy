struct pathkey {
	int wd;
	char *path;
	struct pathkey *previous;
	struct pathkey *next;
};

struct pathkey *add_path_key(const char *path, const int wd);
int rm_path_key(const int wd);
struct pathkey *search_path_key(int wd);
struct pathkey *search_path(char *path);

