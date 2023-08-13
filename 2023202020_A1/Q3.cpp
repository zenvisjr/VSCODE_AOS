#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<iostream>
#include <cstring>
#include <dirent.h>

using namespace std;

int main(int args, char* argv[])
{

    char buffer1[5000], buffer2[5000];
    long long flag=0;
    char *dir_file;

    const char *dir_path = "Assignment1_1/";
    dir_file = (char *) malloc(strlen(argv[1]) + strlen(dir_path) + 1);
    strcpy(dir_file, dir_path);
    strcat(dir_file, argv[1]);
  

    int f1 = open(argv[2], O_RDONLY);

    if(f1==-1)
    {
        perror("File can't be opened due to unexpected error/n");
        return 0;
    }
    
    //Finding size of A.txt using stat system call
    
    struct stat s, t, d;
    int a = stat(argv[2], &s);
    if(a == -1)
    {
        perror("Can't retrieve file information due to unexpected error/n");
    }
    off_t size_file = s.st_size;
    //cout<<"Size of file is "<<size_file<<endl;


    int f2 = open(dir_file, O_RDONLY);


    if(f2==-1)
    {
        perror("File can't be opened due to unexpected error/n");
        return 0;
    }

    if(size_file == 0) 
    {
        perror("There is no content in file/n");
        return 0;
    }

    long long size_buffer = sizeof(buffer1);
    //cout<<size_buffer<<endl;

    //if size of text file is less than buffer
   /*if(size_file<size_buffer)
    {
        lseek(f1, -(size_file-1), SEEK_CUR);
        int l = read(f1, buffer, size_file);
        reverse(buffer, l);
        write(f2, buffer, l);
        cout<<100<<"%"<<endl;;
    }
    else
    {*/
        //Calculating no of times loop will run
        int count = size_file/size_buffer;
        //cout<<count<<endl;
        long long i=1, temp=0;

        //lseek(f1, 0, SEEK_SET);
        while(i<=count)
        {
            read(f1, buffer1, size_buffer);
            
            lseek(f2, -(i*size_buffer), SEEK_END);
            
            read(f2, buffer2, size_buffer);

            /*for(long long l=0;l<size_buffer;l++)
            {
                cout<<buffer1[l]<<" ";
            }
            cout<<endl;
            for(long long g=0;g<size_buffer;g++)
            {
                cout<<buffer2[g]<<" ";
            }
            cout<<endl;*/


            long long j=0, k=size_buffer-1;

            while(j<size_buffer)
            {
                if(buffer1[j] != buffer2[k])
                {
                    //cout<<" NO "<<endl;
                    //return 0;
                    flag=+1;
                    break;
                }
                else
                    j++;
                    k--;
            }
            if(flag!=0) break;
            i++;


            //calculating % of file checked
            long long per = ((i*size_buffer)*100)/size_file;
            cout<<"\r"<< per<<"%";
            cout.flush();
            //sleep(1);
            
        }
        //cout<<"YES"<<endl;
        
        // taking care of edge case when no of charaters to read < buffer size
        
        long long rem_size = size_file - (count)*size_buffer;
        lseek(f2, 0, SEEK_SET);
        read(f2, buffer2, rem_size);
        read(f1, buffer1, rem_size);
        //reverse(buffer, m);
        //write(f2, buffer, m);
        //write(1, buffer, m);

        /*for(int l=0;l<rem_size;l++)
        {
            cout<<buffer1[l]<<" ";
        }
        cout<<endl;
        for(int l=0;l<rem_size;l++)
        {
            cout<<buffer2[l]<<" ";
        }
            cout<<endl;*/

        long long j=0, k=rem_size-1;
        while(j<rem_size)
        {
            if(buffer1[j] != buffer2[k])
             {
                //cout<<" NO "<<endl;
                flag+=1;
                break;
            }
            else
                 j++;
                 k--;
        }

        long long per = ((size_file)*100)/size_file;
        cout<<"\r"<< per<<"%";
        cout.flush();
        //sleep(1);
        
   // }
   cout<<endl;


    int b = stat(dir_file, &t);
    if(b == -1)
    {
        perror("Can't retrieve file information due to unexpected error/n");
        return 0;
    }
    
    //string filePath = argv[3];
    if(opendir(argv[3]))
    {
        cout<<"Directory is created: Yes";
    }
    else 
        {
        perror("Directory can't be created due to unexpected error");
        return 0;
        }

    cout<<endl;
    if(flag!=0) 
    {
        cout<<"Whether file contents are reversed in newfile: No";
    }
    else
        {
            cout<<"Whether file contents are reversed in newfile: Yes";
        }
    cout<<endl;
    cout<<endl;
    cout<<((t.st_mode & S_IRUSR)? "User has read permissions on newfile: Yes": "User has read permissions on newfile: No");
    cout<<endl;
    cout<<((t.st_mode & S_IWUSR)? "User has write permissions on newfile: Yes": "User has write permissions on newfile: No");
    cout<<endl;
    cout<<((t.st_mode & S_IXUSR)? "User has execute permissions on newfile: Yes": "User has execute permissions on newfile: No");
    cout<<endl;
    cout<<((t.st_mode & S_IRGRP)? "Group has read permissions on newfile: Yes": "Group has read permissions on newfile: No");
    cout<<endl;
    cout<<((t.st_mode & S_IWGRP)? "Group has write permissions on newfile: Yes": "Group has write permissions on newfile: No");
    cout<<endl;
    cout<<((t.st_mode & S_IXGRP)? "Group has execute permissions on newfile: Yes": "Group has execute permissions on newfile: No");
    cout<<endl;
    cout<<((t.st_mode & S_IROTH)? "Other has read permissions on newfile: Yes": "Other has read permissions on newfile: No");
    cout<<endl;
    cout<<((t.st_mode & S_IWOTH)? "Other has write permissions on newfile: Yes": "Other has write permissions on newfile: No");
    cout<<endl;
    cout<<((t.st_mode & S_IXOTH)? "Other has execute permissions on newfile: Yes": "Other has execute permissions on newfile: No");
    
    cout<<endl;
    cout<<endl;
    cout<<((s.st_mode & S_IRUSR)? "User has read permissions on oldfile: Yes": "User has read permissions on newfile: No");
    cout<<endl;
    cout<<((s.st_mode & S_IWUSR)? "User has write permissions on oldfile: Yes": "User has write permissions on oldfile: No");
    cout<<endl;
    cout<<((s.st_mode & S_IXUSR)? "User has execute permissions on oldfile: Yes": "User has execute permissions on oldfile: No");
    cout<<endl;
    cout<<((s.st_mode & S_IRGRP)? "Group has read permissions on oldfile: Yes": "Group has read permissions on oldfile: No");
    cout<<endl;
    cout<<((s.st_mode & S_IWGRP)? "Group has write permissions on oldfile: Yes": "Group has write permissions on oldfile: No");
    cout<<endl;
    cout<<((s.st_mode & S_IXGRP)? "Group has execute permissions on oldfile: Yes": "Group has execute permissions on oldfile: No");
    cout<<endl;
    cout<<((s.st_mode & S_IROTH)? "Other has read permissions on oldfile: Yes": "Other has read permissions on oldfile: No");
    cout<<endl;
    cout<<((s.st_mode & S_IWOTH)? "Other has write permissions on oldfile: Yes": "Other has write permissions on oldfile: No");
    cout<<endl;
    cout<<((s.st_mode & S_IXOTH)? "Other has execute permissions on oldfile: Yes": "Other has execute permissions on oldfile: No");
    
    
    int z = stat(argv[3], &d);
    if(z == -1)
    {
        perror("Can't retrieve file information due to unexpected error/n");
    }
    cout<<endl;
    cout<<endl;
    cout<<((d.st_mode & S_IRUSR)? "User has read permissions on directory: Yes": "User has read permissions on directory: No");
    cout<<endl;
    cout<<((d.st_mode & S_IWUSR)? "User has write permissions on directory: Yes": "User has write permissions on directory: No");
    cout<<endl;
    cout<<((d.st_mode & S_IXUSR)? "User has execute permissions on directory: Yes": "User has execute permissions on directory: No");
    cout<<endl;
    cout<<((d.st_mode & S_IRGRP)? "Group has read permissions on directory: Yes": "Group has read permissions on directory: No");
    cout<<endl;
    cout<<((d.st_mode & S_IWGRP)? "Group has write permissions on directory: Yes": "Group has write permissions on directory: No");
    cout<<endl;
    cout<<((d.st_mode & S_IXGRP)? "Group has execute permissions on directory: Yes": "Group has execute permissions on directory: No");
    cout<<endl;
    cout<<((d.st_mode & S_IROTH)? "Other has read permissions on directory: Yes": "Other has read permissions on directory: No");
    cout<<endl;
    cout<<((d.st_mode & S_IWOTH)? "Other has write permissions on directory: Yes": "Other has write permissions on directory: No");
    cout<<endl;
    cout<<((d.st_mode & S_IXOTH)? "Other has execute permissions on oldfile: Yes": "Other has execute permissions on directory: No");
    cout<<endl;
}