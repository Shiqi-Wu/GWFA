#include "GWFA_fasta_read.h"

char *get_gi(char *p,int n){
	static char *m, *t;
	m=(char *)malloc(n);
	t=(char *)malloc(n);
	if(m==NULL)
		return NULL;
	t=m;
	while(n--){
		*m++=*p++;
	}
	*(m)='\0';
	return t;
}

char *get_seq(char *p, int n, int size){
	static char *m, *t;
	m=(char *)malloc(n-1);
	t=(char *)malloc(n-1);
	if(m==NULL)
		return NULL;
	t=m;
	p=p+n;
	while((n<size)&&(*p)){
		*m++=*p++;
		n++;
	}
	*(m-1)='\0';
	return t;
}

char* GWFA_fasta_read(char* argv, int size)
{
	FILE *file;
	size=0;
	file = fopen(argv,"rt");
	/* argv[1]为命令行传入的地址，“rt”表示只读 */
	if(file==NULL){
		printf("ERROR:cannot retrieve this file.\n");
		return NULL;
	}
	fseek(file,0L,SEEK_END);
	size=ftell(file);
	/* 通过定位到文件末尾，读出文件大小size */
	rewind(file);
	printf("The file size is %d\n",size);
	buff = (char *)malloc(size-1);
	start_point = (char *)malloc(size-1);
	if(buff==NULL||start_point==NULL)
		return NULL;
	fread(buff,size-1,1,file);
	/* 将file指向的文本文件内容读入buff缓冲区中 */
	start_point=buff;
	/*start_point用于存储buff指向的首地址，用于free*/
	printf("%s\n",buff);
	/* 打印文本文件内容 */
	static int i;
	
	static int pos;
	static int seq_pos;
	for(;*buff;buff++){
		//printf("%p\n",buff);
		i++;
		if((*buff=='|')&&(*(buff+1)==' ')){
			pos=i;
			//buff--;
			printf("The value of pos is %d\n",pos);
		}
		if((*buff=='A'||*buff=='T'||*buff=='C'||*buff=='G')&&(*(buff+1)=='A'||*(buff+1)=='T'||*(buff+1)=='C'||*(buff+1)=='G')&&(*(buff+2)=='A'||*(buff+2)=='T'||*(buff+2)=='C'||*(buff+2)=='G')){
			seq_pos=i-1;
			printf("The value of seq_pos is %d\n", seq_pos);
			break;
		}
	}
	/* for循环中记录了标识符的结束位置和核酸序列的起始位置，这里的标识符是指的第一个空格前面的字符*/
	char *mm=get_gi(start_point,pos);
	/* mm指向标识符的首位置，被调函数get_gi其实是用指针变量实现的字符串的拷贝，最后需要加上结束符'\0'*/
	char *seq=get_seq(start_point,seq_pos,size);
	/* seq指向核酸序列的首地址，被调函数get_seq也是指针变量实现的字符串的拷贝*/
	printf("DESRIPTOR=\n%s\n",mm);
	printf("SEQ=\n%s\n",seq);
	buff=start_point;
	
	free(buff);
	free(mm);
	//free(seq);
	fclose(file);
	return seq;
}
