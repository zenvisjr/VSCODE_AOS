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
    int z = stat("Assignment1_2", &d);

    if(z == 0)
            {
                if(d.st_mode & S_IFDIR)
                {
                    const char *dir_path = "Assignment1_2/2_";
                    dir_file = (char *) malloc(strlen(argv[1]) + strlen(dir_path) + 1);
                    strcpy(dir_file, dir_path);
                    strcat(dir_file, argv[1]);
                    f2 = open(dir_file, O_TRUNC, 0644);
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
        //Creating a new directory named Assignment1_2
        d1 = mkdir("Assignment1_2", 0700);
        if(d1==0)
        {
            
            const char *dir_path = "Assignment1_2/2_";
            dir_file = (char *) malloc(strlen(argv[1]) + strlen(dir_path) + 1);
            strcpy(dir_file, dir_path);
            strcat(dir_file, argv[1]);

            //cout << dir_file;
       }
       else
       {
            perror(" @ directory't be created due to unexpected error");
            return 1;
       }
    }

        
    f2 = open(dir_file, O_RDWR | O_CREAT | O_APPEND,  0644);
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
        perror("File can't be opened due to unexpected error/n");
    }

    long long n, m, i=1, count1, count2, count3, per, rem_size, n1, m1, n2, m2, n3, m3, z1=0, ss, siz;
    n = atoi(argv[2]);
    m = atoi(argv[3]);
    
    //Finding size of A.txt using stat system call
    
    struct stat s, t;
    int a = stat(argv[1], &s);
    if(a == -1)
    {
        perror("Can't retrieve file information due to unexpected error/n");
    }
    off_t size_file = s.st_size;
    //cout<<"Size of file is "<<size_file<<endl;

    if(size_file == 0) return 0;

    //Finding size of buffer
    long long size_buffer = sizeof(buffer);
    //cout<<size_buffer<<endl;
    
        
       //reversing string till n

        count1 = n/size_buffer;
        lseek(f1,n, SEEK_SET);
        
        while(i<=count1)
        {
            lseek(f1, -(i*size_buffer), SEEK_CUR);
            
            n1 = read(f1, buffer, size_buffer);
            
            reverse(buffer, n1);

            write(f2, buffer, n1);
            //write(1, buffer, n);

            //calculating % of file reversed
            per = ((i*size_buffer)*100)/size_file;
            cout<<"\r"<< per<<"%";
            cout.flush();
            //sleep(1);
            i++;
        }

        // edge case
        
        rem_size = n - count1*size_buffer;
        //cout<<rem_size<<endl;
        lseek(f1, 0, SEEK_SET);
        m1 = read(f1, buffer, rem_size);
        reverse(buffer, m1);
        write(f2, buffer, m1);
        //write(1, buffer, m);

        
        z1 = count1*size_buffer + rem_size;
        per = (z1*100)/size_file;
        cout<<"\r"<< per<<"%";
        cout.flush();
        //sleep(1);
        

         //coping content without reversing

        siz = m-n+1;
        count2 = siz/size_buffer;
        int j=1;
        

        lseek(f1,n, SEEK_SET);
        while(j<=count2)
        {
            //lseek(f1, -(i*size_buffer), SEEK_END);
            
            n2 = read(f1, buffer, size_buffer);
            
            write(f2, buffer, n2);
            //write(1, buffer, n);

            //calculating % of file reversed
            z1 = z1+ j*size_buffer;
            per = ((z1)*100)/size_file;
            cout<<"\r"<< per<<"%";
            cout.flush();
            //sleep(1);
            j++;
        }

        // edge case
        
        rem_size = m-(count2*size_buffer)-n+1;
    
        lseek(f1, 0, SEEK_CUR);
        m2 = read(f1, buffer, rem_size);
        write(f2, buffer, m2);

        z1 = z1 + rem_size;
        per = (z1*100)/size_file;
        cout<<"\r"<< per<<"%";
        cout.flush();
        //sleep(1);

    //reversing content of file after m

        ss = size_file-m-1;
        count3 = (ss)/size_buffer;
    
        int k=1;

        while(k<=count3)
        {
            lseek(f1, -(k*size_buffer)-1, SEEK_END);
            
            n2 = read(f1, buffer, size_buffer);
            
            reverse(buffer, n2);

            write(f2, buffer, n2);
            //write(1, buffer, n);

            //calculating % of file reversed
            per = ((m+k*size_buffer)*100)/size_file;
            cout<<"\r"<< per<<"%";
            cout.flush();
            //sleep(1);
            k++;
        }


        // edge case
        
        rem_size = ss - count3*size_buffer;
        lseek(f1, m+1, SEEK_SET);
        m2 = read(f1, buffer, rem_size);
        reverse(buffer, m2);
        write(f2, buffer, m2);
        //write(1, buffer, m);

        per = ((size_file)*100)/size_file;
        cout<<"\r"<< per<<"%";
        cout.flush();
        //sleep(1);

        cout<<endl;
        
    
        
    /*int b = stat(dir_file, &t);
    if(b == -1)
    {
        perror("Can't retrieve file information due to unexpected error/n");
    }
    off_t size_file1 = t.st_size;
    cout<<endl<<"Size of modified file is "<<size_file1<<endl;*/
}