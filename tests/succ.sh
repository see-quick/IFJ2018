#!/bin/bash

#
 # Predmet:  IFJ
 # Projekt:  Implementace prekladace imperativniho jazyka IFJ18
 # Soubor:   succ.sh
 #
 # Popis:  Skript pre vypis uspesnosti jednotlivych testov a testov celkovo
 #
 # Datum: 30.10.2018 12:00
 #
 # Autori:   Maros Orsak       vedouci
 #           Polishchuk Kateryna     <xpolis03@fit.vutbr.cz>
 #           Igor Ignac
 #           Marek Rohel
#

usage() {
  echo  "Run program simply without any arguments: bash succ.sh"
  exit 1
}


red=`tput setaf 1`
green=`tput setaf 2`
reset=`tput sgr0`

#IMPORTANT  ---> DESTROYS MALLOC AND MEMORY ERRORS
#           ---> ONLY FOR NICE FORMAT OF OUTPUT
#           ---> RUN TESTS TO SEE MEMORY ERRORS!!
export MALLOC_CHECK_=0

if [[ "$1" == "-h" || "$1" == "--help" && -z $2 ]]; then
  usage
elif [[ ! -z $1 ]]; then
  usage
fi

#EXPRESSIONS
OUTPUT=$(bash tester.sh ifj expressionsData 2>/dev/null | tail -n 5)
TESTNUMBER=$(echo $OUTPUT | cut -c8-10)
SUCCTESTS=$(echo $OUTPUT | cut -c26-27)
FAILEDTESTS=$((TESTNUMBER-SUCCTESTS))

# Overall tests + succ and failed ones
TOTALTESTS=$TESTNUMBER
TOTALFAILED=$FAILEDTESTS
TOTALSUCC=$SUCCTESTS

#tests succession
total=$(echo " scale=2;
var1 = $SUCCTESTS * 100;
var1 = var1 / $TESTNUMBER;
var1 " \
| bc)

echo "Test data for expressions: TESTED $TESTNUMBER tests, PASSED: $SUCCTESTS, FAILED: $FAILEDTESTS Totally: ${green}$total%${reset}"

#SEMANTICDATA
OUTPUT=$(bash tester.sh ifj semanticData 2>/dev/null | tail -n 5)
TESTNUMBER=$(echo $OUTPUT | cut -c8-10)
SUCCTESTS=$(echo $OUTPUT | cut -c26-27)
FAILEDTESTS=$((TESTNUMBER-SUCCTESTS))

TOTALTESTS=$((TESTNUMBER+TOTALTESTS))
TOTALFAILED=$((FAILEDTESTS+TOTALFAILED))
TOTALSUCC=$((SUCCTESTS+TOTALSUCC))

total=$(echo " scale=2;
var1 = $SUCCTESTS * 100;
var1 = var1 / $TESTNUMBER;
var1 " \
| bc)

echo "Test data for semanticData: TESTED $TESTNUMBER tests, PASSED: $SUCCTESTS, FAILED: $FAILEDTESTS Totally: ${green}$total%${reset}"

#SYNTAXDATA
OUTPUT=$(bash tester.sh ifj syntaxData 2>/dev/null | tail -n 5)
TESTNUMBER=$(echo $OUTPUT | cut -c8-10)
SUCCTESTS=$(echo $OUTPUT | cut -c26-27)
FAILEDTESTS=$((TESTNUMBER-SUCCTESTS))

TOTALTESTS=$((TESTNUMBER+TOTALTESTS))
TOTALFAILED=$((FAILEDTESTS+TOTALFAILED))
TOTALSUCC=$((SUCCTESTS+TOTALSUCC))

total=$(echo " scale=2;
var1 = $SUCCTESTS * 100;
var1 = var1 / $TESTNUMBER;
var1 " \
| bc)

echo "Test data for syntaxData: TESTED $TESTNUMBER tests, PASSED: $SUCCTESTS, FAILED: $FAILEDTESTS Totally: ${green}$total%${reset}"

#TESTDATA
OUTPUT=$(bash tester.sh ifj testData 2>/dev/null | tail -n 5)
TESTNUMBER=$(echo $OUTPUT | cut -c8-10)
SUCCTESTS=$(echo $OUTPUT | cut -c26-27)
FAILEDTESTS=$((TESTNUMBER-SUCCTESTS))

TOTALTESTS=$((TESTNUMBER+TOTALTESTS))
TOTALFAILED=$((FAILEDTESTS+TOTALFAILED))
TOTALSUCC=$((SUCCTESTS+TOTALSUCC))

total=$(echo " scale=2;
var1 = $SUCCTESTS * 100;
var1 = var1 / $TESTNUMBER;
var1 " \
| bc)

echo "Test data for testData: TESTED $TESTNUMBER tests, PASSED: $SUCCTESTS, FAILED: $FAILEDTESTS Totally: ${green}$total%${reset}"

#genDATA
#OUTPUT=$(sudo bash gen_tester.sh -g 2>/dev/null | tail -n 1)
#TESTNUMBER=$(echo $OUTPUT | cut -c8-10)
#SUCCTESTS=$(echo $OUTPUT | cut -c26-27)
#FAILEDTESTS=$((TESTNUMBER-SUCCTESTS))

#TOTALTESTS=$((TESTNUMBER+TOTALTESTS))
#TOTALFAILED=$((FAILEDTESTS+TOTALFAILED))
#TOTALSUCC=$((SUCCTESTS+TOTALSUCC))

#total=$(echo " scale=2;
#var1 = $SUCCTESTS * 100;
#var1 = var1 / $TESTNUMBER;
#var1 " \
#| bc)

#echo "Test data for genData: TESTED $TESTNUMBER tests, PASSED: $SUCCTESTS, FAILED: $FAILEDTESTS Totally: ${green}$total%${reset}"

#TOTAL NUMBER OF TESTS + STATISTICS
total=$(echo " scale=2;
var1 = $TOTALSUCC * 100;
var1 = var1 / $TOTALTESTS;
var1 " \
| bc)


echo "..."
echo "Totally tests runned: $TOTALTESTS, PASSED: ${green}$TOTALSUCC${reset}, FAILED: ${red}$TOTALFAILED${reset} Totally: ${green}$total%${reset}"

#code gen
echo ""
echo "Testing code generation, this might take few seconds..."


OUTPUT=$(sudo bash gen_tester.sh -n 2>/dev/null | tail -n 1)
TESTNUMBER=$(echo $OUTPUT | cut -c8-10)
SUCCTESTS=$(echo $OUTPUT | cut -c26-29)

#test if we have number
#re='^[0-9]+$'
#if ! [[ $SUCCTESTS =~ $re ]]; then
#  SUCCTESTS=0
#fi
FAILEDTESTS=$((TESTNUMBER-SUCCTESTS))

TOTALTESTSGEN=$((TESTNUMBER+TOTALTESTSGEN))
TOTALFAILEDGEN=$((FAILEDTESTS+TOTALFAILEDGEN))
TOTALSUCCGEN=$((SUCCTESTS+TOTALSUCCGEN))

total=$(echo " scale=2;
var1 = $TOTALSUCCGEN * 100;
var1 = var1 / $TOTALTESTSGEN;
var1 " \
| bc)

echo "..."
echo "Totally tests runned: $TOTALTESTSGEN, PASSED: ${green}$TOTALSUCCGEN${reset}, FAILED: ${red}$TOTALFAILEDGEN${reset} Totally: ${green}$total%${reset}"
