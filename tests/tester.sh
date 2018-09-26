#!/bin/bash

#
 # Predmet:  IFJ
 # Projekt:  Implementace prekladace imperativniho jazyka IFJ18
 # Soubor:   tester.bash
 #
 # Popis:  Skript pre automaticke testovanie lexikalnej analyzy pomocou vstupnych dat
 #
 # Datum: 23.9.2018 20:20
 #
 # Autori:   Maros Orsak       vedouci
 #           Polishchuk Kateryna     <xpolis03@fit.vutbr.cz>
 #           Igor Ignac
 #           Marek Rohel
#

usage() {
  echo  "Automaticke testy pre jazyk ifj18. Spustenie testov pomocou:
        bash tester.bash [program] [test directory]
        program - nazov prelozeneho programu(ifj)
        test directory - nazov zlozky s testami"
  exit 1
}

#nastavenie farieb pre farebny vystup testov
red=`tput setaf 1`
green=`tput setaf 2`
reset=`tput sgr0`

#uplna cesta k testom
testpath="$(realpath .)/$2"

if [[ "$1" == "--help" && -z $2 ]]; then
  usage

elif [[ ! -z "$1" && ! -z "$2" ]]; then
  prog="$(realpath .| rev | cut -c 6- | rev)src/$1" #cesta k prelozenemu projektu
  lines=$(find $testpath -type d | wc -l) #pocet suborov s testami
  ((lines--))
  testsucc=0  #counter pre uspesne testy
  testfail=0  #counter pre neuspesne testy

  for ((n=0; n<$lines;n++)); do #loop cez testovacie adresare
    line=$((n+1))
    currentdir=$(find $testpath -maxdepth 1 -mindepth 1 -type d -printf '%f\n' | awk NR==$line) #nazov adresaru s testami
    filescounter=$(find $testpath/$currentdir -type f | wc -l) #pocet testov v adresari

    for ((m=0; m<$filescounter;m++)); do #loop cez konkretne testy v adresaroch
      linef=$((m+1))
      testfile=$(find $testpath/$currentdir -type f -printf "%f\n" | awk NR==$linef)  #nazov testovacieho suboru
      testtype=${testfile:0:6} #extrakt nazvu testu

      if [ "$testtype" == "error1" ]; then  #vsetky testy obsahujuce chybu maju na zaciatku error. Error1 znaci ocakavanu navratovu hodnotu 1(LEXerror)
        $prog < $testpath/$currentdir/$testfile > /dev/null 2>&1  #presmerovanie vystupu lexeru
        retval=$(echo $?)
        if [ "$retval" -eq "1" ]; then
          ((testsucc++))
          echo "${green}[TEST PASSED]${reset}"
        else
          ((testfail++))
          echo "${red}[TEST FAILED]${reset} : $currentdir/$testfile"
          echo "  expected return value = 1. Returned value = $retval"
        fi
      elif [ "$testtype" == "error2" ]; then #testy oznacene error2 --> SyntaxError
        $prog < $testpath/$currentdir/$testfile > /dev/null 2>&1
        retval=$(echo $?)
        if [ "$retval" -eq "2" ]; then
          ((testsucc++))
          echo "${green}[TEST PASSED]${reset}"
        else
          ((testfail++))
          echo "${red}[TEST FAILED]${reset} : $currentdir/$testfile"
          echo "  expected return value = 2. Returned value = $retval"
        fi
      else
        $prog < $testpath/$currentdir/$testfile > /dev/null 2>&1
        retval=$(echo $?)
        if [ "$retval" -eq "0" ]; then
          ((testsucc++))
          echo "${green}[TEST PASSED]${reset}"
        else
          ((testfail++))
          echo "${red}[TEST FAILED]${reset} : $currentdir/$testfile"
          echo "  expected return value = 0. Returned value = $retval"
        fi
      fi
    done
  done

  echo "TESTED $((testfail+testsucc)) tests, PASSED: $testsucc, FAILED: $testfail"
  echo "..."
  echo "Trying to run valgrind to check for correct allocations"

  #Navratova hodnota valgrindu nastavena na 66 aby sa dal detekovat problem pri alokacii
  valgrind --leak-check=full --show-reachable=yes --error-exitcode=66 $prog < testData/01basic_programs/example_prog01.ifj18 > /dev/null 2>&1

  if [ $? -eq "66" ]; then
    echo "${red}[TEST FAILED]${reset} : Memory wasn't deallocated successfully"
    echo "  Run valgrind for further details"
  else
    echo "${green}[TEST PASSED]${reset} : All allocated memory was deallocated"
  fi
fi
