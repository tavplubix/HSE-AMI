import requests
import time


def allids(html):

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

	return ids


def	getids(token, cookie):
	url = 'http://caos.ejudge.ru/ej/client/view-submissions/S' + token + '?all_runs=1'
	reqtxt = reqtxt[1:-1]
	headers = {}
	for header in reqtxt:
		print(header)
		n,v = header.split(': ')
		headers[n] = v
	headers['Cookie'] = cookie
	resp = requests.get(url, headers=headers)
	return allids(resp.body)





