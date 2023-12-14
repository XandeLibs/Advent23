#compiles the day's source code, tests it and if successiful, runs with the input file

echo -n "Choose day: "
read day

make build day=$day

./run "Day $day/tests.txt" > test_result.txt

if ! [ diff output test_result.txt ]; then
  ./run "Day $day/input.txt" > result.txt


