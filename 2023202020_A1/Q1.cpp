#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<iostream>
#include <cstring>
using namespace std;


void reverse(char buffer[], int n)
{
    int i=0, j=n-1;
    while(i<n/2)
    {
        swap(buffer[i++], buffer[j--]);
    }
    
}
int main(int args, char* argv[])
{
    int d1, f1, f2;
    char *dir_file;

    //seeing if the directory exist or not

    struct stat d;
    int z = stat("Assignment1_1", &d);

    if(z == 0)
            {
                if(d.st_mode & S_IFDIR)
                {
                    const char *dir_path = "Assignment1_1/1_";
                    dir_file = (char *) malloc(strlen(argv[1]) + strlen(dir_path) + 1);
                    strcpy(dir_file, dir_path);
                    strcat(dir_file, argv[1]);
                    f2 = open(dir_file, O_TRUNC, 0600);
                    {
                        
                        if(f2==-1)
                        {
                            perror("# File can't be opened due to unexpected error");
                            return 1;
                        }
                    } 
                }
                else
                    {
                        perror("$ Path exist but Directory do not exist");
                    }
            } 
    else
    {
        //Creating a new directory named Assignment1_1
        d1 = mkdir("Assignment1_1", 0700);
        if(d1==0)
        {
            
            const char *dir_path = "Assignment1_1/1_";
            dir_file = (char *) malloc(strlen(argv[1]) + strlen(dir_path) + 1);
            strcpy(dir_file, dir_path);
            strcat(dir_file, argv[1]);

       }
       else
       {
            perror(" @ directory't be created due to unexpected error");
            return 1;
       }
    }

        
    f2 = open(dir_file, O_RDWR | O_CREAT | O_APPEND,  0600);
    {
        //cout<<d1<<f2;
        if(f2==-1)
        {
            perror("~File can't be created due to unexpected error");
            return 1;
        }
    }

    //creating buffer and opening input file
    
    char buffer[5000];

    f1 = open(argv[1], O_RDONLY);
    
    if(f1==-1)
    {
        perror("^ File can't be opened due to unexpected error/n");
        return 1;
    }
    
    //Finding size of A.txt using stat system call
    
    struct stat s, t;
    int a = stat(argv[1], &s);
    if(a == -1)
    {
        perror("& Can't retrieve file information due to unexpected error/n");
    }
    off_t size_file = s.st_size;
    //cout<<"Size of file is "<<size_file<<endl;

    if(size_file == 0) return 0;


    //Finding size of buffer

    long long size_buffer = sizeof(buffer);
    //cout<<size_buffer<<endl;
    
    //if size of text file is less than buffer
    if(size_file<size_buffer)
    {
        lseek(f1, -(size_file-1), SEEK_CUR);
        int l = read(f1, buffer, size_file);
        reverse(buffer, l);
        write(f2, buffer, l);
        cout<<100<<"%"<<endl;;
    }
    else
    {
        //Calculating no of times loop will run
        int count = size_file/size_buffer;
        //cout<<count<<endl;
        int i=1;

        while(i<=count)
        {
            lseek(f1, -(i*size_buffer), SEEK_END);
            int n = read(f1, buffer, size_buffer);
            reverse(buffer, n);
            write(f2, buffer, n);


            //calculating % of file reversed
            long long per = ((i*size_buffer)*100)/size_file;
            cout<<"\r"<< per<<"%";
            cout.flush();
            //sleep(.5);
            i++;
        }

        // taking care of edge case when no of charaters to read < buffer size
        int rem_size = size_file - (count)*size_buffer;
        lseek(f1, 0, SEEK_SET);
        int m = read(f1, buffer, rem_size);
        reverse(buffer, m);
        write(f2, buffer, m);

        //calculating % of file reversed
        long long per = ((size_file)*100)/size_file;
        cout<<"\r"<< per<<"%";
        cout.flush();
        //sleep(.5);
    }
    cout<<endl;


    /*int b = stat(dir_file, &t);
    if(b == -1)
    {
        perror(" * Can't retrieve file information due to unexpected error/n");
    }
    off_t size_file1 = t.st_size;
    cout<<"Size of modified file is "<<size_file1<<endl;*/

}
