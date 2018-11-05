#!/bin/bash

#
 # Predmet:  IFJ
 # Projekt:  Implementace prekladace imperativniho jazyka IFJ18
 # Soubor:   gen_tester.sh
 #
 # Popis:  Skript pre vypis testovanie vygenerovaneho kodu v IFJcode18 interpretom
 #
 # Datum: 1.11.2018 19:06
 #
 # Autori:   Maros Orsak       vedouci
 #           Polishchuk Kateryna     <xpolis03@fit.vutbr.cz>
 #           Igor Ignac
 #           Marek Rohel
#

usage() {
  echo  "Run program simply without any arguments: bash gen_tester.sh"
  echo  "For proper functionality you have to be logged as sudo or root !!!"
  echo  "Run program with bash gen_tester.sh -s for silent run -> interpret errors will be displayed"
  echo  "Run program with bash gen_tester.sh -n and no output files will be generated"
  echo  "Run program with bash gen_tester.sh -e to run only test for generation code from prece.c"
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
fi

if [[ "$1" == "-s" ]];then
  silent="-s"
fi

if [[ "$1" == "-n" ]];then
  filegen="-n"
fi

echo "Testing return values from interpret..."
echo "Running all test dirs from ../tests with expected return value of 0..."
echo ""

touch test.code #temporary file for generated output of ./ifj

testsucc=0  #counter for succ tests
testfail=0  #counter for failed tests

function testRunner() {

  testpath=$1

  lines=$(find $testpath -type d | wc -l) #number of files in dir
  ((lines--))

  for ((n=0; n<$lines;n++)); do #loop over test dir
    line=$((n+1))
    currentdir=$(find $testpath -maxdepth 1 -mindepth 1 -type d -printf '%f\n' | awk NR==$line) #name of dir with tests
    filescounter=$(find $testpath/$currentdir -type f | wc -l) #number of tests in dir

    for ((m=0; m<$filescounter;m++)); do #loop over tests in dir
      linef=$((m+1))
      testfile=$(find $testpath/$currentdir -type f -printf "%f\n" | awk NR==$linef)  #name of test dir
      testtype=${testfile:0:5} #extrakt nazvu testu

      if [ "$testtype" != "error" ]; then  #all tests without name error should return value of 0
        timeout 3 ../src/ifj < $testpath/$currentdir/$testfile 2>/dev/null > test.code
        retvalIFJ=$(echo $?)

        testname="$testfile.out"  #creating output file by testname
        if [[ "$filegen" == "-n" ]]; then
          :
        else
           touch codeGenData/$testname
        fi

        if [[ "$silent" == "-s" ]]; then
          timeout 3 sudo ./ic18int test.code > /dev/null 2>&1
        else
          timeout 3 sudo ./ic18int test.code
        fi
        retval=$(echo $?)
        if [ "$retval" -eq "124" ]; then
            echo "${red}[TEST FAILED]${reset} Program got stucked killed process after 3 sec"
        fi

        if [ "$retvalIFJ" -eq "124" ]; then
          echo "${red}[TEST FAILED]${reset} Program got stucked killed process after 3 sec"
        fi

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
}

#TESTS FOR code_gen in prece.c for @Maros
if [[ "$1" == "-e" ]];then
  testDIR="preceGen"
  filegen="-n"
  testRunner "$testDIR"
else
  #TESTS FOR testData
  testDIR="testData"
  testRunner "$testDIR"

  #TESTS FOR syntaxData
  testDIR="syntaxData"
  testRunner "$testDIR"

  #TESTS FOR semanticData
  testDIR="semanticData"
  testRunner "$testDIR"

  #TESTS FOR expressionsData
  testDIR="expressionsData"
  testRunner "$testDIR"

fi



#tests succession
tests=$((testfail+testsucc))
total=$(echo " scale=2;
var1 = $testsucc * 100;
var1 = var1 / $tests;
var1 " \
| bc)

echo "TESTED $tests tests, PASSED: $testsucc, FAILED: $testfail Totally: ${green}$total%${reset}"
