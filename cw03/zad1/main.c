#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <time.h>
#include <ftw.h>

time_t t;

void find_less(char* dir_name){
    
    DIR* handle = opendir(dir_name);
    if (handle == NULL){
        perror(dir_name);
        return;
    }
    chdir(dir_name);

    struct dirent* entity = readdir(handle);

    while(entity != NULL){
        
        struct stat file_stats;
        lstat(entity->d_name, &file_stats);

        if (S_ISREG(file_stats.st_mode)){

            if (difftime(file_stats.st_mtime, t) < 0){
                
                printf( (file_stats.st_mode & S_IRUSR) ? "r" : "-");
                printf( (file_stats.st_mode & S_IWUSR) ? "w" : "-");
                printf( (file_stats.st_mode & S_IXUSR) ? "x" : "-");
                printf( (file_stats.st_mode & S_IRGRP) ? "r" : "-");
                printf( (file_stats.st_mode & S_IWGRP) ? "w" : "-");
                printf( (file_stats.st_mode & S_IXGRP) ? "x" : "-");
                printf( (file_stats.st_mode & S_IROTH) ? "r" : "-");
                printf( (file_stats.st_mode & S_IWOTH) ? "w" : "-");
                printf( (file_stats.st_mode & S_IXOTH) ? "x\t" : "-\t");
                
                printf("%ld\t", file_stats.st_size);
                char date[32];
                strftime(date, 32, "%Y %b %d %H:%M:%S", localtime(&(file_stats.st_ctime)));
                printf("%s\t", date);

                char dir[1024];
                getcwd(dir, sizeof(dir));
                printf("%s/%s\n",dir, entity->d_name);
            }
        }

        else if ((strcmp(entity->d_name, ".") != 0) && (strcmp(entity->d_name, "..") != 0) && S_ISDIR(file_stats.st_mode)){
            
            pid_t id = fork();
            if (id == 0){
                find_less(entity->d_name);
                exit(0);
            }
            //chdir("..");

        }

        entity = readdir(handle);

    }

    closedir(handle);
}

void find_greater(char* dir_name){

    DIR* handle = opendir(dir_name);
    if (handle == NULL){
        perror(dir_name);
        return;
    }
    chdir(dir_name);

    struct dirent* entity = readdir(handle);

    while(entity != NULL){
        
        struct stat file_stats;
        lstat(entity->d_name, &file_stats);

        if (S_ISREG(file_stats.st_mode)){

            if (difftime(file_stats.st_mtime, t) > 0){

                printf( (file_stats.st_mode & S_IRUSR) ? "r" : "-");
                printf( (file_stats.st_mode & S_IWUSR) ? "w" : "-");
                printf( (file_stats.st_mode & S_IXUSR) ? "x" : "-");
                printf( (file_stats.st_mode & S_IRGRP) ? "r" : "-");
                printf( (file_stats.st_mode & S_IWGRP) ? "w" : "-");
                printf( (file_stats.st_mode & S_IXGRP) ? "x" : "-");
                printf( (file_stats.st_mode & S_IROTH) ? "r" : "-");
                printf( (file_stats.st_mode & S_IWOTH) ? "w" : "-");
                printf( (file_stats.st_mode & S_IXOTH) ? "x\t" : "-\t");
                
                printf("%ld\t", file_stats.st_size);

                char date[32];
                strftime(date, 32, "%Y %b %d %H:%M:%S", localtime(&(file_stats.st_ctime)));
                printf("%s\t", date);

                char dir[1024];
                getcwd(dir, sizeof(dir));
                printf("%s/%s\n",dir, entity->d_name);
            }
        }

        else if ((strcmp(entity->d_name, ".") != 0) && (strcmp(entity->d_name, "..") != 0) && S_ISDIR(file_stats.st_mode)){
            
            pid_t id = fork();
            if (id == 0){
                find_greater(entity->d_name);
                exit(0);
            }
            //chdir("..");

        }

        entity = readdir(handle);

    }

    closedir(handle);
}

