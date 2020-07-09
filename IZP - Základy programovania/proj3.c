/********************************************************************************/
/*              Autor : Samuel Valaštín, xvalas10@stud.fit.vutbr.cz             */
/*              Login : xvalas10                                                */
/*              Dátum: 2019/12                                                  */
/*              Projekt 3, Bludisko                                             */
/********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <limits.h>

typedef struct {
  int rows;
  int cols;
  unsigned char *cells;
} Map;
//enumeration of walls in triangles
enum walls{LEFT_WALL=1,RIGHT_WALL=2,UPDOWN_WALL=4};
//enumeration of four possible directions
enum dir{LEFT,RIGHT,DOWN,UP};
//functions prototypes
void init_map(Map *map);
bool check_file(const char *file_name);
bool load_map(Map *map,const char *file_name);
void destroy_map(Map *map);
void help();
bool isborder(Map *map, int r, int c, int border);
bool check_map(Map *map);
bool triangle_with_up_wall(int r,int c);
int start_border(Map *map,int r, int c, int leftright);
bool check_position(Map *map,int r,int c);
void print_position(int r,int c);
int rpath(Map *map,int r,int c);
bool triangle_without_walls(Map *map,int r,int c);
int lpath(Map *map,int r,int c);
bool check_input_values(int input_r,int input_c);
bool is_not_inside(Map *map,int input_r,int input_c);
bool border_input(Map *map,int input_r,int input_c);


int main(int argc,char *argv[])
{
	Map map;
	const char *subor;
	if((argc == 2) && strcmp("--help",argv[1])==0)
	{
		help();
	}
	else if((argc == 3) && strcmp("--test",argv[1])==0)
	{
		subor = argv[2];
		init_map(&map);
		if(!load_map(&map,subor))
		{
			fprintf(stdout,"Invalid\n");
		}
		else
		{
			fprintf(stdout,"Valid\n");
		}
		destroy_map(&map);
	}
	else if((argc == 5) && strcmp("--rpath",argv[1])==0)
	{
		int input_rows,input_cols;
		char *endpt;
		input_rows = strtol(argv[2],&endpt,10);
		input_cols = strtol(argv[3],&endpt,10);
		if(*endpt != '\0')
		{
			fprintf(stderr,"Invalid input -> use ./proj3 -- help\n");
			return -1;
		}
		subor = argv[4];
		init_map(&map);
		if(!load_map(&map,subor))
		{
			fprintf(stderr,"Invalid map! -> use ./proj3 -- help\n");
			destroy_map(&map);
			return -1;
		}
		if(!is_not_inside(&map,input_rows,input_cols))
		{
			fprintf(stderr,"Invalid input values use ./proj3 --help\n");
			return -1;
		}
		check_input_values(input_rows,input_cols);
		if(!border_input(&map,input_rows,input_cols))
		{
			destroy_map(&map);
			fprintf(stderr,"Cannot go to the labyrinth by the wall use ./proj3 --help\n");
			return -1;
		}
		rpath(&map,input_rows,input_cols);
		destroy_map(&map);
	}
	else if((argc == 5) && strcmp("--lpath",argv[1])==0)
	{
		int input_rows,input_cols;
		char *endpt;
		input_rows = strtol(argv[2],&endpt,10);
		input_cols = strtol(argv[3],&endpt,10);
		if(*endpt != '\0')
		{
			fprintf(stderr,"Invalid input -> use /proj3 -- help\n");
			return -1;
		}
		subor = argv[4];
		init_map(&map);
		if(!load_map(&map,subor))
		{
			fprintf(stderr,"Invalid map! -> use ./proj3 -- help\n");
			destroy_map(&map);
			return -1;
		}
		if(!is_not_inside(&map,input_rows,input_cols))
		{
			fprintf(stderr,"Invalid input values use ./proj3 --help\n");
			destroy_map(&map);
			return -1;
		}
		if(!border_input(&map,input_rows,input_cols))
		{
			destroy_map(&map);
			fprintf(stderr,"Cannot go to the labyrinth by the wall use ./proj3 --help\n");
			return -1;
		}
		lpath(&map,input_rows,input_cols);
		destroy_map(&map);
	}
	else
	{
		fprintf(stderr,"Use help! ./proj3 --help\n");
	}
	return 0;
}



/*Function initializing structure Map
 * @param *map       pointer to structure  Map
 */
void init_map(Map *map)
{
	map->rows = 0;
	map->cols = 0;
	map->cells = NULL;
}


