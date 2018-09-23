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

#define MAX_SIZE_OF_HASH_TABLE 101 // cislo musi byt prvocislo

typedef char* String; // (toto bude kluc)
typedef int Integer;  // (toto bude obsah)

/*Datová položka TRP s explicitně řetězenými synonymy*/
typedef struct MapItem{
    String key;				    // kluc
    Integer data;				// obsah
    struct MapItem* ptrnext;	// ukazatel na dalsi synonym
} MapItem;

typedef MapItem* Map[MAX_SIZE_OF_HASH_TABLE];
struct MapItem* undefined_pointer;

// popis k proceduram / funckiam -> programova dokumentacia ak pouzivate Clion (staci ku funckii a pouzit CTLR + q)
int hashcode ( String key );
void map_init ( Map* ptrMap);
void map_put ( Map* ptrMap, String key, Integer data );
MapItem* map_get_item ( Map* ptrMap, String key );
Integer* map_get_pointer_to_value ( Map* ptrMap, String key );
Integer map_get_value(Map* ptrMap, String key);
String* map_get_pointer_to_key ( Map* ptrMap, String key );
String map_get_key(Map* ptrMap, String key);
bool map_contain(Map* ptrMap, String key);
void map_remove ( Map* ptrMap, String key );
void map_free ( Map* ptrMap );
void map_print( Map* ptrMap );

#define  _HASHMAP_H_

#endif //IFJPROJ_HASHTABLE_H
