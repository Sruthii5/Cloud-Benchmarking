#define _GNU_SOURCE
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/time.h>
#include<time.h>
#include<pthread.h>
#include<unistd.h>
#include<fcntl.h>
#include<math.h>

#define FILE_SIZE 10737418240

char file_names[48][10] = {"file1.txt", "file2.txt", "file3.txt", "file4.txt", "file5.txt", "file6.txt", "file7.txt", "file8.txt", "file9.txt", "file10.txt", "file11.txt", "file12.txt", "file13.txt", "file14.txt", "file15.txt", "file16.txt", "file17.txt", "file18.txt", "file19.txt", "file20.txt", "file21.txt", "file22.txt", "file23.txt", "file24.txt", "file25.txt", "file26.txt", "file27.txt", "file28.txt", "file29.txt", "file30.txt", "file31.txt", "file32.txt", "file33.txt", "file34.txt", "file35.txt", "file36.txt", "file37.txt", "file38.txt", "file39.txt", "file40.txt", "file41.txt", "file42.txt", "file43.txt", "file44.txt", "file45.txt", "file46.txt", "file47.txt", "file48.txt"};

pthread_mutex_t mutex;

struct pthreadArgs {
    int record_size;
    int no_of_files;
};

int file_no = 0;

void* sequentialWrite(void *arg)
{
   struct pthreadArgs *params = (struct pthreadArgs*) arg;
   static int file_n = 1;

   char* buffer = calloc(params -> record_size,  sizeof(char));
   memset(buffer, 'a', params -> record_size);

   long long int i, j;
   long int single_file_size = FILE_SIZE / params -> no_of_files;

   char file_name[15];
   snprintf(file_name, 15, "file%d.txt", file_n++);
   int fc = open(file_name, O_WRONLY | O_CREAT | O_TRUNC | O_SYNC , S_IRWXU | S_IRGRP);
   if(fc == -1)
          printf("file not opened\n");
   int no_of_chunks = single_file_size / params -> record_size;

    for(j = 0; j < no_of_chunks; j++)
    {
         write(fc, buffer, params -> record_size);
         //fsync(fc);
    }

   close(fc);
   free(buffer);
}


void createFiles(int no_of_files)
{
    int file_n = 1;
    int  j;
    long long int single_file_size = FILE_SIZE / no_of_files;
    char* buffer = malloc(single_file_size *  sizeof(char));
    memset(buffer, 'a', single_file_size);

    size_t ret;
    for(j = 0; j < no_of_files; j++)
    {
        FILE *fptr;
        char file_name[15];
        snprintf(file_name, 15, "file%d.txt", j);
        fptr = fopen(file_name, "w");
        ret = fwrite(buffer, 1, single_file_size, fptr);
        fclose(fptr);
    }

   free(buffer);

}

void * sequentialRead(void* arg)
{
    struct pthreadArgs *params = (struct pthreadArgs*) arg;

    static int file_n = 1;
    char* buffer = malloc(params -> record_size *  sizeof(char));
    long long int single_file_size = FILE_SIZE / params -> no_of_files;

    char file_name[15];
    snprintf(file_name, 15, "file%d.txt", file_n++);
    int fd = open(file_name, O_RDONLY | O_DIRECT | O_SYNC , S_IRWXU | S_IRGRP);

    long long int no_of_chunks = single_file_size / params -> record_size;
    size_t ret;
    long long int j;
    long curr_pos;

    for(j = 0; j < no_of_chunks - 1; j++)
    {
        memset(buffer, 0, params -> record_size);
        read(fd, buffer, params -> record_size);
        fdatasync(fd);
    }

    close(fd);
    free(buffer);

}

void * randomRead(void* arg)
{
    struct pthreadArgs *params = (struct pthreadArgs*) arg;
    static int file_n = 1;

    char* buffer = malloc(params -> record_size *  sizeof(char));
    int single_file_size = FILE_SIZE / params -> no_of_files;

    char file_name[15];
    snprintf(file_name, 15, "file%d.txt", file_n++);
    int fd = open(file_name, O_RDONLY | O_SYNC, S_IRWXU | S_IRGRP);
    int no_of_chunks = single_file_size / params -> record_size;
    size_t ret;
    int j;
    long curr_pos;
    int random_pos;

    for(j = 0; j < no_of_chunks - 1; j++)
    {
         random_pos = (rand()%(single_file_size - params -> record_size));
         lseek(fd, random_pos, SEEK_SET);
         memset(buffer, 0, params -> record_size);
         read(fd, buffer, params -> record_size);
         fsync(fd);
    }

    close(fd);
    free(buffer);

}


void randomWrite(void* arg)
{
    struct pthreadArgs *params = (struct pthreadArgs*) arg;
    static int file_n = 1;

    char* buffer = calloc(params -> record_size, sizeof(char));
    memset(buffer, 'a', params -> record_size);

    int i, j;
    long  int single_file_size = FILE_SIZE / params -> no_of_files;
    long int curr_file_size = 0;

    int fd;
    char file_name[15];
    snprintf(file_name, 15, "file%d.txt", file_n++);
    fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC | O_SYNC | O_APPEND, S_IRWXU | S_IRGRP);
    write(fd, buffer, params -> record_size);
    curr_file_size += params -> record_size;
    long  int curr_pos;
    long  int random_pos;

    srand(time(0));
    long int no_of_chunks = single_file_size / params -> record_size;

    for(j = 0; j < no_of_chunks - 1; j++)
    {
         random_pos = (rand() % curr_file_size);
         curr_file_size += params -> record_size;
        lseek(fd, random_pos, SEEK_SET);
        write(fd, buffer, params -> record_size);
        fsync(fd);
    }

    close(fd);
    free(buffer);

}




