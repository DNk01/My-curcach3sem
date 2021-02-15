#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <locale.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#define N 260
#define exit 300
#define aftercyclenextcycle 400
#define afterdonextcycle 1400
#define aftercyclenextcycleonthisline 500
#define aftercyclebrace 600
#define afterdobrace 1600
#define afterconditionbrace 2600
#define aftercyclesemicolon 800
#define afterdosemicolon 1800
#define afterconditioncycle 2400

char while1[] = "while ";
char while2[] = "while(";
char for1[] = "for ";
char for2[] = "for(";
char if1[] = "if ";
char if2[] = "if(";
char do1[] = "do ";
char do2[] = "do{";
char do3[] = "do\n";
char else1[] = "else ";
char else2[] = "else(";
char else3[] = "else\n";

/***************************************************************/
/*          П Р О Т О Т И П      Ф У Н К Ц И И                 */
/***************************************************************/
void checkkeywords(char* line, char** equalswhile, char** equalsfor, char** equalsif, char** equalselse, char** equalsdo);
int findcycle(char* line, FILE* in,int i,int *code);
int iscomm(char* line, FILE* in, int i, int *code);
int afterfindcycle(char* line, FILE* in, int i, int *code);
int bracket(char* line, FILE* in, int i, int *code);

/**************************************************************/
/*            О С Н О В Н А Я   П Р О Г Р А М М А             */
/**************************************************************/
int main()
{
	/****************************************************************/
	/*                   П Е Р Е М Е Н Н Ы Е                       */
	/***************************************************************/
	char line[N];
	FILE* in = fopen("input.txt", "rt");
	int i = 0;
	int maxdegree = 0;
	int diff = 0;
	int degree = 0;
	int countif = 0;
	int countdo = 0;
	int linecycle = -1;
	int code = 0;

	setlocale(LC_ALL, "RUS");

	if (in == 0 || in == NULL) { 
		perror("Ошибка");
		return 0;
	}
	
	while (fgets(line, 256, in) != NULL)
	{	
		i = 0;
		while(i<256 && line[i] != '\0')
		{
			i = iscomm(line, in, i, &code);
			if (code == exit)
			{
				printf("%s", "Максимальная степень вложенности циклов = ");
				printf("%d", maxdegree);
				fclose(in);
				return 0;
			}
			i = findcycle(line, in, i, &code);
			
			if (code == exit)
			{
				printf("%s", "Максимальная степень вложенности циклов = ");
				printf("%d", maxdegree);
				fclose(in);
				return 0;
			}
			else if ((code== afterdonextcycle )||( code== aftercyclenextcycleonthisline) || (code == aftercyclenextcycle))
			{
				if (linecycle == -1)
				{
					linecycle = degree;
				}
				degree++;

				if (maxdegree < degree)
				{
					maxdegree = degree;
				}
				if (code == afterdonextcycle) {
					countdo = diff;
					i = -1;
				}
				if (code == aftercyclenextcycleonthisline)
				{
					i = i - 1;
				}
				else {
					i = -1;
				}
			}
			else if (code == aftercyclebrace || code == afterdobrace)
			{
				degree++;
				diff++;
				if (maxdegree < degree)
				{
					maxdegree = degree;
				}
				if (code == afterdobrace)
				{
					countdo = diff;
				}
			}
			else if (code== afterconditionbrace)
			{
				diff++;
				countif = diff;
				
			}
			else if (code == aftercyclesemicolon || code== afterdosemicolon)
			{
				degree++;
				if (maxdegree < degree)
				{
					maxdegree = degree;
				}
				if (linecycle != -1)
				{
					degree = linecycle;
					linecycle = -1;
				}
				else
					degree--;
			}
			else if (code == afterconditioncycle)
			{
				i = -1;
			}
			else if (line[i] == '}' && diff>0)
			{
				if (countdo == diff && diff != 0)
				{
					while (line[i] != ';')
					{
						i++;
					}
				}
				if (countif == diff)
				{
					diff--;
					countif = 0;
				}
				else
				{
					degree--;
					diff--;
				}
			}
			i++;
			code = 0;
		}
	}
	if (ferror(in))
	{
		printf("%s","Текстовый файл поврежден.");
		return 1;
	}
	else {
		printf("%s", "Максимальная степень вложенности циклов = ");
		printf("%d", maxdegree);
		fclose(in);
	}
}
int findcycle(char*line,FILE*in,int i,int *code)
{

	int z = 0;
	char* equalswhile=NULL;
	char* equalsfor=NULL;
	char* equalsif = NULL;
	char* equalselse = NULL;
	char* equalsdo = NULL;
	checkkeywords(line, &equalswhile, &equalsfor, &equalsif, &equalselse, &equalsdo);

	if((equalswhile==(line+i))||(equalsfor==(line+i)))
	{
		for (z; z <= i; z++)
		{
			line[i] = ' ';
		}
		i=bracket(line, in, i,code);
		checkkeywords(line, &equalswhile, &equalsfor, &equalsif, &equalselse, &equalsdo);
		while (i != 256 && line[i] != '\n')
		{
			if (line[i] == '{')
			{
				*code = aftercyclebrace;
				return i;
			}
			if ((equalswhile == (line + i)) || (equalsfor == (line + i)) || (equalsif == (line + i)) || (equalsdo == (line + i)))
			{
				*code = aftercyclenextcycleonthisline;
				return i;
			}
			if (line[i] == ';')
			{
				*code = aftercyclesemicolon;
				return i;
			}
			i++;
		}
		if (fgets(line, 256, in) != NULL)
		{
			i = afterfindcycle(line, in, i, code);
			return i;
		}
		else 
		{
			*code = exit;
			return i;
		}
	}
	if ((equalsif == (line + i)) || (equalselse == (line + i)))
		{
		i = bracket(line, in, i, code);
		while (i != 256 && line[i] != '\n')
		{
			if (line[i] == '{')
			{
				*code = afterconditionbrace;
				return i;
			}
			if(line[i]==';')
			{
				return i;
			}
			i++;
		}
		if (fgets(line, 256, in) != NULL)
		{
			i = afterfindcycle(line, in, i, code);
			*code=*code+2000;
			return i;
		}
		else 
		{
			*code = exit;
			return i;
		}
	}
	if ((equalsdo == (line + i)))
	{
		while (i != 256 && line[i] != '\n')
		{	
			if (line[i] == '{')
			{
				*code = afterdobrace;
				return i;
			}
			i++;
		}
		if (fgets(line, 256, in) != NULL)
		{
			i = afterfindcycle(line, in, i, code);
			*code =*code + 1000;
			return i;
		}
		else 
		{
			*code = exit;
			return i;
		}
	}
	return i;
}

