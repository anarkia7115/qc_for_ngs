#!/usr/bin/env python

def parseQual(s, l, u):
	for pos in range(0, len(s)):
		ic = ord(s[pos]) - 33
		#if ic < 0:
			#print s[pos]

		# if l and u are not long enough, append
		if pos >= len(l):
			l.append(ic)
			u.append(ic)
		# get lower and upper bound
		if ic < l[pos]:
			l[pos] = ic
		if ic > u[pos]:
			u[pos] = ic

	return l, u

def main():
	f = open("./B201_S56_L001_R1_001.clean_val_1.fq")

	# 0..3
	lineNum = 0

	# init bound
	al = []
	au = []

	for l in f:
		l = l.rstrip()
		if lineNum ==3:
			lineNum = 0
			# parse quality line
			al, au = parseQual(l, al, au)
			#print l
			#print len(l)
		else:
			lineNum += 1

	print al
	print au
		

if __name__ == "__main__":
	main()
