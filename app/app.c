#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *fp;

void menu()
{
	putst("1: Read temporary string state");
	putst("2: Write new string");
	putst("3: Concatenate string on temporary string");
	putst("4: Delete whole string");
	putst("5: Delete spaces at the begin and end of string");
	putst("6: Remove string from temporary string");
	putst("7: Remove last n characters from string");
}
void file_open()
{
	fp = fopen("/dev/stred", "r");
	if(fp == NULL)
	{
		putst("Problem pri otvaranju /dev/stred");
	}

}
void file_close()
{
	if(fclose(fp))
	{
		putst("Problem pri zatvaranju");
	}

}
int main()
{
	
	char choose;
	char string[100];
	int n;
	while(1)
	{
		menu();
		scanf("%c", choose);
		if((choose<1) || choose>7)
			putst("Izaberite broj od 1 do 7);
		else
		{
			switch(choose)
			{
				case 1: file_open();
					fscanf(fp, "%s",string);
					putst(string);
					file_close();
					break;
				case 2: putst("Insert string");
					scanf("%s", string);
					file_open();
					fprintf(fp, "string=%s", string);
					file_colse();
					break;
				case 3: putst("Insert string that should be concatenated to the temporary");
					scanf("%s", string);
					file_open();
					fprintf(fp, "append=%s", string);
					file_close();
					break;
				case 4: file_open();
					fprintf(fp, "clear");
					file_close();
					break;
				case 5: file_open();
					fprintf(fp, "shrink");
					file_close();
					break;
				case 6: putst("Insert string that should be removed");
					scanf("%s", string);
					file_open();
					fprintf(fp, "remove=%s), string);
					file_close();
					break;
				case 7: putst("How many characters would you like to remove");
					scanf("%d", n);
					file_open();
					fprintf(fp, "truncate=%d", n);
					file_close();
					break; 
			} 
		}
	}
}
