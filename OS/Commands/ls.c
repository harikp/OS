/*Listing the Files in a directory */

/**
Harikrishnan.P
AM.EN.P2CSN14013
M.Tech 2014 CSN
**/

#include <sys/types.h>
#include <sys/dir.h>
#include <sys/param.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <locale.h>
#include <langinfo.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>

static char perms_buff[30];
/**Function returns a string with permissions**/
const char *get_perms(mode_t mode)
{
  char ftype = '?';

  if (S_ISREG(mode)) ftype = '-';
  if (S_ISLNK(mode)) ftype = 'l';
  if (S_ISDIR(mode)) ftype = 'd';
  if (S_ISBLK(mode)) ftype = 'b';
  if (S_ISCHR(mode)) ftype = 'c';
  if (S_ISFIFO(mode)) ftype = '|';

  sprintf(perms_buff, "%c%c%c%c%c%c%c%c%c%c %c%c%c", ftype,
  mode & S_IRUSR ? 'r' : '-',
  mode & S_IWUSR ? 'w' : '-',
  mode & S_IXUSR ? 'x' : '-',
  mode & S_IRGRP ? 'r' : '-',
  mode & S_IWGRP ? 'w' : '-',
  mode & S_IXGRP ? 'x' : '-',
  mode & S_IROTH ? 'r' : '-',
  mode & S_IWOTH ? 'w' : '-',
  mode & S_IXOTH ? 'x' : '-',
  mode & S_ISUID ? 'U' : '-',
  mode & S_ISGID ? 'G' : '-',
  mode & S_ISVTX ? 'S' : '-');

  return (const char *)perms_buff;
}

char pathname[MAXPATHLEN];

/**Function to exit from the program with the msg as error mssg**/
void die(char *msg)
{
  perror(msg);
  exit(0);
}

static int
one (const struct dirent *unused)
{
  return 1;
}

int lsla(char *path)
{
  int count,i;
  struct direct **files;
  struct stat statbuf;
  char datestring[256];
  struct passwd pwent;
  struct passwd *pwentp;
  struct group grp;
  struct group *grpt;
  struct tm time;
  char buf[1024];

  /*if(!getcwd(pathname, sizeof(pathname)))
    die("Error getting pathnamen");
  */
  count = scandir(path, &files, one, alphasort);

  if(count > 0)
  { 
    int total=0;
    for(i=0;i<count;++i)
    {
    	char* temp = (char *) malloc(sizeof(path)+sizeof(files[i]->d_name));
    	strcpy(temp, path); //copy path to temp
    	stat(strcat(temp, files[i]->d_name), &statbuf); // we pass path to + name of file
    	total += statbuf.st_blocks;
    }

    printf("total %d\n",total);
    for (i=0; i<count; ++i)
    {
      if (stat(files[i]->d_name, &statbuf) == 0)
      {
        /* Print out type, permissions, and number of links. */
        printf("%10.10s", get_perms(statbuf.st_mode));
        printf(" %d", statbuf.st_nlink);

        if (!getpwuid_r(statbuf.st_uid, &pwent, buf, sizeof(buf), &pwentp))
          printf(" %s", pwent.pw_name);
        else
          printf(" %d", statbuf.st_uid);

        if (!getgrgid_r (statbuf.st_gid, &grp, buf, sizeof(buf), &grpt))
          printf(" %s", grp.gr_name);
        else
          printf(" %d", statbuf.st_gid);

        /* Print size of file. */
        printf(" %5d", (int)statbuf.st_size);

        localtime_r(&statbuf.st_mtime, &time);
        /* Get localized date string. */
        strftime(datestring, sizeof(datestring), "%b %d %H:%M", &time);

        printf(" %s %s\n", datestring, files[i]->d_name);
      }
	
      free (files[i]);
    }

    free(files);
  }
}
int ls(char *path)
{
	int count,i;
	struct direct **files;
	struct stat statbuf;
	count = scandir(path, &files, one, alphasort);
	char space[5];
	if(count>8)
        {
                sprintf(space,"%s","\n");
        }
        else
        {
                sprintf(space,"%s","  ");
        }

	if(count>0)
	{
		for(i=0;i<count;++i)
		{
			if (stat(files[i]->d_name, &statbuf) == 0)
      			{
				if(strncmp(files[i]->d_name,".",1)!=0)
				{
        				printf("%s%s", files[i]->d_name,space);
				}
      			}

      			free (files[i]);
		}
		printf("\n");
		free(files);
	}

}
int lsa(char *path)
{
        int count,i;
        struct direct **files;
        struct stat statbuf;
        count = scandir(path, &files, one, alphasort);
        char space[5];
        if(count>8)
        {
                sprintf(space,"%s","\n");
        }
        else
        {
                sprintf(space,"%s","  ");
        }
        if(count>0)
        {
                for(i=0;i<count;++i)
                {
                        if (stat(files[i]->d_name, &statbuf) == 0)
                        {
                        	printf("%s%s", files[i]->d_name,space);
                        }

                        free (files[i]);
                }
                printf("\n");
                free(files);
        }

}

