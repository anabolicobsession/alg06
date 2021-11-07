#!/bin/bash

ARG_TEST="test"
TEST_DIR="datapub"
TEST_IN_DIR_PREFIX="pub"
EXEC="cmake-build-debug/hw6" # <- to change
TEST_PREFIX="$TEST_DIR/$TEST_IN_DIR_PREFIX"

ARG_ZIP="zip"
ZIP_TARGET="alg06.tgz" # <- to change
ZIP_FILES=(*".cpp" *".h")

ARG_CLEAN="clean"
CLEAN_FILES=("$ZIP_TARGET")

ARG_HELP="help"
HELP_TEXT="Auxiliary script for testing C++ programs, cleaning working directory and archiving essential files.
Arguments:
    test        run all tests
    test N      run test number N
    test N M    run tests in the range [N,M], where N <= M
    clean       remove all files that are in corresponding variable
    zip         create archive
    help        print help"

COLOR_RESET="\033[0m"
COLOR_RED="\033[1m\033[31m"
COLOR_GREEN="\033[1m\033[32m"

add_leading_zeros() {
  ZEROS=""
  (( N_ZEROS = "$2" - "${#1}" ))
  for i in $(seq 1 "$N_ZEROS"); do
    ZEROS+="0"
  done
  echo "$ZEROS$1"
}

for ARG_IDX in $(seq 1 "$#"); do
  ARG="${*:$ARG_IDX:1}"

  if [[ "$ARG" = "$ARG_TEST" ]]; then
    (( N_TESTS = $(find "$TEST_DIR" -iname "$TEST_IN_DIR_PREFIX*.in" | wc -l) ))
    LAST_ARG_IDX="$#"
    (( PENULTIMATE_ARG_IDX = "$LAST_ARG_IDX" - 1 ))

    if [[ "$ARG_IDX" = "$#" ]]; then
      LOOP_START=1
      LOOP_END="$N_TESTS"
    elif [[ "$ARG_IDX" = "$PENULTIMATE_ARG_IDX" ]]; then
      LOOP_START="${*:$LAST_ARG_IDX:1}"
      LOOP_END="$LOOP_START"
    else
      LOOP_START="${*:PENULTIMATE_ARG_IDX:1}"
      LOOP_END="${*:LAST_ARG_IDX:1}"
    fi

    N_TESTS_PASSED=0
    START_TOTAL_TIME=$(date +%s.%N)
    for i in $(seq "$LOOP_START" "$LOOP_END"); do
      NUM=$(add_leading_zeros "$i" "${#N_TESTS}")
      TEST="$TEST_PREFIX$NUM"

      echo "Running $TEST..."
      START_TIME=$(date +%s.%N)
      RECEIVED="$("./$EXEC" < "$TEST.in")"
      TIME=$(echo "$(date +%s.%N) - $START_TIME" | bc)
      EXPECTED="$(tr -d '\r' < "$TEST.out")"

      echo "$RECEIVED"
      echo "$EXPECTED"

      if [[ "$RECEIVED" = "$EXPECTED" ]]; then
        (( N_TESTS_PASSED++ ))
        echo -ne "$COLOR_GREEN"
        echo -n "Passed"
      else
        echo -ne "$COLOR_RED"
        echo -n "Failed"
      fi
      echo -ne "$COLOR_RESET"
      printf " (%.1fs)\n" "$TIME"

      if [[ "$i" -lt "$LOOP_END" ]]; then
        echo ""
      fi
    done
    TOTAL_TIME=$(echo "$(date +%s.%N) - $START_TOTAL_TIME" | bc)

    (( N_TESTS_LAUNCHED = LOOP_END - LOOP_START + 1 ))
    if [[ "$N_TESTS_LAUNCHED" -gt 1 ]]; then
      echo ""
      echo -n "Tests passed: "
      if [[ "$N_TESTS_PASSED" = "$N_TESTS_LAUNCHED" ]]; then
        echo -ne "$COLOR_GREEN"
      else
        echo -ne "$COLOR_RED"
      fi
      echo -ne "$N_TESTS_PASSED/$N_TESTS_LAUNCHED $COLOR_RESET"
      printf "(%.1fs)\n" "$TOTAL_TIME"
    fi
  elif [[ "$ARG" = "$ARG_ZIP" ]]; then
    tar -czf "$ZIP_TARGET" "${ZIP_FILES[@]}"
  elif [[ "$ARG" = "$ARG_CLEAN" ]]; then
    rm -rfv "${CLEAN_FILES[@]}"
  elif [[ "$ARG" = "$ARG_HELP" ]]; then
    echo -e "$HELP_TEXT"
  fi
done
