/********************************************************************************/
/*              Autor : Samuel Valaštín, xvalas10@stud.fit.vutbr.cz             */
/*              Login : xvalas10                                                */
/*              Dátum: 2019/11                                                  */
/*              Projekt 2, Iteračné výpočty                                     */
/********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

//constants
#define SATURATION_STREAM (1e-12)
#define HEAT_VOLTAGE (25.8563e-3)
#define VALID_DOUBLE 1e-15
#define NO_ERROR 0
#define ERROR -1

//function prototypes
double equation(double Up,double U0, double R);
double diode(double U0, double R, double eps);
double search_current(double Up);
bool check_arguments(int argc);
bool check_values(double Up,double R,double eps);
bool check_input(char *check_first,char *check_second,char *check_third);

int main(int argc,char *argv[])
{
	double U0,R,eps;
	char *check_first,*check_second,*check_third;
	if(!check_arguments(argc))
	{
		return ERROR;
	}
	U0 = strtod(argv[1],&check_first);                                       //converting arguments from char* to double
	R = strtod(argv[2],&check_second);
	eps = strtod(argv[3],&check_third);
	if(!check_input(check_first,check_second,check_third))
	{
		return ERROR;
	}
	if(!check_values(U0,R,eps))
	{
		return ERROR;
	}
	double voltage = diode(U0,R,eps);
	double current = search_current(voltage);
	printf("Up=%g V\nIp=%g A\n",voltage,current);
	return NO_ERROR;
}


/* Function expresses the first Kirchhoff law
 * @param Up     Estimate diode voltage 
 * @param U0     Source voltage
 * @param R      Resistance
 * function return result of this equation
 * */
double equation(double Up,double U0,double R)
{
	double x = Up/HEAT_VOLTAGE;
	double result = SATURATION_STREAM*(exp(x)-1) - (U0-Up)/R;
	return result;
}


/*Function search Up (diode Voltage) by using the bisection method from 0 to Source Voltage
 * @param U0    Source Voltage
 * @param R     Resistance
 * @param eps   Deviation(epsilon)
 * function return searched Up
 * */
double diode(double U0,double R,double eps)
{
	double left = 0;
	double right = U0;
	double Up,f_value_Up;
	double f_value_right = equation(right,U0,R);                          //function value of right side
	const double reach_limit = 1e-15;                                     // double limit
	bool limit_count = false;
	while(fabs(right-left) > eps && !limit_count)
	{
		Up = (left+right)/2;
		f_value_Up = equation(Up,U0,R);
		if(f_value_right * f_value_Up > 0)                              // if root is not located between middle and right side
		{
			right = Up;
			f_value_right = equation(right,U0,R);
		}
		else                                                                // else root is between left side and middle
		{
			left = Up;                                                  
		}
		if(fabs(right-left) < reach_limit)                                  //if absolute value of left side - right side is lower than double limit
		{
			limit_count = true;                                             // artificial variable break this iteration
		}
	}
	return Up;
}


/* Function search Ip
 * @param Up    Founded voltage
 * function return diode Working point current
 * */
double search_current(double Up)
{
	double x = Up/HEAT_VOLTAGE;
	return  SATURATION_STREAM *(exp(x)-1);
}


/* Function check if arguments are valid
 * @param U0         Source Voltage
 * @param R          Resistance
 * */
bool check_values(double U0, double R,double eps)
{
	if((U0 < 0) || (R <= 0) ||(eps < 0))
	{
		fprintf(stderr,"error: invalid arguments\n");
		return false;
	}
	return true;
}


/* Function check the user has entered three arguments
 * @param argc   Number of arguments
 */
bool check_arguments(int argc)
{
	if(argc != 4)
	{
		fprintf(stderr,"error: add 3 arguments [U0][R][eps]\n");
		return false;
	}
	return true;
}


/* Function check if there is a valid input, 
 * @param check_first      Checks if first argument is valid
 * @param check_second     Checks if second argument is valid
 * @param check_third      Checks if third argument is valid
 */
bool check_input(char *check_first,char *check_second,char *check_third)
{
	if(strlen(check_first) > 0 || strlen(check_second) > 0 || strlen(check_third) > 0)    // if length of non double value is greater than 0 return false
	{
		fprintf(stderr,"error: invalid input\n");
		return false;
	}
	return true;
}
