# DeskFinder
This is a tool to make the best Desk arrangement.
# Usage
First, make people.json

people.json: 
```
{
 "likes": array(int),
 "weights": array(double),
 "front": array(bool)
}
```
Next, compile the .cpp files.

```
~$ g++ *.cpp -o deskgen -std=c++11
```
And initialize desk arrangement data file.
```
~$ ./deskgen init -i ./people.json -o ./desk.json
```
Finally, run make.sh
```
~$ chmod 777 ./make.sh
~$ ./make.sh <loop number>
```
# Commands
```
genpeople -o <output path> -l <length>
```
Generates random people by length and save to the output path.
```
mine -p <people> -d <desk>
```
Search for better desk.  
When there found a better desk, it will be saved and printed.
```
init -p <people> -o <desk>
```
Generates ten desks randomly and save to the output path.
```
evaluate -p <people> -d <desk>
```
Evaluates desk arrangement using people file.
```
print -p <people> -o <desk> -n <namelist>
```
Print desk figure with people's name list.