void find_eq(char* dir_name){
    
    DIR* handle = opendir(dir_name);
    if (handle == NULL){
        perror(dir_name);
        return;
    }
    chdir(dir_name);

    struct dirent* entity = readdir(handle);

    while(entity != NULL){
        
        struct stat file_stats;
        lstat(entity->d_name, &file_stats);

        if (S_ISREG(file_stats.st_mode)){

            if (difftime(file_stats.st_mtime, t) == 0){

                printf( (file_stats.st_mode & S_IRUSR) ? "r" : "-");
                printf( (file_stats.st_mode & S_IWUSR) ? "w" : "-");
                printf( (file_stats.st_mode & S_IXUSR) ? "x" : "-");
                printf( (file_stats.st_mode & S_IRGRP) ? "r" : "-");
                printf( (file_stats.st_mode & S_IWGRP) ? "w" : "-");
                printf( (file_stats.st_mode & S_IXGRP) ? "x" : "-");
                printf( (file_stats.st_mode & S_IROTH) ? "r" : "-");
                printf( (file_stats.st_mode & S_IWOTH) ? "w" : "-");
                printf( (file_stats.st_mode & S_IXOTH) ? "x\t" : "-\t");
                
                printf("%ld\t", file_stats.st_size);

                char date[32];
                strftime(date, 32, "%Y %b %d %H:%M:%S", localtime(&(file_stats.st_ctime)));
                printf("%s\t", date);

                char dir[1024];
                getcwd(dir, sizeof(dir));
                printf("%s/%s\n",dir, entity->d_name);
            }
        }

        else if ((strcmp(entity->d_name, ".") != 0) && (strcmp(entity->d_name, "..") != 0) && S_ISDIR(file_stats.st_mode)){
            
            pid_t id = fork();
            if (id == 0){
                find_eq(entity->d_name);
                exit(0);
            }
            //chdir("..");

        }

        entity = readdir(handle);

    }

    closedir(handle);
}

int nftw_eq_fn(const char* file_name, const struct stat* file_stats, int flags, struct FTW* ftw_buf){

    if (flags == FTW_F){

            if (difftime(file_stats->st_mtime, t) == 0){

                printf( (file_stats->st_mode & S_IRUSR) ? "r" : "-");
                printf( (file_stats->st_mode & S_IWUSR) ? "w" : "-");
                printf( (file_stats->st_mode & S_IXUSR) ? "x" : "-");
                printf( (file_stats->st_mode & S_IRGRP) ? "r" : "-");
                printf( (file_stats->st_mode & S_IWGRP) ? "w" : "-");
                printf( (file_stats->st_mode & S_IXGRP) ? "x" : "-");
                printf( (file_stats->st_mode & S_IROTH) ? "r" : "-");
                printf( (file_stats->st_mode & S_IWOTH) ? "w" : "-");
                printf( (file_stats->st_mode & S_IXOTH) ? "x\t" : "-\t");
                
                printf("%ld\t", file_stats->st_size);

                char date[32];
                strftime(date, 32, "%Y %b %d %H:%M:%S", localtime(&(file_stats->st_ctime)));
                printf("%s\t", date);

                char dir[1024];
                realpath(file_name, dir);
                printf("%s\n", dir);
            }
    }
    return 0;
}

