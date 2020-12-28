#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *fp;
char *str;

void menu()
{
	puts("1: Read temporary string state");
	puts("2: Write new string");
	puts("3: Concatenate string on temporary string");
	puts("4: Delete whole string");
	puts("5: Delete spaces at the begin and end of string");
	puts("6: Remove string from temporary string");
	puts("7: Remove last n characters from string");
	puts("Q: quit");
}
void file_open_r()
{
	fp = fopen("/dev/stred", "r");
	if(fp == NULL)
	{
		puts("Cannot open /dev/stred");
	}

}
void file_open_w()
{
	fp = fopen("/dev/stred", "w");
	if(fp == NULL)
	{
		puts("Cannot open /dev/stred");
	}

}

void file_close()
{
	if(fclose(fp))
	{
		puts("Cannot close /dev/stred");
	}

}
int main()
{
	
	char  choose;
	char string[100];
	int n;
	while(1)
	{
		menu();
		scanf("%s", string);
		choose = string[0];
		//printf("%c", choose);
			switch(choose)
			{
				case '1': file_open_r();
					fscanf(fp, "%s",string);
					printf("%s\n",string);
					file_close();
					break;
				case '2': puts("Insert string");
					scanf("%s", string);
					file_open_w();
					fprintf(fp, "string=%s\n", string);
					file_close();
					break;
				case '3': puts("Insert string that should be concatenated to the temporary");
					scanf("%s", string);
					file_open_w();
					fprintf(fp, "append=%s\n", string);
					file_close();
					break;
				case '4': file_open_w();
					fprintf(fp, "clear\n");
					file_close();
					break;
				case '5': file_open_w();
					fprintf(fp, "shrink\n");
					file_close();
					break;
				case '6': puts("Insert string that should be removed");
					scanf("%s", string);
					file_open_w();
					fprintf(fp, "remove=%s\n", string);
					file_close();
					break;
				case '7': puts("How many characters would you like to remove");
					scanf("%s", string);
					n = atoi(string);
					file_open_w();
					fprintf(fp, "truncate=%d\n", n);
					file_close();
					break; 
				case 'Q': puts("Goodbay cruel world");
					  exit(0);
					  break;
				default: puts("Wrong input command. The input must be a number greater then 0 and smaller than 8 or letter Q");
			} 
		
	}
}