int bracket(char* line, FILE* in, int i, int *code)
{
	int countbracket = 0;
	int begini = i;
	while (line[i] != '(' && line[i]!='\n')
	{
		i++;
	}
	if (line[i] == '(')
	{
		countbracket++;
		while (countbracket != 0)
		{
			i++;
			if (line[i] == '(')
			{
				countbracket++;
			}
			if (line[i] == ')')
			{
				countbracket--;
			}
		}
		return i;
	}
	else
	{
		i = begini;
		return i;
	}
	
}
int afterfindcycle(char* line, FILE* in, int i, int *code)
{
	int z = 0;
	char* equalswhile = NULL;
	char* equalsfor = NULL;
	char* equalsif = NULL;
	char* equalselse = NULL;
	char* equalsdo = NULL;
	i = 0;
	checkkeywords(line, &equalswhile, &equalsfor, &equalsif, &equalselse, &equalsdo);
	while(i < 256 && line[i] != '\n')
	{
		if (line[i] == '{')
		{
			*code = 600;
			return i;
		}
		else if((equalswhile == (line + i)) || (equalsfor == (line + i)) || (equalsif == (line + i)) || (equalsdo == (line + i)))
		{
			*code = 400;
			return i;
		}
		else if (line[i] == ';')
		{
			*code = 800;
			return i;
		}
		i++;
	}

	if (fgets(line, 256, in) != NULL)
	{
		i = afterfindcycle(line, in, i, code);
		return i;
	}
	else {
		*code = 300;
		return i;
	}
}
void checkkeywords(char* line, char **equalswhile, char** equalsfor, char** equalsif, char** equalselse, char** equalsdo)
{
	*equalswhile = strstr(line, while1);
	if (*equalswhile == NULL)
	{
		*equalswhile = strstr(line, while2);
	}
	*equalsfor = strstr(line, for1);
	if (*equalswhile == NULL)
	{
		*equalswhile = strstr(line, for2);
	}
	*equalsif = strstr(line, if1);
	if (*equalsif == NULL)
	{
		*equalsif = strstr(line, if2);
	}
	*equalselse = strstr(line, else1);
	if (*equalselse == NULL)
	{
		*equalselse = strstr(line, else2);
		if (*equalselse == NULL)
		{
			*equalselse = strstr(line, else3);
		}
	}
	*equalsdo = strstr(line, do1);
	if (*equalsdo == NULL)
	{
		*equalsdo = strstr(line, do2);
		if (*equalsdo == NULL)
		{
			*equalsdo = strstr(line, do3);
		}
	}
}

int iscomm(char* line, FILE* in, int i, int *code)
{
	if (line[i] == '/' && line[i + 1] == '/')
		while (i < 256 && line[i] != '\n')
		{
			if (line[i] == '*' && line[i + 1] == '/')
				break;
			i++;
		}

	if (line[i] == '/' && line[i + 1] == '*')
	{
		while ((line[i] != '*' || line[i + 1] != '/'))
		{
			if (line[i] == '\n')
			{
				if (fgets(line, 256, in) != NULL)
					i = 0;
				else
				{
					*code = exit;
					return i;
				}
			}
			i++;
		}
		return i;
	}
	else {
		return i;
	}
}
