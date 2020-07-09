/********************************************************************************/
/*              Autor : Samuel Valaštín, xvalas10@stud.fit.vutbr.cz             */
/*              Login : xvalas10                                                */
/*              Dátum: 2019/11                                                  */
/*              Projekt 1, Praca s textom                                       */
/********************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
// 100 znakov + '\n' + '\0'
#define LINE_MAX 102
//prototypy funkcii
bool check_input(char *input);
void print_file();
void load_file(char *input);
bool check_length(char *contact_name, char *contact_number);
bool find_name (char *contact_name, char *input);
bool find_number(char *contact_number, char *input);
void edit_strings(char *contact_name,char *contact_number);
void contact_name_tolower(char *contact_name);
int main(int argc, char *argv[])
{
	char *input;
	input = argv[1];
	if(argc == 1)
	{
		print_file();
	}
	else if (argc == 2)
	{
		if (!check_input(input))                                                     // ak je input vyhodnoteny ako false ukonci program
		{
			return -1;
		}
		load_file(input);
	}
	else                                                                              
	{
		fprintf(stderr,"Invalid input - add max 1 argument\n");                     
	}
	return 0;
}


// funkcia check input kontroluje vstup -> ak sa sklada z cifier 0-9 vracia true inak vracia false
bool check_input(char *input)
{
	for(int i = 0; input[i]!='\0';i++)
	{
		if(!isdigit(input[i]))
		{
			fprintf(stderr,"Invalid argument(only 0 - 9 characters)!\n");
			return false;
		}
	}
	return true;
}


/* funkcia nacitava riadky zo suboru vo formate meno, tel.cislo 
 zaroven pomocou funkcie tolower z kniznice ctype prevedie mena na male znaky */
void load_file(char *input)
{
	int searched_contacts = 0;
	char contact_name[LINE_MAX];
	char contact_number[LINE_MAX];
	while(fgets(contact_name,LINE_MAX,stdin)!=NULL)
	{
		fgets(contact_number,LINE_MAX,stdin);
		if(!check_length(contact_name,contact_number))
		{
			return;
		}
		contact_name_tolower(contact_name);
		edit_strings(contact_name,contact_number);
		if(find_name(contact_name,input)||find_number(contact_number,input))             //ak sa nasla zhoda v mene alebo cisle printuje kontakt
		{
			searched_contacts++;
			printf("%s, %s\n",contact_name,contact_number);
		}
	}
	if(searched_contacts == 0)                                                         //ak sa nenasla ziadna zhoda vracia not found                                                 
	{
	printf("Not found\n");
	}
	
}


//funkcia odstrihne new line character a uzatvori string
void edit_strings(char *contact_name, char *contact_number)
{
	contact_name[strlen(contact_name)-1]='\0';                                       
	contact_number[strlen(contact_number)-1]='\0';
}

//funkcia prechadza kontakt po znakoch a prevadza velke znaky na male
void contact_name_tolower(char *contact_name)
{
	for(int i = 0;contact_name[i]!='\0';i++)
		{
			contact_name[i] = tolower(contact_name[i]);                                   
		}
}

/*Funkcia find_name hlada prerusovanu postupnost mena, pouziva  pointerov na pole znakov
  pricom kazde pole reprezentuje cislo 0 - 9, napr 2 reprezentuje abc*/
bool find_name (char *contact_name, char *input)
{
	unsigned int input_length = strlen(input);
	unsigned int contact_name_length = strlen(contact_name);
	char *alphabet[] = {"+","","abc","def","ghi","jkl","mno","pqrs","tuv","wxyz"};     //pole pointerov
	unsigned int find_name_match = 0;                                                      
	int name_position = 0;
	char *to_compare = alphabet[(input[name_position]-'0')];                        //pretypovanie inputu (char to int) a zaroven pointer ktory ukazuje na prvy znak inputu - pretoze name_position = 0                      
	char *count;
	if(input_length > contact_name_length)                                            // pokial je dlzka inputu > dlzka riadku vrati false
	{
		return false;
	}
	for(int i = 0;contact_name[i]!='\0';i++)
	{
		count = strchr(to_compare,contact_name[i]);                                     //funkcia strchr vracia ukazatel pokial najde ak nie vracia NULL
		if(count!=NULL)                                                                 // ak je zhoda posunie sa na dalsiu poziciu inputu 
		{
			find_name_match++;                                                               
			name_position++;
			to_compare = alphabet[(input[name_position]-'0')];
		}
		if(find_name_match == input_length)                                           // pokial najde cely vstup vracia true
		{
			return true;
		}
	}
	return false;
}



/* Funkcia hlada prerusovanu postupnost cisel v telefonnych cislach kontaktov */
bool find_number(char *contact_number, char *input)
{
	unsigned int input_length = strlen(input);
	unsigned int contact_number_length = strlen(contact_number);
	unsigned int find_number_match = 0;
	int number_position = 0;
	if(input_length > contact_number_length)                       //ak je input > ako dlzka tel cisla vrati false
	{
		return false;
	}
	for (int j = 0;contact_number[j]!='\0';j++)
	{
		if(contact_number[j] == input[number_position])              //ak je zhoda posuva sa na dalsiu poziciu inputu 
		{
			find_number_match++;
			number_position++;
		}
		if(find_number_match == input_length)                      // pokial najde cely input vrati true
		{
			return true;
		}
	}
	return false;
}
	
	
// funkcia kontroluje dlzku mena a tel. cisla aby sme zabranili pripadnemu preteceniu
bool check_length(char *contact_name,char *contact_number)
{
	if((contact_name[strlen(contact_name)-1] != '\n') || (contact_number[strlen(contact_number)-1] != '\n'))               // pokial sa v stringu nenechadza new line character vrati false
	{
		fprintf(stderr,"Too long line(max. 100 characters)!\n");
		return false;
	}
	else
	{
		return true;
	}
}

//ak uzivatel zada vyhladavanie bez argumentov upravi mu data na predpisany format
void print_file()
{
	char contact_name[LINE_MAX];
	char contact_number[LINE_MAX];
	while(fgets(contact_name,LINE_MAX,stdin)!=NULL)                                      //cyklus sa ukonci pokial nie je mozne nacitat dalsi kontakt
	{
		fgets(contact_number,LINE_MAX,stdin);
		if(!check_length(contact_name,contact_number))
		{
			return;
		}
		contact_name_tolower(contact_name);
		edit_strings(contact_name,contact_number);
		printf("%s, %s\n",contact_name,contact_number);
	}
}
