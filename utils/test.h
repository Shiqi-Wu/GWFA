#define READ_FASTA(file,size,buff,num,start_point,mm,i, seq,pos,seq_pos) \
	FILE *file;	\
	size=0;	\
	file = fopen(argv[num],"rt");	\
	/*argv[1]为命令行传入的地址，"rt"表示以只读的方式打开一个文本文件*/	\
	if(file==NULL)	\
		{printf("ERROR:cannot retrieve this file.\n");	\
		return ERROR;}	\
	fseek(file,0L,SEEK_END);	\
	size=ftell(file);	\
	/*通过定位到文件末尾，读出文件大小size，或者也可通过下面注释掉的for循环读取文件大小size*/	\
	rewind(file);	\
	printf("The file size is %d\n",size);	\
	buff = (char *)malloc(size-1);	\
	start_point = (char *)malloc(size-1);	\
	if(buff==NULL||start_point==NULL)	\
		return ERROR;	\
	fread(buff,size-1,1,file);	\
	/*将file指向的文本文件内容读入buff缓冲区中*/	\
	start_point=buff;	\
	/*start_point用于存储buff指向的首地址，用于free等*/	\
	/*printf("%s\n",buff);*/	\
	/*打印出文本文件内容，此处用于调试，printf是个很好的调试方法，此处可检查文本是否读出，以及是否正确等*/	\
	static int i;	\
	static int pos;	\
	static int seq_pos;	\
	for(;*buff;buff++){	\
		i++;	\
		if((*buff=='|')&&(*(buff+1)==' ')){	\
			pos=i;	\
			printf("The value of pos is %d\n",pos);	\
		}	\
		if((*buff=='A'||*buff=='T'||*buff=='C'||*buff=='G')&&(*(buff+1)=='A'||*(buff+1)=='T'||*(buff+1)=='C'||*(buff+1)=='G')\
			&&(*(buff+2)=='A'||*(buff+2)=='T'||*(buff+2)=='C'||*(buff+2)=='G')){	\
		seq_pos=i-1;	\
		printf("The value of seq_pos is %d\n",seq_pos);	\
		break;	\
		}	\
	}	\
	/*for循环中记录了标识符的结束位置和核酸序列的起始位置，这里的标识符是指的第一个空格前面的字符*/	\
	char *mm=get_gi(start_point,pos);	\
	/*mm指向标识符的首地址，被调函数get_gi其实是用指针变量实现的字符串的拷贝，末了需要加上结束符'\0'*/	\
	char *seq=get_seq(start_point,seq_pos,size);	\
	/*seq指向核酸序列的首地址，被调函数get_seq也是指针变量实现的字符串的拷贝*/	\
	printf("SEQ=\n%s\n",seq);	\
	buff=start_point;	
	
#define FREE_FASTA(buff,file) \
	free(buff);\
	fclose(file);
