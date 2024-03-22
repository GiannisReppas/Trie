import csv
import random
import sys

def string_generator( start, end):
	string_size = random.randint( start, end)

	toReturn = ""
	toAppend = ""
	for i in range (1,string_size+1):
		while True:
			c = random.randint(48,126)
			if c != 96:
				break
		toReturn = toReturn + str(chr(c))

	return toReturn

if len((sys.argv)) != 2:
	print("Usage: python create_random.py file_lines\n")
	exit(1)

filesize = int(sys.argv[1])

word = ""
translation = ""

f = open('./random_input.csv', 'w')

for i in range(1,filesize+1):
	word = string_generator(5,15)
	translation = string_generator(90,110)
	f.write(word + "," + translation + "\n")

f.close()
