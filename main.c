#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <vulkan/vulkan.h>

char usage[]=
	"Usage: vulkancodegen FILENAME.c\n"
	"This program is generating the Vulkan API program in C code, asking questions what the result program will do, during the generation.\n";

	/*
char* itoa(int n)
{
	int i, sign;
	if((sign=n)<0)
		n=-n;
	i=5;
	do {
		s[i--] = n % 10 + '0';
	} while((n /= 10) > 0);
	if(sign<0)
		s[i--] = '-';
}
*/
#include <itoa.c>


char *code;
unsigned int codesize=0;
unsigned int n=0;

char headerparser(char* path)
{
	char hfd;
	if((hfd=open(path,0,0))==-1){
		return -1;
	}
	int wstatus;
	struct stat hst;
	fstat(hfd,&hst);
	long int hbsize;
	char *hbuff=malloc(hst.st_size);
	read(hfd,hbuff,hbsize);
	close(hfd);

	char comment=0;
	char string=0;
	for(int c=0;;c++){
		switch(hbuff[c]){
		case '/':
			switch(hbuff[c+1]){
			case '/':
				while(hbuff[c]!='\n')
					c++;
				break;
			case '*':
				while(!(hbuff[c]=='*'&&hbuff[c+1]=='/'))
					c++;
				break;
			}
		}
	}
}

void ac(char *buff)
{
	unsigned int size=strlen(buff)+1;
	printf("\n%lu\n",size);
	code=realloc(code,codesize+size);
	n=0;
	for(;n<size-1;){
		code[codesize]=buff[n];
		n++;
		codesize++;
	}
	write(1,code+codesize-n,n);
}

//#define ac(buff) appendtocode(buff, sizeof(buff))
//doesn't work with pointers

//constants of future generated code
int ielc;
int ieec;


char *filename;
char s[10];
unsigned int i=0;
char fd;
char yes=1;
char q=0;
char windowyes=1;
char num=0;
char evdev=0;
char keyboardpath[1024]="/dev/input/event3";
unsigned char keyoffset=20;
unsigned char keystateoffset;
unsigned char kbsize=72;
unsigned char *kb;
char kfd;
char ck=0;
void in(){
	if(evdev==0){
		fgets(s,10,stdin);
		for(i=0; i<10;i++){
			switch(s[i]){
				case 's':
					write(fd,code,codesize);
					break;
				case 'y':
				case 'Y':
					yes=1;
					break;
				case 'n':
				case 'N':
					yes=0;
					break;
				case '?':
					q=1;
					break;
				case '0':
					  num=0;
				case '1':
					  num=1;
				case '2':
					  num=2;
				case '3':
					  num=3;
				case '4':
					  num=4;
				case '5':
					  num=5;
				case '6':
					  num=6;
				case '7':
					  num=7;
				case '8':
					  num=8;
				case '9':
					  num=9;
			}
		}
	} else {
		read(kfd,&kb,kbsize);//some systems have no tty
	}
}
	/*TODO will compare scancode with each keycode*/
#define IN in()

//TODO add other than Linux OS support
#define pr(message) write(1,message,sizeof(message))

int main(int argc, char** argv)
{
	if(argv[1]!=NULL){
	for(unsigned char arg=0; arg<argc;arg++){
	switch(argv[arg][0]){
	case '-':
		switch(argv[arg][1]){
			case 'h':
				pr(usage);
				return 0;
			case '-':
				switch(argv[arg][2]){
					case 'h':
						pr(usage);
						return 0;
				}
		}
	default:
		filename=argv[arg];
	}
	}
	}
	size_t len=0;
	if(filename==NULL){
		pr("Enter the filename\n");
		getline(&filename,&len,stdin);
		filename[sizeof(&filename)]=0;
	}
	
	fd=open(filename,O_RDWR|O_CREAT|O_NONBLOCK|O_TRUNC,06666);
	char headers[]="#include <vulkan/vulkan.h>\n"
		"\n";
	ac(headers);
	IN;
	pr("create window? Y/n\n");
	yes=1;
	IN;
	if(yes==0){
		windowyes=0;
		goto initialize;
	}
initialize:
	//read(0,NULL,1);
	//write(1,code+codesize-n,n);
	pr("enable extra layers? y/N\n");
	yes=0;
	IN;
	if(yes==0){
		goto instance_enabled_layer_names;
	}
instance_enabled_layer_names:
	ielc=0;
	ac(
	"int ilayers[]={\n"
	"}\n"
	);
instance_enabled_extensions:
	ieec=0;
	ac(
	"int iextensions[]={\n"
	);
	IN;
	pr("enable extensions? y/N\n");
	yes=0;
	IN;
	if(yes==0){
		ac("}\n");
		goto instance;
	}
	yes=0;
	pr("VK_EXT_DEBUG_UTILS_EXTENSION_NAME? y/N\n");
	IN;
	if(yes==1){
		ac("VK_EXT_DEBUG_UTILS_EXTENSION_NAME,\n");
		ieec++;
	}
	ac("}\n");
instance:
	ac("int ieec=");
	ac(itoa(ieec));
	//printf("\n%lu\n",strlen(itoa(ieec)));
	ac(";\n");
	IN;
	ac(
	"VkInstance vkinstance;\n"
	"VkInstanceCreateInfo instanceCreateInfo = {\n"
	"	.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,\n"
	"	.pNext = NULL,\n"
	"	.enabledLayerCount = ielc,\n"
	"	.ppEnabledLayerNames = &ilayers,\n"
	"	.enabledExtensionCount = ieec,\n"
	"	.ppEnabledExtensionNames = iextensions \n"
	"};\n"
	"if((err=vkCreateInstance(&instanceCreateInfo, NULL, &vkinstance))) {\n"
	"	printf(\"vkCreateInstance() failed\");\n"
	"	return err;\n"
	"}\n");
	IN;
	write(fd,code,codesize);
	ac("VkPhysicalDevice vkphysicaldevice"
	  );

}