/*Function check if there are only digits & isspaces in our text file
 * param @char *file_name          file we have to read from
 * returns true if there are only digits and isspaces
 * returns false if there are some ilegal chars
 */
bool check_file(const char *file_name)
{
	int i;
	FILE *file;
	if((file = fopen(file_name,"r"))==NULL)
	{
		fclose(file);
		return false;
	}
	while((i = fgetc(file))!=EOF)
	{
		if(!isspace(i) && !isdigit(i))
		{
			fclose(file);
			return false;
		}
	}
	fclose(file);
	return true;
}


/*Function load map from text file and also checks if is valid
 * param @map               pointer to structure map
 * param @file_name         file we have to read from
 * returns true if map is valid
 * return false if map is invalid
 */
bool load_map(Map *map,const char *file_name)
{
	int element;
	if(!check_file(file_name))
	{
		return false;
	}
	FILE *file;
	file = fopen(file_name,"r");
	if(file == NULL)
	{
		fclose(file);
		return false;
	}
	fscanf(file,"%d %d",&map->rows,&map->cols);
	map->cells = malloc(map->rows * map->cols * sizeof(unsigned char)); 
	if(map->cells == NULL)
	{
		fclose(file);
		return false;
	}
	for(int row = 0; row < map->rows;row++)
	{
		for(int col = 0; col < map->cols;col++)
		{
			fscanf(file,"%d",&element);
			map->cells[(row*map->cols)+col] = element;
			
		}
	}
	fclose(file);
	if(!check_map(map))
	{
		destroy_map(map);
		return false;
	}
	return true;
}


/* Function free our map
 * param @Map *map pointer to structure Map 
 */
void destroy_map(Map *map)
{
	free(map->cells);
}


/*Function print help if first argument is --help
 */ 
void help()
{
	puts("Projekt 3 - Bludisko \n"
	"./proj3 --help\n"
	"./proj3 --test subor.txt\n"
	"./proj3 --rpath R C subor.txt\n"
	"./proj3 --lpath R C subor.txt\n"
	"Program sa pouziva pomocou vyssie uvedeneych argumentov:\n"
	"--help vypise napoveda a ukonci program\n"
	"--test zisti, ci subor.txt obsahuje validnu mapu bludiska\n"
	"--rpath hlada cestu z bludiska pomocou pravidla pravej ruky:[R]=riadok,[C]=stlpec\n"
	"--lpath hlada cestu z bludiska pomocou pravidla lavej ruky:[R]=riadok,[C]=stlpec\n");
}


/*Function works with map and checks if there are borders in the walls
 * @param r        actual row
 * @param c        actual coll
 * @param border   wall we asked for
 * function returns true if there is a wall
 * fuction returns false if there is not a wall
 */
bool isborder(Map *map, int r, int c, int border)                               
{                                                                               
	int cell = map->cells[(r-1)*map->cols+(c-1)];
	if(border == LEFT_WALL)
	{
		if((cell&LEFT_WALL) != 0)
		{
			return true;
		}
	}
	if(border == RIGHT_WALL)
	{
		if((cell&RIGHT_WALL) != 0)
		{
			return true;
		}
	}
	if(border == UPDOWN_WALL)
	{
		if((cell&UPDOWN_WALL) != 0)
		{
			return true;
		}
	}
		return false;
}



/*Function checks if there are valid walls in our Map structure
 * @param *map          pointer to structure mapu
 * returns true if there are not invalid walls inside the labyrinth
 * returns false if there are some invalid walls inside the labyrinth
 */
bool check_map(Map *map)
{
	int ch_horizontal = 0; 
	int ch_vertical = 0;
	int j,k;
	for(int i = 1; i < map->rows+1;i++)                                           
	{
		for(j = 1,k = 2;j < map->cols;j++,k++)
		{
			if(isborder(map,i,j,RIGHT_WALL) && !isborder(map,i,k,LEFT_WALL))             //checks left and right walls is the cells
			{
				ch_horizontal++;
			}
			else if(!isborder(map,i,j,RIGHT_WALL) && isborder(map,i,k,LEFT_WALL))
			{
				ch_horizontal++;
			}
		}
	}
	for(int l = 1; l <= map->rows-1;l=l+2)
	{
		for(int m = 2; m <= map->cols-1;m=m+2)                                                 //checks UP and DOWN borders in the cells in even rows
		{
			if(isborder(map,l,m,UPDOWN_WALL) && !isborder(map,l+1,m,UPDOWN_WALL))
			{
				ch_vertical++;
			}
			else if(!isborder(map,l,m,UPDOWN_WALL) && isborder(map,l+1,m,UPDOWN_WALL))
			{
				ch_vertical++;
			}
		}
	}
	for(int n = 2; n <= map->rows-1;n=n+2) 
	{
		for(int p = 1; p<= map->cols;p=p+2)
		{
			if(isborder(map,n,p,UPDOWN_WALL) && !isborder(map,n+1,p,UPDOWN_WALL))
			{
				
				ch_vertical++;                                                                          //checks Up and Down borders in the cells in odd rows
			}
			else if(!isborder(map,n,p,UPDOWN_WALL) && isborder(map,n+1,p,UPDOWN_WALL))
			{
				ch_vertical++;
			}
		}
	}
	if(ch_vertical == 0 && ch_horizontal == 0)                                                // if there are not any ilegal borders return true
	{
		return true;
	}
	else
	{
		return false;
	}
}


