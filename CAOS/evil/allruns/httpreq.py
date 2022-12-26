import requests

reqtxt = open('request').read().split('\n')

url = 'http://caos.ejudge.ru' + reqtxt[0].split()[1]

reqtxt = reqtxt[1:-1]

headers = {}
for header in reqtxt:
	print(header)
	n,v = header.split(': ')
	headers[n] = v

resp = requests.get(url, headers=headers)

print(resp.status_code)
print()
print(resp.headers)
print()
print(resp.content)
print()
print(resp.cookies)
print()

open('kr04-5.html', 'bw').write(resp.content)


