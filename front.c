#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "back.h"

// Formato csv 
#define OACI_LENGTH 5
#define DESCRIPTION_LENGTH 71
#define PROVINCE_LENGTH 70
#define MAX_LINE_LENGTH 300
#define MOVEMENT_TYPE 20
#define HOUR_LENGTH 6
#define DATE_LENGTH 11
#define SEPARATOR ";"

#define FIRST_QUERY_HEADER "OACI;Denominación;Movimientos\n"
#define SECOND_QUERY_HEADER "Día;Diurnos;Nocturnos;Total\n"
#define THIRD_QUERY_HEADER "Provincia;Porcentaje\n"


// Manejo de archivos
#define AIRPORT_CSV "aeropuertos2019.csv"
#define MOVS_CSV "movimientos2019.csv"

#define FIRST_QUERY_NAME "query1.csv"
#define SECOND_QUERY_NAME "query2.csv"
#define THIRD_QUERY_NAME "query3.csv"

#define EXPECTED_ARGUMENTS 3
#define FIRST_ARGUMENT 1
#define SECOND_ARGUMENT 2

#define READ "r"
#define WRITE "w+"


// Manejo de errores
#define TOO_MANY_ARGUMENTS "Error: too many arguments\n"
#define MORE_ARGUMENTS_EXPECTED "Error: more arguments expected\n"
#define FILE_ERROR_MESSAGE "File error has occurred. Check file existence or permissions\n"
#define ALLOCATION_ERROR_MESSAGE "Memory allocation error has occurred\n"

#define SUCCESS 0
#define ARGUMENT_ERROR 1
#define FILE_ERROR 2
#define ALLOCATION_ERROR 3


#define WEEK_DAYS 7

#define TRUE 1
#define FALSE 0

typedef enum {OACI, DESCRIPTION, PROVINCE} airport_field ;

typedef enum {DATE, HOUR, ORIGIN , DESTINATION , MOVE_TYPE} movement_field ;


/* 
* Funcion encargada de obtener los datos necesarios de cada linea del archivo aeropuertos2019.csv
*/
int parse_airport(char * airport_line, char * oaci_code, char * description_name, char * province_name){
  char * token=strtok(airport_line,SEPARATOR);
  airport_field field = OACI;

  while(token!=NULL){
    switch(field){
      case OACI: {
        strcpy(oaci_code,token);
      }
      break;
      case DESCRIPTION: {
        strcpy(description_name,token);
      }
      break;
      case PROVINCE:{
        strcpy(province_name,token);
      }
      break;
      default: break;
    }
    field++;
    token=strtok(NULL,SEPARATOR);
  }
  return SUCCESS;
}

/* 
* Recorre el archivo de aeropuertos para obtener datos y generar la lista de aeropuertos 
*/
int process_airports(FILE * airports , movementsADT movs){
  
  char airport_line[MAX_LINE_LENGTH];
  char * result = fgets(airport_line,MAX_LINE_LENGTH,airports);
  char oaci_code[OACI_LENGTH];
  char description_name[DESCRIPTION_LENGTH];
  char province_name[PROVINCE_LENGTH];
  int e;
  while(!feof(airports)){
    result= fgets(airport_line,MAX_LINE_LENGTH,airports);
    if(result!= NULL){
      parse_airport(airport_line,oaci_code,description_name,province_name);
      e=insert_airport(movs,oaci_code,description_name,province_name);
      if(e!=SUCCESS)
        return ALLOCATION_ERROR;
    }
  }
  return SUCCESS;
}

/* 
* Funcion encargada de obtener los datos necesarios de cada linea del archivo movimientos2019.csv
*/
void parse_movement(char *movements_line, char * new_date, char * new_hour, char * new_oaci_o, char * new_oaci_d, char *new_move_type){
  char * token=strtok(movements_line,SEPARATOR);
  movement_field field = DATE;

  while(token!=NULL){
    switch(field){
      case DATE:{
        strcpy(new_date,token);
        break;
      }
      case HOUR:{
        strcpy(new_hour,token);
        break;
      }
      case ORIGIN:{
        strcpy(new_oaci_o,token);
        break;
      }
      case DESTINATION:{
        strcpy(new_oaci_d,token);
        break;
      }
      case MOVE_TYPE:{
        strcpy(new_move_type,token);
        break;
      }
      default: break;
    }
    field++;
    token=strtok(NULL,SEPARATOR);
  }
}