/*Function returns true if is it triangle with up wall, if triangle has down wall returns false
 * @param r            actual row
 * @param c            actual coll
 */
bool triangle_with_up_wall(int r,int c)
{
	if((r+c)%2 == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/*Function returns start direction 
 * @param *map         pointer to structure map
 * @param r            input row
 * @param c            input coll
 * @param leftright    if leftright == RIGHT we use right hand rule, else left hand
 */

int start_border(Map *map,int r, int c, int leftright)                         
{
	if(leftright == RIGHT)                                                         // start direction for rpath
	{
		if((triangle_with_up_wall(r,c)) && (c == 1))
		{
			if(isborder(map,r,c,RIGHT_WALL))
			{
				return UP;
			}
			else
			{
				return RIGHT;
			}
		}
		else if((!triangle_with_up_wall(r,c)) && (c == 1))
		{
			if(isborder(map,r,c,RIGHT))
			{
				return DOWN;
			}
			else
			{
				return RIGHT;
			}
		}
		else if(triangle_with_up_wall(r,c) && (c == map->cols))
		{
			if(isborder(map,r,c,UPDOWN_WALL))
			{
				return LEFT;
			}
			else if(isborder(map,r,c,LEFT_WALL) && !isborder(map,r,c,UPDOWN_WALL))
			{
				return UP;
			}
		}
		else if(!triangle_with_up_wall(r,c) && (c== map->cols))
		{
			if(isborder(map,r,c,UPDOWN_WALL))
			{
				return LEFT;
			}
			else if(isborder(map,r,c,LEFT_WALL) && !isborder(map,r,c,UPDOWN_WALL))
			{
				return DOWN;
			}
		}
		else if((triangle_with_up_wall(r,c)) && (r==1))
		{
			if(isborder(map,r,c,RIGHT_WALL))
			{
				return RIGHT;
			}
			else
			{
				return LEFT;
			}
		}
		else if(!triangle_with_up_wall(r,c) && (r == map->rows))
		{
			if(isborder(map,r,c,LEFT_WALL))
			{
				return RIGHT;
			}
			else
			{
				return LEFT;
			}
		}
	}
	
	else if(leftright == LEFT)                                              //start direction for lpath
	{
		if((triangle_with_up_wall(r,c)) && (c == 1))
		{
			if(isborder(map,r,c,RIGHT_WALL))
			{
				return UP;
			}
			else
			{
				return RIGHT;
			}
		}
		else if((!triangle_with_up_wall(r,c)) && (c == 1))
		{
			if(isborder(map,r,c,RIGHT_WALL))
			{
				return DOWN;
			}
			else
			{
				return RIGHT;
			}
		}
		else if(triangle_with_up_wall(r,c) && (c == map->cols))
		{
			if(isborder(map,r,c,LEFT_WALL))
			{
				return UP;
			}
			else
			{
				return LEFT;
			}
		}
		else if(!triangle_with_up_wall(r,c) && (c == map->cols))
		{
			if(isborder(map,r,c,LEFT_WALL))
			{
				return DOWN;
			}
			else
			{
				return LEFT;
			}
		}
		else if((triangle_with_up_wall(r,c)) && (r==1))
		{
			if(isborder(map,r,c,LEFT_WALL))
			{
				return RIGHT;
			}
			else if(isborder(map,r,c,LEFT_WALL) && isborder(map,r,c,RIGHT_WALL))
			{
				return UP;
			}
			else
			{
				return LEFT;
			}
		}
		else if(!triangle_with_up_wall(r,c) && (r == map->rows))
		{
			if(isborder(map,r,c,LEFT_WALL) && isborder(map,r,c,RIGHT_WALL))
			{
				return DOWN;
			}
			else if(isborder(map,r,c,LEFT_WALL))
			{
				return RIGHT;
			}
			else
			{
				return LEFT;
			}
		}
	}
	return 0;
}


/*Function check actual position and if we are still inside returns true if we are not inside returns false
 * @param *map      pointer to structure map
 * @param r         actual row
 * @param c         actual coll
 */
bool check_position(Map *map,int r,int c)
{
	if((r >= 1 && r <= map->rows) && (c >= 1 && c <= map->cols))
	{
		return true;
	}
	return false;
}


/*Function print current position
 */
void print_position(int r,int c)
{
	printf("%d,%d\n",r,c);
}
	


/*Function searching exit from the labyrinth by right hand rule
 * @param *map           pointer to structure Map
 * @int r                actual row
 * @int c                actual coll
 */
int rpath(Map *map,int r,int c)
{
	int dir = start_border(map,r,c,RIGHT);
	print_position(r,c);
	while(check_position(map,r,c))
	{
		if(dir == RIGHT)
		{
			if(!check_position(map,r,c))
			{
				break;
			}
			while(!isborder(map,r,c,RIGHT_WALL) && check_position(map,r,c))
			{
				c++;
				if(check_position(map,r,c))
				{
					print_position(r,c);
				}
				else
				{
					break;
				}
			}
			if(isborder(map,r,c,RIGHT_WALL))
			{
				if(triangle_with_up_wall(r,c))
				{
					dir = UP;
				}
				else if(!triangle_with_up_wall(r,c) && !isborder(map,r,c,UPDOWN_WALL))
				{
					dir = DOWN;
				}
				else if(isborder(map,r,c,RIGHT_WALL) && isborder(map,r,c,UPDOWN_WALL))
				{
					dir = LEFT;
				}
			}
		}
		if(dir == LEFT)
		{
			while(!isborder(map,r,c,LEFT_WALL) && check_position(map,r,c))
			{
				c--;
				if(check_position(map,r,c))
				{
					print_position(r,c);
				}
				else
				{
					break;
				}
			}
			if(isborder(map,r,c,LEFT_WALL) && (dir == LEFT))
			{
				if(triangle_with_up_wall(r,c))
				{
					dir = UP;
				}
				else if(!triangle_with_up_wall(r,c) && check_position(map,r,c+1))
				{
					dir = RIGHT;
				}
				else
				{
					break;
				}
				
			}
		}
		if(dir == UP)
		{
			r--;
			print_position(r,c);
			if(isborder(map,r,c,RIGHT_WALL))
			{
				dir = LEFT;
			}
			else if(isborder(map,r,c,LEFT_WALL))
			{
				dir = RIGHT;
			}
			else
			{
				dir = RIGHT;
			}
		}
		if(dir == DOWN)
		{
			r++;
			print_position(r,c);
			if(isborder(map,r,c,LEFT_WALL))
			{
				dir = RIGHT;
			}
			else
			{
				dir = LEFT;
			}
		}
	}
	return 0;
}

/*Function returns true if there are no walls in triangle, else returns false 
 * @param *map            pointer to structure Map
 * @param r               actual row
 * @param c               actual coll
 */
bool triangle_without_walls(Map *map,int r,int c)
{
	if(!isborder(map,r,c,LEFT_WALL) && !isborder(map,r,c,RIGHT_WALL) && !isborder(map,r,c,UPDOWN_WALL))
	{
		return true;
	}
	return false;
}

/*Function searching exit by the left hand rule
 * @param *map               pointer to structure Map
 * @param r                  actual row
 * @param c                  actual coll
 */
int lpath(Map *map,int r,int c)
{
	int dir = start_border(map,r,c,LEFT);
	print_position(r,c);
	while(check_position(map,r,c))
	{
		if(dir == LEFT)
		{
			if(!check_position(map,r,c))
			{
				break;
			}
			if(triangle_without_walls(map,r,c) && !triangle_with_up_wall(r,c) && check_position(map,r,c))
			{
				c--;
				if(check_position(map,r,c))
				{
					print_position(r,c);
				}
				else
				{
					break;
				}
			}
			while((isborder(map,r,c,RIGHT_WALL) || isborder(map,r,c,UPDOWN_WALL)) && check_position(map,r,c))
			{
				c--;
				if(check_position(map,r,c))
				{
					print_position(r,c);
				}
				else
				{
					break;
				}
			}
			if(triangle_with_up_wall(r,c) && isborder(map,r,c,LEFT_WALL))
			{
				dir = UP;
			}
			else if(!triangle_with_up_wall(r,c) && isborder(map,r,c,LEFT_WALL))
			{
				dir = DOWN;
			}
			else if(triangle_without_walls(map,r,c))
			{
				dir = DOWN;
			}
		}
		if(dir == RIGHT)
		{
			if(!check_position(map,r,c))
			{
				break;
			}
			if(!isborder(map,r,c,RIGHT_WALL))
			{
				c++;
				if(check_position(map,r,c))
				{
					print_position(r,c);
				}
				else
				{
					break;
				}
			}
			while(isborder(map,r,c,UPDOWN_WALL) && check_position(map,r,c))
			{
				c++;
				if(check_position(map,r,c))
				{
					print_position(r,c);
				}
				else
				{
					break;
				}
			}
			if(isborder(map,r,c,RIGHT_WALL) && triangle_with_up_wall(r,c))
			{
				dir = UP;
			}
			else if(isborder(map,r,c,RIGHT_WALL) && !triangle_with_up_wall(r,c))
			{
				dir = DOWN;
			}
			else if(triangle_without_walls(map,r,c))
			{
				dir = UP;
			}
				
		}
		if(dir == UP && check_position(map,r,c))
		{
			r--;
			if(check_position(map,r,c))
			{
				print_position(r,c);
			}
			else
			{
				break;
			}
			if(isborder(map,r,c,LEFT_WALL) && !triangle_with_up_wall(r,c))
			{
				dir = RIGHT;
			}
			else if(isborder(map,r,c,RIGHT_WALL) && !triangle_with_up_wall(r,c))
			{
				dir = LEFT;
			}
			else if(triangle_without_walls(map,r,c))
			{
				dir = LEFT;
			}
		}
		if(dir == DOWN && check_position(map,r,c))
		{
			r++;
			if(check_position(map,r,c))
			{
				print_position(r,c);
			}
			else
			{
				break;
			}
			if(isborder(map,r,c,LEFT_WALL) && isborder(map,r,c,RIGHT_WALL))
			{
				dir = UP;
			}
			if(isborder(map,r,c,RIGHT_WALL) && !isborder(map,r,c,LEFT_WALL))
			{
				dir = LEFT;
			}
			else if(triangle_without_walls(map,r,c))
			{
				dir = RIGHT;
			}
		}
	}
	return 0;
}	

/*Function checks if input values are positive then returns true
 * @param input_r    input row
 * @param input_c    input col
 */

bool check_input_values(int input_r,int input_c)
{
	if((input_r < 1 || input_r > INT_MAX) || (input_c < 1 || input_c > INT_MAX))
	{
		fprintf(stderr,"Invalid position \n");
		return false;
	}
	return true;
}

/*Funkcia check if we start outside the labyrinth
 * @param *map           pointer to structure Map
 * @param input_r        input row
 * @param input_c        input coll
 */
bool is_not_inside(Map *map,int input_r,int input_c)
{
	if((input_r == 1) && ((input_c >= 1) && (map->cols >= input_c)))
	{
		return true;
	}
	else if((input_c == 1) && (input_r >=1 && (map->rows >= input_r)))
	{
		return true;
	}
	else if(input_r == map->rows && (input_c >=1 && (map->cols >= input_c)))
	{
		return true;
	}
	else if(input_c == map->cols && (input_r >=1 && (map->rows >= input_r)))
	{
		return true;
	}
	else
	{
		return false;
	}
}


/*Function checks if it is possible to start searching the exit
 * @param *map           pointer to structure map
 * @param input_r        input row
 * @param input_c        input coll
 */
bool border_input(Map *map,int input_r,int input_c)
{
	if(input_c == 1)
	{
		if(!isborder(map,input_r,input_c,LEFT_WALL))
		{
			return true;
		}
	}
	else if(input_c == map->cols)
	{
		if(!isborder(map,input_r,input_c,RIGHT_WALL))
		{
			return true;
		}
	}
	else if(input_r == 1)
	{
		if(triangle_with_up_wall(input_r,input_c) && !isborder(map,input_r,input_c,UPDOWN_WALL))
		{
			return true;
		}
	}
	else if(input_r == map->rows)
	{
		if(!triangle_with_up_wall(input_r,input_c) && !isborder(map,input_r,input_c,UPDOWN_WALL))
		{
			return true;
		}
	}
	return false;
}	
	

