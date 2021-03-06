/*Show Process Attributes */

/**
Harikrishnan.P
AM.EN.P2CSN14013
M.Tech 2014 CSN
**/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include <libgen.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
 
/* checks if the string is purely an integer
 * we can do it with `strtol' also
 */
int check_if_number (char *str)
{
  int i;
  for (i=0; str[i] != '\0'; i++)
  {
    if (!isdigit (str[i]))
    {
      return 0;
    }
  }
  return 1;
}
 
#define MAX_BUF 1024
#define INT_SIZE_BUF 6
#define PID_LIST_BLOCK 32
#define UP_TIME_SIZE 10
const char *getUserName(int uid)
{

  struct passwd *pw = getpwuid(uid);
  if (pw)
  {
    return pw->pw_name;
  }

  return "";
}
 
void pidaux ()
{
  DIR *dirp;
  FILE *fp;
  struct dirent *entry;
  char path[MAX_BUF], read_buf[MAX_BUF],temp_buf[MAX_BUF];
  char uid_int_str[INT_SIZE_BUF]={0},*line;
  char uptime_str[UP_TIME_SIZE];
  char *user,*command;
  size_t len=0;
  dirp = opendir ("/proc/");
  if (dirp == NULL)
  {
    perror ("Fail");
    exit(0);
  } 
  strcpy(path,"/proc/");
  strcat(path,"uptime");
 
  fp=fopen(path,"r");
  if(fp!=NULL)
  {
	getline(&line,&len,fp);
	sscanf(line,"%s ",uptime_str);
  }
  long uptime=atof(uptime_str);
  long Hertz=sysconf(_SC_CLK_TCK); 
  strcpy(path,"/proc/");
  strcat(path,"meminfo");

  fp=fopen(path,"r");
  unsigned long long total_memory;
  if(fp!=NULL)
  {
	getline(&line,&len,fp);
	sscanf(line,"MemTotal:        %llu kB",&total_memory);
  }	
  fprintf(stdout,"USER\t      PID CPU %MEM     VSZ    RSS   STAT START  COMMAND\n");
  while ((entry = readdir (dirp)) != NULL)
  {
    if (check_if_number (entry->d_name))
    {
	char fmt_time[20];
	strcpy(path,"/proc/");
	strcat(path,entry->d_name);
	struct stat info;
	stat(path,&info);
	struct tm * timeinfo = localtime(&info.st_ctime); // or gmtime() depending on what you want
	char t_info[20];
	strftime(t_info,20,"%H:%M",timeinfo);

	strcat(path,"/status");
	unsigned long long memory_rss;
	fp=fopen(path,"r");
	unsigned long long vmsize;
	//fprintf(stdout,"%s ",entry->d_name);
	if(fp!=NULL)
	{
		vmsize=0;
		getline(&line,&len,fp);
		getline(&line,&len,fp);
		getline(&line,&len,fp);
		getline(&line,&len,fp);
		getline(&line,&len,fp);
		getline(&line,&len,fp);
		getline(&line,&len,fp);
		getline(&line,&len,fp);
		sscanf(line,"Uid:    %s ",uid_int_str);
		getline(&line,&len,fp);
                getline(&line,&len,fp);
                getline(&line,&len,fp);
		getline(&line,&len,fp);
                getline(&line,&len,fp);
		sscanf(line,"VmSize:    %llu kB",&vmsize);
                getline(&line,&len,fp);
                getline(&line,&len,fp);
                getline(&line,&len,fp);
		getline(&line,&len,fp);
		sscanf(line,"VmRSS:     %llu kB",&memory_rss);
		//fprintf(stdout,"VmRSS::%llu",memory_rss);
	}
	else
	{
		fprintf(stdout,"FP is NULL\n");
	}
	float memory_usage=100*memory_rss/total_memory;
	strcpy(path,"/proc/");
	strcat(path,entry->d_name);
	strcat(path,"/stat");
	//getline(&line,&len,fp);
	fp=fopen(path,"r");
	getline(&line,&len,fp);
	char comm[10],state;
	unsigned int flags;
	int pid,ppid,pgrp,session,tty_nr,tpgid;
	unsigned long minflt,cminflt,majflt,cmajflt,utime,stime;
	unsigned long long starttime;
	long cutime,cstime,priority,nice,num_threads,itreavalue;
	sscanf(line,"%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu %ld %ld %ld %ld %ld  %ld %llu",&pid,comm,&state,&ppid,&pgrp,&session,&tty_nr,&tpgid,&flags,&minflt,&cminflt,&majflt,&cmajflt,&utime,&stime,&cutime,&cstime,&priority,&nice,&num_threads,&itreavalue,&starttime);
	unsigned long total_time=utime+stime;
	total_time=total_time+(unsigned long)cutime+(unsigned long)cstime;
	float seconds=uptime-(starttime/Hertz);
	float cpu_usage=100*((total_time/Hertz)/seconds);
	if(isnan(cpu_usage))
	{
		cpu_usage=0.0;
	}
	if(isnan(memory_usage))
	{
		memory_usage=0.0;
	}
	strcpy (path, "/proc/");
	strcat (path, entry->d_name);
	strcat (path, "/comm");
	 
      	/* A file may not exist, it may have been removed.
       	* dut to termination of the process. Actually we need to
       	* make sure the error is actually file does not exist to
       	* be accurate.
       	*/
      	fp = fopen (path, "r");
      	if (fp != NULL)
      	{
        	fscanf (fp, "%s", read_buf);
		fclose(fp);
      	}
      	char *userName=getUserName(atoi(uid_int_str));
     	if(strlen(userName)<9)
      	{
		user=userName;	
      	}
      	else
      	{
		user=uid_int_str;

      	}
      	fprintf(stdout,"%-8s   %5d  %2.1f  %2.1f %7llu %7llu   %c \t %s  %-s\n",user,atoi(entry->d_name),cpu_usage,memory_usage,vmsize,memory_rss,state,t_info,read_buf);
     	 
    }
  } 
  closedir (dirp);
}