int nftw_less_fn(const char* file_name, const struct stat* file_stats, int flags, struct FTW* ftw_buf){

    if (flags == FTW_F){

            if (difftime(file_stats->st_mtime, t) < 0){

                printf( (file_stats->st_mode & S_IRUSR) ? "r" : "-");
                printf( (file_stats->st_mode & S_IWUSR) ? "w" : "-");
                printf( (file_stats->st_mode & S_IXUSR) ? "x" : "-");
                printf( (file_stats->st_mode & S_IRGRP) ? "r" : "-");
                printf( (file_stats->st_mode & S_IWGRP) ? "w" : "-");
                printf( (file_stats->st_mode & S_IXGRP) ? "x" : "-");
                printf( (file_stats->st_mode & S_IROTH) ? "r" : "-");
                printf( (file_stats->st_mode & S_IWOTH) ? "w" : "-");
                printf( (file_stats->st_mode & S_IXOTH) ? "x\t" : "-\t");
                
                printf("%ld\t", file_stats->st_size);

                char date[32];
                strftime(date, 32, "%Y %b %d %H:%M:%S", localtime(&(file_stats->st_ctime)));
                printf("%s\t", date);

                char dir[1024];
                realpath(file_name, dir);
                printf("%s\n", dir);
            }
    }
    return 0;
}

int nftw_greater_fn(const char* file_name, const struct stat* file_stats, int flags, struct FTW* ftw_buf){

    if (flags == FTW_F){

            if (difftime(file_stats->st_mtime, t) > 0){

                printf( (file_stats->st_mode & S_IRUSR) ? "r" : "-");
                printf( (file_stats->st_mode & S_IWUSR) ? "w" : "-");
                printf( (file_stats->st_mode & S_IXUSR) ? "x" : "-");
                printf( (file_stats->st_mode & S_IRGRP) ? "r" : "-");
                printf( (file_stats->st_mode & S_IWGRP) ? "w" : "-");
                printf( (file_stats->st_mode & S_IXGRP) ? "x" : "-");
                printf( (file_stats->st_mode & S_IROTH) ? "r" : "-");
                printf( (file_stats->st_mode & S_IWOTH) ? "w" : "-");
                printf( (file_stats->st_mode & S_IXOTH) ? "x\t" : "-\t");
                
                printf("%ld\t", file_stats->st_size);

                char date[32];
                strftime(date, 32, "%Y %b %d %H:%M:%S", localtime(&(file_stats->st_ctime)));
                printf("%s\t", date);

                char dir[1024];
                realpath(file_name, dir);
                printf("%s\n", dir);
            }
    }
    return 0;
}

void find_less_nftw(char* dir_name){
    int nftw_res = nftw(dir_name, nftw_less_fn, 5, FTW_PHYS);
    if (nftw_res < 0){
        perror(dir_name);
    }
}

void find_eq_nftw(char* dir_name){
    int nftw_res = nftw(dir_name, nftw_eq_fn, 5, FTW_PHYS);
    if (nftw_res < 0){
        perror(dir_name);
    }
}

void find_greater_nftw(char* dir_name){
    int nftw_res = nftw(dir_name, nftw_greater_fn, 5, FTW_PHYS);
    if (nftw_res < 0){
        perror(dir_name);
    }
}

int main(int argc, char* argv[]){

    if (argc < 5){
        printf("Too few arguments\n");
        return 0;
    }

    struct tm times = {0};

    if (strptime(argv[2],"%F@%T",&times) == NULL){
        puts("Incorrect time format");
        return 0;
    }
    t = mktime(&times);

    if (strcmp(argv[4], "mode1") == 0){

        if (strcmp(argv[3], "lt") == 0){
            find_less(argv[1]);
        }
        else if (strcmp(argv[3], "eq") == 0){
            find_eq(argv[1]);
        }
        else if (strcmp(argv[3], "gt") == 0){
            find_greater(argv[1]);
        }
        else{
            printf("Incorrect sign\n");
            return 0;
        }

    }
    else if (strcmp(argv[4], "mode2") == 0){

        if (strcmp(argv[3], "lt") == 0){
            find_less_nftw(argv[1]);
        }
        else if (strcmp(argv[3], "eq") == 0){
            find_eq_nftw(argv[1]);
        }
        else if (strcmp(argv[3], "gt") == 0){
            find_greater_nftw(argv[1]);
        }
        else{
            printf("Incorrect sign\n");
            return 0;
        }

    }
    else{
        printf("Incorrect mode\n");
        return 0;
    }
    return 0;
}