int lsl(char *path)
{
  int count,i;
  struct direct **files;
  struct stat statbuf;
  char datestring[256];
  struct passwd pwent;
  struct passwd *pwentp;
  struct group grp;
  struct group *grpt;
  struct tm time;
  char buf[1024];

  /*if(!getcwd(pathname, sizeof(pathname)))
    die("Error getting pathnamen");
  */
  count = scandir(path, &files, one, alphasort);

  if(count > 0)
  { 
    int total=0;
    for(i=0;i<count;++i)
    {
    	char* temp = (char *) malloc(sizeof(path)+sizeof(files[i]->d_name));
    	strcpy(temp, path); //copy path to temp
    	stat(strcat(temp, files[i]->d_name), &statbuf); // we pass path to + name of file
    	total += statbuf.st_blocks;
    }

    printf("total %d\n",total);
    for (i=0; i<count; ++i)
    {
      if (stat(files[i]->d_name, &statbuf) == 0 && strncmp(files[i]->d_name,".",1)!=0)
      {
        /* Print out type, permissions, and number of links. */
        printf("%10.10s", get_perms(statbuf.st_mode));
        printf(" %d", statbuf.st_nlink);

        if (!getpwuid_r(statbuf.st_uid, &pwent, buf, sizeof(buf), &pwentp))
          printf(" %s", pwent.pw_name);
        else
          printf(" %d", statbuf.st_uid);

        if (!getgrgid_r (statbuf.st_gid, &grp, buf, sizeof(buf), &grpt))
          printf(" %s", grp.gr_name);
        else
          printf(" %d", statbuf.st_gid);

        /* Print size of file. */
        printf(" %5d", (int)statbuf.st_size);

        localtime_r(&statbuf.st_mtime, &time);
        /* Get localized date string. */
        strftime(datestring, sizeof(datestring), "%b %d %H:%M", &time);

        printf(" %s %s\n", datestring, files[i]->d_name);
      }
	
      free (files[i]);
    }

    free(files);
  }
}
int main(int argc,char **argv)
{
    if(argc<2)
    {
	fprintf(stderr,"Usage:<exec><ls_command>\n");
	exit(0);
    }
    if(strcmp(argv[1],"ls")!=0)
    {
	fprintf(stderr,"Command Not Recognised\n");
	exit(0);
    }
    if(argv[2]==NULL)
    {
     	 if(!getcwd(pathname, sizeof(pathname)))
         die("Error getting pathname\n");
	 ls(pathname);		 
         exit(0);
    }		
    if(strncmp(argv[2],"-",1)==0)
    {
	 if(!getcwd(pathname, sizeof(pathname)))
         die("Error getting pathname\n");
	 if(strcmp(argv[2],"-l")==0)
	 {
		lsl(pathname);
	 }
	 else if(strcmp(argv[2],"-la")==0)
	 {
		lsla(pathname);
	 }
	 else if(strcmp(argv[2],"-a")==0)
	 {
 		lsa(pathname);
         }

   }		
}
