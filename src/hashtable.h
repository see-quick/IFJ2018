/**
 * Predmet:  IFJ
 * Projekt:  Implementace prekladace imperativniho jazyka IFJ18
 * Soubor:   hashtable.h
 *
 * Popis: Hlavickovy subor pre HashTable
 *
 *
 * Datum: 22.9.2018 23:56
 *
 * Autori:   Maros Orsak       vedouci
 *           Polishchuk Kateryna     <xpolis03@fit.vutbr.cz>
 *           Igor Ignac
 *           Marek Rohel

*/

#ifndef  _HASHMAP_H_
#define  _HASHMAP_H_

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "string.h"

#define MAX_SIZE_OF_HASH_TABLE 101 // cislo musi byt prvocislo

typedef char* String; // (toto bude kluc)

/** Enum pre vsetky typy  **/
typedef enum Types
{
    NONE     = 500, // neincializaova hodnota
    INTEGER  = 501,
    FLOAT    = 502,
    STRING   = 503,
    BOOLEAN  = 504,  // toto je pre rozsirenie
    FUNCTION = 505
} Types;

/** Union pre, konkretnu hodnotu datatypu napr -> int i = 10 (10 bude ulozene v i) **/
typedef union DataTypes
{
    int i;
    double d;
    tString string;
} dType;

/** stucture for data of indetificator (for local map) **/
typedef struct DataIdentificator { // (toto bude obsah pre localnu mapu)
    Types type;                     // typ identifikatora
    dType value;                    // hodnota identifikatora
    bool defined;                   // zda bola premmena definovana (ale taktiez to zistime tak ak bude nebude value
} tDataIDF;

/** stucture for data of function (for global map) **/
typedef struct DataFunction {
    Types type;                     // typ parametrov
    Types returnType;               // navratova hodnota
    int positionOfParameter;        // pozicia parametrov  //TODO: toto bude fungovat iba pre 0 a 1 argument, implementovat to ako list(v GTS bude typ a hodnota daneho parametru), LTS(key -> nazov premmenej, value -> typ, hodnota(v pripade arguemntov NONE(prazdna)))
    bool defined;                   // zda bola funckcia definovana
    tString functionParametersNames;//
} tDataFunction;

/*Datová položka TRP s explicitně řetězenými synonymy pre Globalnu mapu*/
typedef struct GlobalMapItem{
    String key;				    // kluc
    tDataFunction globalData;	// obsah pre globalnu mapu
    struct GlobalMapItem* ptrnext;	// ukazatel na dalsi synonym
} GlobalMapItem;

/*Datová položka TRP s explicitně řetězenými synonymy pre Localnu mapu*/
typedef struct LocalMapItem{
    String key;				    // kluc
    tDataIDF localData;			// obsah pre localnu mapu
    struct LocalMapItem* ptrnext;	// ukazatel na dalsi synonym
} LocalMapItem;

typedef LocalMapItem* LocalMap[MAX_SIZE_OF_HASH_TABLE];
typedef GlobalMapItem* GlobalMap[MAX_SIZE_OF_HASH_TABLE];

struct LocalMapItem* undefined_pointer_local;
struct GlobalMapItem* undefined_pointer_global;

// popis k proceduram / funckiam -> programova dokumentacia ak pouzivate Clion (staci ku funckii a pouzit CTLR + q)
/** VSEOBECNE FUNCKIE **/
int hashcode ( String key );

/** FUNKCIE S LOCALNOU MAPOU **/

void local_map_init ( LocalMap* ptrMap);
LocalMapItem* local_map_get_item ( LocalMap* ptrMap, String key );
tDataIDF* local_map_get_pointer_to_value ( LocalMap* ptrMap, String key );
tDataIDF local_map_get_value(LocalMap* ptrMap, String key);
String* local_map_get_pointer_to_key ( LocalMap* ptrMap, String key );
String local_map_get_key(LocalMap* ptrMap, String key);
bool local_map_contain(LocalMap* ptrMap, String key);
void local_map_remove ( LocalMap* ptrMap, String key );
void local_map_free ( LocalMap* ptrMap );
void local_map_print( LocalMap* ptrMap );
void local_map_put ( LocalMap* ptrMap, String key, tDataIDF data );

/** FUNKCIE S GLOBALNOU MAPOU **/
void global_map_init ( GlobalMap* ptrMap);
GlobalMapItem* global_map_get_item ( GlobalMap* ptrMap, String key );
tDataFunction* global_map_get_pointer_to_value ( GlobalMap* ptrMap, String key );
tDataFunction global_map_get_value(GlobalMap* ptrMap, String key);
String* global_map_get_pointer_to_key ( GlobalMap* ptrMap, String key );
String global_map_get_key(GlobalMap* ptrMap, String key);
bool global_map_contain(GlobalMap* ptrMap, String key);
void global_map_remove ( GlobalMap* ptrMap, String key );
void global_map_free ( GlobalMap* ptrMap );
void global_map_print( GlobalMap* ptrMap );
void global_map_put ( GlobalMap* ptrMap, String key, tDataFunction data );

#define  _HASHMAP_H_

#endif //IFJPROJ_HASHTABLE_H