/* 
* Recorre el archivo de movimientos para obtener datos, incrementar movimientos
* en aeropuertos y generar la lista de provincias 
*/
int process_movements(FILE * movements, movementsADT movs){

  char movements_line[MAX_LINE_LENGTH];
  char *result=fgets(movements_line,MAX_LINE_LENGTH,movements);
  char new_date[DATE_LENGTH];
  char new_hour[HOUR_LENGTH];
  char new_oaci_o[OACI_LENGTH];
  char new_oaci_d[OACI_LENGTH];
  char new_move_type[MOVEMENT_TYPE];

  while(!feof(movements)){
    result=fgets(movements_line,MAX_LINE_LENGTH,movements);
    if(result!=NULL){
      parse_movement(movements_line,new_date,new_hour,new_oaci_o,new_oaci_d,new_move_type);
      add_movement(movs,new_date,new_hour,new_oaci_o,new_oaci_d,new_move_type);
    }
  }
  
  int e=generate_provinces_movements(movs);
  
  return e==SUCCESS? SUCCESS : ALLOCATION_ERROR;
}

void generate_query_one(movementsADT movs, FILE *query_one){
  char oaci_code[OACI_LENGTH];
  char description_name[DESCRIPTION_LENGTH];
  
  fputs(FIRST_QUERY_HEADER,query_one);

  unsigned int movs_qty;

   int keep_going=TRUE;

  while(has_next_airport(movs) && keep_going){
    get_airport_info(movs,oaci_code,description_name,&movs_qty);
    
    if(movs_qty>0){
      fputs(oaci_code,query_one);
      fputs(SEPARATOR,query_one);
      fputs(description_name,query_one);
      fputs(SEPARATOR,query_one);
      fprintf(query_one,"%d\n", movs_qty);    
      next_airport(movs);
    }
    else
      keep_going=FALSE;
  }
}


void generate_query_two(movementsADT movs, FILE * query_two){
	fputs(SECOND_QUERY_HEADER, query_two);
	int day, night;
  const char * week[] = {"domingo", "lunes", "martes", "miercoles","jueves","viernes","sabado"};
	for(int i=0 ; i < WEEK_DAYS ; i++){
		fputs(week[i],query_two);
		fputs(SEPARATOR,query_two);
		get_days_info(movs,i,&day,&night);
		fprintf(query_two,"%d;%d;%d\n",day,night,day+night );
	}
}

void generate_query_three(movementsADT movs, FILE * query_three){
  fputs(THIRD_QUERY_HEADER,query_three);
  char province_name[PROVINCE_LENGTH];
  unsigned int percentage;
  
  int keep_going=TRUE;

  while(has_next_province(movs) && keep_going){
    get_province_info(movs,province_name,&percentage);
    if(percentage > 0){
      fputs(province_name,query_three);
      fputs(SEPARATOR,query_three);
      fprintf(query_three,"%d\n",percentage );
      next_province(movs);
    }
    else
      keep_going=FALSE;
  }
}

int generate_querys(movementsADT movs){
  FILE * query_one = fopen(FIRST_QUERY_NAME,WRITE);
  FILE * query_two = fopen(SECOND_QUERY_NAME,WRITE);
  FILE * query_three = fopen(THIRD_QUERY_NAME,WRITE);

  if(query_one==NULL || query_two==NULL || query_three==NULL)
    return FILE_ERROR;

  generate_query_one(movs,query_one);
  generate_query_two(movs,query_two);
  generate_query_three(movs,query_three);

  fclose(query_one);
  fclose(query_two);
  fclose(query_three);

  return SUCCESS;
}

int process_files(char * airports_file, char * movements_file){

  FILE * airports = fopen(airports_file,READ);
  FILE * movements = fopen(movements_file, READ);

  if(airports==NULL || movements==NULL)
    return FILE_ERROR;

  movementsADT movs = new_movements();
  if(movs==NULL)
    return ALLOCATION_ERROR;

  int e=process_airports(airports,movs);
  if(e!=SUCCESS)
    return e;
  process_movements(movements,movs);
  if(e!=SUCCESS)
    return e;

  fclose(airports);
  fclose(movements);
  
  if(generate_querys(movs) == FILE_ERROR)
     return FILE_ERROR;  

  free_movements(movs);
  
  return SUCCESS;
}

int main(int argc, char * argv[]){

  if(argc<EXPECTED_ARGUMENTS || argc>EXPECTED_ARGUMENTS){
      argc<EXPECTED_ARGUMENTS ? printf(MORE_ARGUMENTS_EXPECTED) : printf(TOO_MANY_ARGUMENTS);
      return ARGUMENT_ERROR;
  } 

  int e=process_files(argv[FIRST_ARGUMENT],argv[SECOND_ARGUMENT]);
  
  if(e==FILE_ERROR){
      printf(FILE_ERROR_MESSAGE);
  }
  else if(e==ALLOCATION_ERROR){
      printf(ALLOCATION_ERROR_MESSAGE);
  }
      
  return e;

}
