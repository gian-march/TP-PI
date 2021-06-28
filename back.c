#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OACI_LENGTH 5
#define DESCRIPTION_LENGTH 71
#define PROVINCE_LENGTH 70

#define WEEK_DAYS 7
#define DAY_PERIODS 2

#define TAKEOFF "Despegue"
#define LANDING "Aterrizaje"

#define TRUE 1
#define FALSE 0

#define DAY 0
#define NIGHT 1

//Formato de la hora HHMM
#define DAYTIME_PERIOD_START 601 
#define DAYTIME_PERIOD_END 1800

#define ERROR 1
#define SUCCESS 0 

struct airport{
    char oaci[OACI_LENGTH];
    char description[DESCRIPTION_LENGTH];
    char province[PROVINCE_LENGTH];
    unsigned int movs_qty;
    struct airport * next;
};

struct province{
    char province[PROVINCE_LENGTH];
    unsigned int movs_qty;
    struct province * next;
};

struct movementsCDT{
    struct airport * airports;
    struct airport * airport_iterator;
    struct province * provinces;
    struct province * province_iterator;
    unsigned int days[WEEK_DAYS][DAY_PERIODS];
    unsigned int total_movs;  
};

typedef struct airport * airportP;
typedef struct province * provinceP;

struct movementsCDT * new_movements(){
    struct movementsCDT * movements=calloc(1,sizeof(*movements));
    return movements;
}

static int has_next(void * iterator){
    return iterator != NULL;
}

int has_next_airport(struct movementsCDT * movements){
	return has_next(movements->airport_iterator);
}

int has_next_province(struct movementsCDT * movements){
	return has_next(movements->province_iterator);
}


void next_airport(struct movementsCDT * movements){
    movements->airport_iterator=movements->airport_iterator->next;
}

void next_province(struct movementsCDT * movements){
    movements->province_iterator=movements->province_iterator->next;
}

void get_airport_info(struct movementsCDT * movements, char * oaci, char * description, unsigned int * movs_qty){
    strcpy(oaci, movements->airport_iterator->oaci);
    strcpy(description, movements->airport_iterator->description);
    *movs_qty=movements->airport_iterator->movs_qty;
}

void get_province_info(struct movementsCDT * movements, char * province, unsigned int * percentage){
    strcpy(province, movements->province_iterator->province);
    *percentage=movements->province_iterator->movs_qty;
}

void get_days_info(struct movementsCDT * movements,int day, int * daytime, int * nighttime){
    *daytime = movements->days[day][0];
    *nighttime= movements->days[day][1];
}

static airportP insert_airport_rec(airportP current, airportP to_insert){
    if(current == NULL ) // Llegue al final de la lista
        return to_insert;

    int cmp = strcmp(current->oaci,to_insert->oaci);
    if(cmp>0){
        to_insert->next=current;
        return to_insert;
    }
    else if(cmp == 0){
        free(to_insert);
        return current;
    }
    current->next=insert_airport_rec(current->next,to_insert);
    return current;
}

static airportP new_airport(char * oaci, char * province, char * description){
    airportP airport=malloc(sizeof(*airport));
    strcpy(airport->oaci,oaci);
    strcpy(airport->province,province);
    strcpy(airport->description,description);
    airport->movs_qty=0;
    airport->next=NULL;
    return airport;
}

int insert_airport(struct movementsCDT * movements, char * oaci, char * description, char * province){
    
    airportP airport=new_airport(oaci,province,description);
    if(airport==NULL)
        return ERROR;
    movements->airports=insert_airport_rec(movements->airports,airport);
    return SUCCESS;
}

static airportP swap(airportP incremented, airportP current){
    current->next=incremented->next;
    incremented->next=current;
    return incremented;
}

/* Incrementa en 1 los movimientos del aeropuerto correspondiente
   Retorna un puntero al primer aeropuerto de la lista
*/
static airportP insert_airport_movement(airportP current, char * oaci){
    if(current==NULL) // Llegue al final de la lista y no encontre el oaci
        return current;

    int c=strcmp(oaci,current->oaci);
    if(c==0){                   // Lo encontro!
        current->movs_qty+=1;
        return current;
    }   
    
    current->next=insert_airport_movement(current->next,oaci);
    if(current->next->movs_qty > current->movs_qty) // Ordena por movimientos
        return swap(current->next,current);
    else if(current->next->movs_qty == current->movs_qty){ 
        int c=strcmp(current->next->oaci,current->oaci);   // Ordena por orden alfabetico
        if(c<0)
            return swap(current->next,current);        
    } 
    return current;
}