/*void pidelf ()
{
  DIR *dirp;
  FILE *fp;
  struct dirent *entry;
  char path[MAX_BUF], read_buf[MAX_BUF],temp_buf[MAX_BUF];
  char uid_int_str[INT_SIZE_BUF]={0},*line;
  char uptime_str[UP_TIME_SIZE];
  char *user,*command;
  size_t len=0;
  dirp = opendir ("/proc/");
  if (dirp == NULL)
  {
    perror ("Fail");
    exit(0);
  } 
  strcpy(path,"/proc/");
  strcat(path,"uptime");
 
  fp=fopen(path,"r");
  if(fp!=NULL)
  {
	getline(&line,&len,fp);
	sscanf(line,"%s ",uptime_str);
  }
  long uptime=atof(uptime_str);
  long Hertz=sysconf(_SC_CLK_TCK); 
  strcpy(path,"/proc/");
  strcat(path,"meminfo");

  fp=fopen(path,"r");
  unsigned long long total_memory;
  if(fp!=NULL)
  {
	getline(&line,&len,fp);
	sscanf(line,"MemTotal:        %llu kB",&total_memory);
  }	
  fprintf(stdout,"S UID\t       PID    PPID     C   PRI 	  NI     SZ  WCHAN  STIME\n");
  while ((entry = readdir (dirp)) != NULL)
  {
    if (check_if_number (entry->d_name))
    {
	char fmt_time[20];
	strcpy(path,"/proc/");
	strcat(path,entry->d_name);
	struct stat info;
	stat(path,&info);
	struct tm * timeinfo = localtime(&info.st_ctime); // or gmtime() depending on what you want
	char t_info[20];
	strftime(t_info,20,"%H:%M",timeinfo);

	strcat(path,"/status");
	unsigned long long memory_rss;
	fp=fopen(path,"r");
	unsigned long long vmsize;
	//fprintf(stdout,"%s ",entry->d_name);
	if(fp!=NULL)
	{
		vmsize=0;
		getline(&line,&len,fp);
		getline(&line,&len,fp);
		getline(&line,&len,fp);
		getline(&line,&len,fp);
		getline(&line,&len,fp);
		getline(&line,&len,fp);
		getline(&line,&len,fp);
		getline(&line,&len,fp);
		sscanf(line,"Uid:    %s ",uid_int_str);
		getline(&line,&len,fp);
                getline(&line,&len,fp);
                getline(&line,&len,fp);
		getline(&line,&len,fp);
                getline(&line,&len,fp);
		sscanf(line,"VmSize:    %llu kB",&vmsize);
                getline(&line,&len,fp);
                getline(&line,&len,fp);
                getline(&line,&len,fp);
		getline(&line,&len,fp);
		sscanf(line,"VmRSS:     %llu kB",&memory_rss);
		//fprintf(stdout,"VmRSS::%llu",memory_rss);
	}
	else
	{
		fprintf(stdout,"FP is NULL\n");
	}
	float memory_usage=100*memory_rss/total_memory;
	strcpy(path,"/proc/");
	strcat(path,entry->d_name);
	strcat(path,"/stat");
	//getline(&line,&len,fp);
	fp=fopen(path,"r");
	getline(&line,&len,fp);
	char comm[10],state;
	unsigned int flags;
	int pid,ppid,pgrp,session,tty_nr,tpgid;
	unsigned long minflt,cminflt,majflt,cmajflt,utime,stime;
	unsigned long long starttime;
	long cutime,cstime,priority,nice,num_threads,itreavalue;
	sscanf(line,"%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu %ld %ld %ld %ld %ld  %ld %llu",&pid,comm,&state,&ppid,&pgrp,&session,&tty_nr,&tpgid,&flags,&minflt,&cminflt,&majflt,&cmajflt,&utime,&stime,&cutime,&cstime,&priority,&nice,&num_threads,&itreavalue,&starttime);
	unsigned long total_time=utime+stime;
	total_time=total_time+(unsigned long)cutime+(unsigned long)cstime;
	float seconds=uptime-(starttime/Hertz);
	int cpu_usage=(int)100*((total_time/Hertz)/seconds);
	if(cpu_usage >1 || cpu_usage<0)
	{
		cpu_usage=0;
	}
	strcpy (path, "/proc/");
	strcat (path, entry->d_name);
	strcat (path, "/statm");
	fp=fopen(path,"r");
	getline(&line,&len,fp);

	unsigned long size,data,text;
	int resident,share,lib,dt; 	

	if(line!=NULL)
	{
		sscanf(line,"%ld %d %d %ld %d %ld %d",&size,&resident,&share,&text,&lib,&data,&dt);
	}
	else{
		size=0;
		resident=0;
		share=0;	
		text=0;
		lib=0;
		data=0;
		dt=0;
	}

	strcpy (path, "/proc/");
	strcat (path, entry->d_name);
	strcat (path, "/wchan");
	fp=fopen(path,"r");
	getline(&line,&len,fp);

	char wchan_str[8]={0};

	sscanf(line,"%6s ",wchan_str);

	strcpy (path, "/proc/");
	strcat (path, entry->d_name);
	strcat (path, "/comm");

      	fp = fopen (path, "r");
      	if (fp != NULL)
      	{
        	fscanf (fp, "%s", read_buf);
		fclose(fp);
      	}
      	char *userName=getUserName(atoi(uid_int_str));
     	if(strlen(userName)<9)
      	{
		user=userName;	
      	}
      	else
      	{
		user=uid_int_str;

      	}
      	fprintf(stdout,"%c %-8s   %5d  %5d    %3d  %4ld  %4ld  %6ld  %s  \t %s  %-s\n",state,user,atoi(entry->d_name),ppid,cpu_usage,priority+60,nice,size,wchan_str,t_info,read_buf);
     	 
    }
  } 
  closedir (dirp);
}*/
void pidelf ()
{
  DIR *dirp;
  FILE *fp;
  struct dirent *entry;
  char path[MAX_BUF], read_buf[MAX_BUF],temp_buf[MAX_BUF];
  char uid_int_str[INT_SIZE_BUF]={0},*line;
  char uptime_str[UP_TIME_SIZE];
  char *user,*command;
  size_t len=0;
  dirp = opendir ("/proc/");
  if (dirp == NULL)
  {
    perror ("Fail");
    exit(0);
  } 
  strcpy(path,"/proc/");
  strcat(path,"uptime");
 
  fp=fopen(path,"r");
  if(fp!=NULL)
  {
	getline(&line,&len,fp);
	sscanf(line,"%s ",uptime_str);
  }
  long uptime=atof(uptime_str);
  long Hertz=sysconf(_SC_CLK_TCK); 
  strcpy(path,"/proc/");
  strcat(path,"meminfo");

  fp=fopen(path,"r");
  unsigned long long total_memory;
  if(fp!=NULL)
  {
	getline(&line,&len,fp);
	sscanf(line,"MemTotal:        %llu kB",&total_memory);
  }	
  fprintf(stdout,"S UID\t       PID    PPID     C   PRI 	  NI     SZ     WCHAN    STIME   CMD\n");
  while ((entry = readdir (dirp)) != NULL)
  {
    if (check_if_number (entry->d_name))
    {
	char fmt_time[20];
	strcpy(path,"/proc/");
	strcat(path,entry->d_name);
	struct stat info;
	stat(path,&info);
	struct tm * timeinfo = localtime(&info.st_ctime); // or gmtime() depending on what you want
	char t_info[20];
	strftime(t_info,20,"%H:%M",timeinfo);

	strcat(path,"/status");
	unsigned long long memory_rss;
	fp=fopen(path,"r");
	unsigned long long vmsize;
	//fprintf(stdout,"%s ",entry->d_name);
	if(fp!=NULL)
	{
		vmsize=0;
		getline(&line,&len,fp);
		getline(&line,&len,fp);
		getline(&line,&len,fp);
		getline(&line,&len,fp);
		getline(&line,&len,fp);
		getline(&line,&len,fp);
		getline(&line,&len,fp);
		getline(&line,&len,fp);
		sscanf(line,"Uid:    %s ",uid_int_str);
		getline(&line,&len,fp);
                getline(&line,&len,fp);
                getline(&line,&len,fp);
		getline(&line,&len,fp);
                getline(&line,&len,fp);
		sscanf(line,"VmSize:    %llu kB",&vmsize);
                getline(&line,&len,fp);
                getline(&line,&len,fp);
                getline(&line,&len,fp);
		getline(&line,&len,fp);
		sscanf(line,"VmRSS:     %llu kB",&memory_rss);
		//fprintf(stdout,"VmRSS::%llu",memory_rss);
	}
	else
	{
		fprintf(stdout,"FP is NULL\n");
	}
	float memory_usage=100*memory_rss/total_memory;
	strcpy(path,"/proc/");
	strcat(path,entry->d_name);
	strcat(path,"/stat");
	//getline(&line,&len,fp);
	fp=fopen(path,"r");
	getline(&line,&len,fp);
	char comm[10],state;
	unsigned int flags;
	int pid,ppid,pgrp,session,tty_nr,tpgid;
	unsigned long minflt,cminflt,majflt,cmajflt,utime,stime;
	unsigned long long starttime;
	long cutime,cstime,priority,nice,num_threads,itreavalue;
	sscanf(line,"%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu %ld %ld %ld %ld %ld  %ld %llu",&pid,comm,&state,&ppid,&pgrp,&session,&tty_nr,&tpgid,&flags,&minflt,&cminflt,&majflt,&cmajflt,&utime,&stime,&cutime,&cstime,&priority,&nice,&num_threads,&itreavalue,&starttime);
	unsigned long total_time=utime+stime;
	total_time=total_time+(unsigned long)cutime+(unsigned long)cstime;
	float seconds=uptime-(starttime/Hertz);
	int cpu_usage=(int)100*((total_time/Hertz)/seconds);
	if(cpu_usage>1 || cpu_usage<0)
	{
		cpu_usage=0;
	}
	if(isnan(memory_usage))
	{
		memory_usage=0.0;
	}
	
	strcpy (path, "/proc/");
	strcat (path, entry->d_name);
	strcat (path, "/statm");
	fp=fopen(path,"r");
	getline(&line,&len,fp);

	unsigned long size,data,text;
	int resident,share,lib,dt; 	

	if(line!=NULL)
	{
		sscanf(line,"%ld %d %d %ld %d %ld %d",&size,&resident,&share,&text,&lib,&data,&dt);
	}
	else{
		size=0;
		resident=0;
		share=0;	
		text=0;
		lib=0;
		data=0;
		dt=0;
	}

	strcpy (path, "/proc/");
	strcat (path, entry->d_name);
	strcat (path, "/wchan");
	fp=fopen(path,"r");
	getline(&line,&len,fp);

	char wchan_str[8]={0};

	sscanf(line,"%6s ",wchan_str);

	strcpy (path, "/proc/");
	strcat (path, entry->d_name);
	strcat (path, "/comm");
	 
      	/* A file may not exist, it may have been removed.
       	* dut to termination of the process. Actually we need to
       	* make sure the error is actually file does not exist to
       	* be accurate.
       	*/
      	fp = fopen (path, "r");
      	if (fp != NULL)
      	{
        	fscanf (fp, "%s", read_buf);
		fclose(fp);
      	}
      	char *userName=getUserName(atoi(uid_int_str));
     	if(strlen(userName)<9)
      	{
		user=userName;	
      	}
      	else
      	{
		user=uid_int_str;

      	}
      	fprintf(stdout,"%c %-8s   %5d  %5d    %3d  %4ld  %4ld  %6ld   %s  \t %s  %-s\n",state,user,atoi(entry->d_name),ppid,cpu_usage,priority+60,nice,size,wchan_str,t_info,read_buf);
     	 
    }
  } 
  closedir (dirp);
}
 
int main (int argc, char *argv[])
{
	if(argc<2)
    {
		fprintf(stderr,"Usage:<exec><ps_command>\n");
		exit(0);
    }
    if(strcmp(argv[1],"ps")!=0)
    {
		fprintf(stderr,"Command Not Recognised\n");
		exit(0);
    }
    if(argv[2]==NULL)
    {
        fprintf(stderr,"Error getting command\n");
         exit(0);
    }		

	if(strcmp(argv[2],"-elf")==0)
	{
		pidelf();
	}
	else if(strcmp(argv[2],"aux")==0)		
	{
		pidaux();
	}
	else
	{
		fprintf(stderr,"Command not Found!!\n");
		exit(0);
	}
	return 0;
}
