/**
 * Predmet:  IFJ
 * Projekt:  Implementace prekladace imperativniho jazyka IFJ18
 * Soubor:   hashtabletest.c
 *
 * Popis: Testovanie hashovacej tabulky
 *
 * Datum: 22.9.2018 23:56
 *
 * Autori:   Maros Orsak       vedouci
 *           Polishchuk Kateryna     <xpolis03@fit.vutbr.cz>
 *           Igor Ignac
 *           Marek Rohel

*/

#include "../src/hashtable.h"
#include <stdio.h>

/**
 * Procedura, ktora sluzi na testovanie funckionality interface s mapou
 */
static void hashtableTest(){
    Map* table = NULL;
    map_init(table);
    table = (Map*) malloc ( sizeof(Map) );
    for ( int i=0; i<MAX_SIZE_OF_HASH_TABLE; (*table)[i++] = undefined_pointer );

    printf ("---------------------------\n");
    printf ("\nLet's set HTSIZE to 19 ( must be prvocislo )\n");
    map_print(table);

    map_put(table, "IntegerValue10", 10);
    map_put(table, "IntegerValue20", 20);
    map_put(table, "IntegerValue30", 30);
    map_put(table, "IntegerValue40", 40);

    map_print(table);

    MapItem *mapItem = map_get_item(table, "IntegerValue20");
    printf("Pointer on Map value -> %p \n", map_get_pointer_to_value(table, "IntegerValue20")); // pointer na item
    printf("Pointer on Map Key -> %p \n", map_get_pointer_to_key(table, "IntegerValue20")); // pointer na item
    printf("Map key -> %s \n", map_get_key(table, "IntegerValue30")); // takto sa ziska danu stringovu hodnotu z kluca
    printf("Map value -> %d \n", map_get_value(table, "IntegerValue30")); // takto sa ziska honodta z kluca
    printf("Key -> %s , Value -> %d \n", mapItem->key, mapItem->data);
    printf("Vysledna hodnota je -> %d", map_contain(table, "IntegerVal20")); // FALSE -> 0
    printf("Vysledna hodnota je -> %d", map_contain(table, "IntegerValue20")); // TRUE -> 1
    map_remove(table, "IntegerValue20");
    map_print(table);
    map_free(table);
    map_print(table);
}

