/*
 * back.h
 * Contrato del TAD.
 */

#ifndef __BACK__
#define __BACK__

/* 
 * Tipo de dato que define el TAD.
 * Se lo define como puntero a "cierta estructura"
 * para ocultar la real representacion de la misma
 */
typedef struct movementsCDT * movementsADT;


/* Funcion:  		new_movements
 * Uso:  			movementsADT movements = new_movements();
 * -------------------------------------------------------------------
 * Descripción: 	Inicializa el TAD
 */
movementsADT new_movements();



/* Funcion:  		insert_airport
 * Ejemplo de uso: 	insert_airport( movements , "SAAA" , "SAN ANTONIO DE ARECO", "BUENOS AIRES");
 * -------------------------------------------------------------------
 * Descripción: 	Inserta un elemento en la lista aeropuertos 
 *  				ordenandolos por cantidad de movimientos descendentemente
 *	  				luego por orden alfabetico.	
 *                  Retorna 1 su hubo errores de alocacion de memoria, 0 si se ejecuto correctamente   
 */
int insert_airport(movementsADT movements, char * oaci, char * description, char * province);


/* Funcion:  		generate_provinces_movements
 * Ejemplo de uso: 	generate_provinces_movements( movements )
 * -------------------------------------------------------------------
 * Descripción: 	Genera lista de provincias ordenandolas
 *                  por cantidad de movimientos descendentemente
 *	  				luego por orden alfabetico.	
 * 					Esta lista se genera a partir de los datos obtenidos 
 *					anteriormente, ya almacenados en la lista de aeropuertos.
 *                  Retorna 1 su hubo errores de alocacion de memoria, 0 si se ejecuto correctamente               
 */
int generate_provinces_movements(movementsADT movements);



/* Funcion:  		add_movement
 * Ejemplo de uso: 	add_movement( movements , "DD/MM/AAAA" , "HH:MM", "SAAA", "SABE", "DESPEGUE");
 * -------------------------------------------------------------------
 * Descripción: 	Incrementa un movimiento al aeropuerto correspondiente
 * 					Contabilizando únicamente despegues que tengan al
 * 					aeropuerto como origen y aterrizajes que tengan al aeropuerto como destino. 
 */
void add_movement(movementsADT movements, char * date, char * time, char * oaci_o, char * oaci_d, char * flight_type);



/* 
 * Funciones  de consulta: para obtener los datos necesarios para armar las query´s
 * Estas tres funciones reciben el TAD y almacenan en los parametros de entrada/salida
 * la informacion necesaria para generar las estadisticas de interes.
 */



void  get_airport_info(movementsADT movements, char * oaci, char * description, unsigned int * movs_qty);
void  get_province_info(movementsADT movements, char * province, unsigned int * percentage);
void get_days_info(struct movementsCDT * movements,int day, int * daytime, int * nighttime);


/*
 * Funciones para poder iterar sobre las listas
 */


// Retorna 1 si se puede iterar a un siguiente aeropuerto, caso contrario retorna 0
int has_next_airport(movementsADT movements);

// Posiciona la lista de aeropuertos en el siguiente aeropuerto, su ejecucion solo es valida si has_next_airport retorna 1
void next_airport(movementsADT movements);


// Retorna 1 si se puede iterar a una siguiente provincia
int has_next_province(movementsADT movements);

// Posiciona la lista de provincias en la siguiente provincia, su ejecucion solo es valida si has_next_province retorna 1
void next_province(movementsADT movements);



/* Funcion: 		free_movements
 * Ejemplo de uso: 	free_movements( movements );
 * -------------------------------------------------------------------
 * Descripción: 	Libera todos los recursos de memoria utilizados por el TAD
 */
void free_movements(movementsADT movements);




#endif // __BACK__