int main(int argc, char *argv[])
{
        int i = 0;
        int record_size = atoi(argv[1]);
        int no_of_threads = atoi(argv[2]);
        char *access_pattern = argv[3];

//      printf("values %d %d %s",  record_size, no_of_threads, access_pattern);

        struct timeval start_time, end_time;
        struct pthreadArgs arg = (struct pthreadArgs) arg;

        int j, k;


        if(strcmp(access_pattern, "WS") == 0)
        {

        file_no = 0;
        pthread_t threadIds[no_of_threads];
             //write sequential method
             printf("Write Sequential:\n");
             printf("Record Size: %d bytes\n", record_size);
             printf("No of threads: %d \n", no_of_threads);
             printf("File Size: %ld bytes\n", FILE_SIZE/no_of_threads);

             gettimeofday(&start_time, NULL);

             for(k = 0; k < no_of_threads; k++)
             {
                 arg.record_size = record_size;
                 arg.no_of_files = no_of_threads;
                 pthread_create(&threadIds[k], NULL, (void *) sequentialWrite, &arg);
             }

             int m;
             for(m = 0; m < no_of_threads; m++)
             {
                 pthread_join(threadIds[m], NULL);

             }

             gettimeofday(&end_time, NULL);

             double execution_time = (end_time.tv_sec + (end_time.tv_usec /1000000.0)) - (start_time.tv_sec     + (start_time.tv_usec/ 1000000.0));
            printf("Execution time for writing is %lf and throughput is %lf Mbps\n", execution_time, FILE_SIZE/(execution_time * 1048576));

    char *str = "rm file*";
    system(str);
        }
        else if(strcmp(access_pattern, "RS") == 0)
        {

        file_no = 0;
        pthread_t threadIds[no_of_threads];
             printf("Read Sequential:\n");
             printf("Record Size: %d bytes\n", record_size);
             printf("No of threads: %d\n", no_of_threads);
             printf("File Size: %ld bytes\n", FILE_SIZE/no_of_threads);

             createFiles(no_of_threads);
             gettimeofday(&start_time, NULL);

             for(k = 0; k < no_of_threads; k++)
             {
                 arg.record_size = record_size;
                 arg.no_of_files = no_of_threads;
                 pthread_create(&threadIds[k], NULL, (void *) sequentialRead, &arg);
            }

            int m;
             for(m = 0; m < no_of_threads; m++)
           {
                pthread_join(threadIds[m], NULL);

            }

            gettimeofday(&end_time, NULL);

            double execution_time = (end_time.tv_sec + (end_time.tv_usec /1000000.0)) - (start_time.tv_sec     + (start_time.tv_usec/ 1000000.0));
            printf("Execution time for reading is %lf secs and throughput is %lf\n\n\n", execution_time, FILE_SIZE / (execution_time * 1048576));

           char *str = "rm file*";
           system(str);
        }

        else if(strcmp(access_pattern, "RR") == 0)
        {
        file_no = 0;
        pthread_t threadIds[no_of_threads];

             printf("Read Random:\n");
             printf("Record Size: %d bytes\n", record_size);
             printf("No of threads: %d\n", no_of_threads);
             printf("File Size: %ld bytes\n", FILE_SIZE/no_of_threads);
           // read random method

             createFiles(no_of_threads);
             gettimeofday(&start_time, NULL);

             for(k = 0; k < no_of_threads; k++)
             {
                 arg.record_size = record_size;
                 arg.no_of_files = no_of_threads;
                 pthread_create(&threadIds[k], NULL, (void *) randomRead, &arg);
            }

             int m;
             for(m = 0; m < no_of_threads; m++)
           {
                pthread_join(threadIds[m], NULL);

            }

            gettimeofday(&end_time, NULL);

            double execution_time = (end_time.tv_sec + (end_time.tv_usec /1000000.0)) - (start_time.tv_sec     + (start_time.tv_usec/ 1000000.0));
            printf("Execution time for writing is %lf secs and throughput is %lf\n\n\n", execution_time, FILE_SIZE / (execution_time * 1048576));

            char *str = "rm file*";
            system(str);
        }

        else if(strcmp(access_pattern, "WR") == 0)
        {
        file_no = 0;
        pthread_t threadIds[no_of_threads];

             printf("Write Random:\n");
             printf("Record Size: %d bytes\n", record_size);
             printf("No of threads: %d \n", no_of_threads);
             printf("File Size: %ld bytes\n", FILE_SIZE/no_of_threads);

            // random write method

             gettimeofday(&start_time, NULL);

             for(k = 0; k < no_of_threads; k++)
             {
                 arg.record_size = record_size;
                 arg.no_of_files = no_of_threads;
                 pthread_create(&threadIds[k], NULL, (void *) randomWrite, &arg);
             }

             int m;
             for(m = 0; m < no_of_threads; m++)
             {
                  pthread_join(threadIds[m], NULL);

             }

             gettimeofday(&end_time, NULL);

             double execution_time = (end_time.tv_sec + (end_time.tv_usec /1000000.0)) - (start_time.tv_sec     + (start_time.tv_usec/ 1000000.0));
             printf("Execution time for writing is %lf secs and throughput is %lf Mbps\n", execution_time, FILE_SIZE/(execution_time * 1048576));
        char *str = "rm file*";
        system(str);
        }

//     printf("Hello World\nkidaan baai\n");
         return 0;
}