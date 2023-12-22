#compiles the day's source code, tests it and if successiful, runs with the input file

echo -n "Choose day: "
read day

make build day=$day

echo "Running test"
./run "Day $day/test.txt" "test_result.txt"

echo "Testing difference"
DIFF=$(diff --strip-trailing-cr "Day $day/test_answer.txt" "test_result.txt")
echo $DIFF

if ! [ "$DIFF" != "" ]; then
  echo "Running input"
  ./run "Day $day/input.txt" "result.txt"
  cat "result.txt"
fi

