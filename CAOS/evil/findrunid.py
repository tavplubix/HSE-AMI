html = open('bpmi16501.submissions.html').read()

beg = 0

ids = []

def to_id(s):
	first_non_digit = 0
	for c in s:
		if '0' <= c <= '9':
			first_non_digit += 1
	return int(s[0:first_non_digit])

pattern = '?run_id='
beg = html.find(pattern)
while beg >= 0:
	beg += len(pattern)
	ids += [to_id(html[beg:beg+5]),]
	beg = html.find(pattern, beg+1)

print(ids)	

