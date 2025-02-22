#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define COLOR_RESET "\x1b[0m"
#define COLOR_DIR "\x1b[34m"
#define COLOR_EXEC "\x1b[32m"
#define COLOR_REG "\x1b[0m"

#define VERSION "1.0"

typedef struct
{
	struct dirent* entry;
	char full_path[1024];
} dirent_with_path;

void print_size(off_t size, int human_readable)
{
	if(human_readable)
	{
		char buf[10];
		if(size >= (1 << 30))
		{
			snprintf(buf, sizeof(buf), "%.1fG", size / (double)(1 << 30));
		}
		else if(size >= (1 << 20))
		{
			snprintf(buf, sizeof(buf), "%.1fM", size / (double)(1 << 20));
		}
		else if(size >= (1 << 10))
		{
			snprintf(buf, sizeof(buf), "%.1fK", size / (double)(1 << 10));
		}
		else
		{
			snprintf(buf, sizeof(buf), "%ldB", size);
		}
		printf("%7s ", buf);
	}
	else
	{
		printf("%7ld ", size);
	}
}

int compare_by_time(const void* a, const void* b)
{
	struct stat stat_a, stat_b;
	const dirent_with_path* entry_a = (const dirent_with_path*)a;
	const dirent_with_path* entry_b = (const dirent_with_path*)b;

	if(stat(entry_a->full_path, &stat_a) == -1 || stat(entry_b->full_path, &stat_b) == -1)
	{
		perror("stat");
		return 0;
	}

	return stat_b.st_mtime - stat_a.st_mtime;
}

int compare_by_size(const void* a, const void* b)
{
	struct stat stat_a, stat_b;
	const dirent_with_path* entry_a = (const dirent_with_path*)a;
	const dirent_with_path* entry_b = (const dirent_with_path*)b;

	if(stat(entry_a->full_path, &stat_a) == -1 || stat(entry_b->full_path, &stat_b) == -1)
	{
		perror("stat");
		return 0;
	}

	return stat_b.st_size - stat_a.st_size;
}

void ls(const char* path,
		int long_listing,
		int recursive,
		int show_hidden,
		int human_readable,
		int sort_by_time,
		int sort_by_size)
{
	DIR* dir = opendir(path);
	if(dir == NULL)
	{
		perror("opendir");
		exit(EXIT_FAILURE);
	}

	struct dirent** namelist;
	int n = scandir(path, &namelist, NULL, alphasort);
	dirent_with_path* entries = (dirent_with_path*)malloc(n * sizeof(dirent_with_path));
	for(int i = 0; i < n; i++)
	{
		entries[i].entry = namelist[i];
		snprintf(
			entries[i].full_path, sizeof(entries[i].full_path), "%s/%s", path, namelist[i]->d_name);
	}

	if(sort_by_time)
	{
		qsort(entries, n, sizeof(dirent_with_path), compare_by_time);
	}
	else if(sort_by_size)
	{
		qsort(entries, n, sizeof(dirent_with_path), compare_by_size);
	}

	struct stat st;
	struct dirent* entry;
	for(int i = 0; i < n; i++)
	{
		entry = entries[i].entry;

		if(!show_hidden && entry->d_name[0] == '.')
		{
			free(entry);
			continue;
		}

		if(stat(entries[i].full_path, &st) == -1)
		{
			perror("stat");
			free(entry);
			continue;
		}

		if(long_listing)
		{
			struct passwd* pw = getpwuid(st.st_uid);
			struct group* gr = getgrgid(st.st_gid);

			char perms[11];
			perms[0] = S_ISDIR(st.st_mode) ? 'd' : '-';
			perms[1] = (st.st_mode & S_IRUSR) ? 'r' : '-';
			perms[2] = (st.st_mode & S_IWUSR) ? 'w' : '-';
			perms[3] = (st.st_mode & S_IXUSR) ? 'x' : '-';
			perms[4] = (st.st_mode & S_IRGRP) ? 'r' : '-';
			perms[5] = (st.st_mode & S_IWGRP) ? 'w' : '-';
			perms[6] = (st.st_mode & S_IXGRP) ? 'x' : '-';
			perms[7] = (st.st_mode & S_IROTH) ? 'r' : '-';
			perms[8] = (st.st_mode & S_IWOTH) ? 'w' : '-';
			perms[9] = (st.st_mode & S_IXOTH) ? 'x' : '-';
			perms[10] = '\0';

			char time_buf[64];
			strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M", localtime(&st.st_mtime));

			printf("%s %ld %s %s ", perms, st.st_nlink, pw->pw_name, gr->gr_name);
			print_size(st.st_size, human_readable);
			printf("%s ", time_buf);
		}
		if(S_ISDIR(st.st_mode))
		{
			printf(COLOR_DIR "%s\n" COLOR_RESET, entry->d_name);
		}
		else if(st.st_mode & S_IXUSR)
		{
			printf(COLOR_EXEC "%s\n" COLOR_RESET, entry->d_name);
		}
		else
		{
			printf(COLOR_REG "%s\n" COLOR_RESET, entry->d_name);
		}

		if(recursive && S_ISDIR(st.st_mode))
		{
			printf("\n%s:\n", entries[i].full_path);
			ls(entries[i].full_path,
			   long_listing,
			   recursive,
			   show_hidden,
			   human_readable,
			   sort_by_time,
			   sort_by_size);
		}
		free(entry);
	}

	free(entries);
	closedir(dir);
}

void print_help(const char* prog_name)
{
	printf("Usage: %s [options] [file...]\n", prog_name);
	printf("Options:\n");
	printf("  -l         Use a long listing format\n");
	printf("  -R         List subdirectories recursively\n");
	printf("  -a         Include hidden files\n");
	printf("  -h         Human readable sizes (e.g., 1K 234M 2G)\n");
	printf("  -S         Sort by file size\n");
	printf("  -t         Sort by modification time\n");
	printf("  --help     Display this help and exit\n");
	printf("  --version  Output version information and exit\n");
}

int main(int argc, char* argv[])
{
	int long_listing = 0;
	int recursive = 0;
	int show_hidden = 0;
	int human_readable = 0;
	int sort_by_time = 0;
	int sort_by_size = 0;
	char* path = ".";

	for(int i = optind; i < argc; i++)
	{
		if(strcmp(argv[i], "--help") == 0)
		{
			print_help(argv[0]);
			exit(EXIT_SUCCESS);
		}
		else if(strcmp(argv[i], "--version") == 0)
		{
			printf("%s version %s\n", argv[0], VERSION);
			exit(EXIT_SUCCESS);
		}
	}

	int opt;
	while((opt = getopt(argc, argv, "lRahSt")) != -1)
	{
		switch(opt)
		{
		case 'l':
			long_listing = 1;
			break;
		case 'R':
			recursive = 1;
			break;
		case 'a':
			show_hidden = 1;
			break;
		case 'h':
			human_readable = 1;
			break;
		case 'S':
			sort_by_size = 1;
			break;
		case 't':
			sort_by_time = 1;
			break;
		default:
			fprintf(stderr, "Usage: %s [-lRahSt] [file...]\n", argv[0]);
			exit(EXIT_FAILURE);
		}
	}

	if(optind < argc)
	{
		path = argv[optind];
	}

	ls(path, long_listing, recursive, show_hidden, human_readable, sort_by_time, sort_by_size);

	return 0;
}