static provinceP new_province(char * province_name, unsigned int movs_qty){ 
    provinceP province=malloc(sizeof(*province));
    strcpy(province->province,province_name);
    province->movs_qty=movs_qty; 
    province->next=NULL;
    return province;
}


static provinceP swap_provinces(provinceP incremented, provinceP current){
    current->next=incremented->next;
    incremented->next=current;
    return incremented;
}

static provinceP insert_province_movement(struct movementsCDT * movements, provinceP current, char * province_name, unsigned int movs_qty){ //
    if(current==NULL){                                  // La provincia no esta en la lista
        return new_province(province_name,movs_qty); 
    }

    int c=strcmp(province_name,current->province);

    if(c==0){               // La encontre!
        current->movs_qty+=movs_qty;
        return current;
    }   

    current->next=insert_province_movement(movements,current->next,province_name,movs_qty); // Sigue buscando
    if(current->next==NULL)
        return NULL;
    else if(current->next->movs_qty>current->movs_qty){      // Ordena por movimientos
        return swap_provinces(current->next,current);
    }
    else if(current->next->movs_qty==current->movs_qty){    // Ordena por orden alfabetico
        int c=strcmp(current->next->province,current->province);
        if(c>0)
            return swap_provinces(current->next,current);
    }
    return current;
}

static unsigned int to_percentage(unsigned int movs, unsigned int total){
    return (int)((((float)movs)/(float)total)*100);
}

provinceP movs_qty_to_percentage(provinceP current, unsigned int total){
    if(current==NULL)
        return current;
    
    current->movs_qty=to_percentage(current->movs_qty,total);
    current->next=movs_qty_to_percentage(current->next,total);
    if(current->next!=NULL){
        if(current->movs_qty == current->next->movs_qty){
            int c=strcmp(current->province,current->next->province);
            if(c>0)
                return swap_provinces(current->next,current);
        }
    }

    return current;
}

int generate_provinces_movements(struct movementsCDT * movements){

    movements->airport_iterator=movements->airports;
    int keep_going=TRUE;

    while(has_next_airport(movements) && keep_going){
        unsigned int airport_movs=movements->airport_iterator->movs_qty;
        if(airport_movs==0)
            keep_going=FALSE;
        else{
            movements->provinces=insert_province_movement(movements,movements->provinces,movements->airport_iterator->province,movements->airport_iterator->movs_qty);
            if(movements->provinces==NULL)
                return ERROR;
            next_airport(movements);     
        }
    }
    movements->provinces=movs_qty_to_percentage(movements->provinces, movements->total_movs);

    movements->province_iterator=movements->provinces;
    movements->airport_iterator=movements->airports;
    return SUCCESS;
}

static int get_day_period(char * time){
    int h;
    int m;
    int total;
    sscanf(time,"%d:%d",&h,&m);
    total = h*100+m;
    if(total>=DAYTIME_PERIOD_START && total<=DAYTIME_PERIOD_END){
        return DAY;
    }
    return NIGHT;
}

static int get_week_day(const char * date){
    int day;
    int month;
    int year;
    sscanf(date,"%d/%d/%d",&day,&month,&year);

  	//Formula de Stack overflow
    int week_day=(day += month < 3 ? year-- : year - 2, 23*month/9 + day + 4 + year/4- year/100 + year/400)%7;

    return week_day;
}



void add_movement(struct movementsCDT * movements, char * date, char * time, char * oaci_o, char * oaci_d, char * flight_type){
    
    movements->total_movs+=1;
    int c=strcmp(flight_type,LANDING);
    if(c==0)
        movements->airports=insert_airport_movement(movements->airports,oaci_d);
    else{
        movements->airports=insert_airport_movement(movements->airports,oaci_o);
    } 
    

    int day = get_week_day(date);
    int period = get_day_period(time);
    movements->days[day][period]++;
    
}

static void free_provinces(provinceP provinces){
    provinceP aux = provinces;
    while(aux!=NULL){
        aux=provinces->next;
        free(provinces);
        provinces=aux;
    }
}
static void free_airports(airportP airports){
    airportP aux = airports;
    while(aux!=NULL){
        aux=airports->next;
        free(airports);
        airports=aux;
    }
}

void free_movements(struct movementsCDT * movements){
    free_provinces(movements->provinces);
    free_airports(movements->airports);
    free(movements);
}
