import requests
import time

logpass = open('logpass').read().split('\n')[0:-1]

url = 'http://google.com'

result = ""

for lp in logpass:
	log, pas = lp.split(' ')
	body = 'contest_id=60&role=0&prob_name=&login=' + log + '&password=' + pas + '&locale_id=0&action_2=Log+iniW'
	resp = requests.post(url, data=body, allow_redirects=False)

	print()
	print(resp.status_code)
	print(resp.headers)
	print(resp.content)
	print(resp.cookies)
	print('Cookie: ' + resp.headers['Set-Cookie'])
	print('Location: ' + resp.headers['Location'])
	print()

	cookie = resp.headers['Set-Cookie'].split(';')[0]
	token = resp.headers['Location'].split('S')[1]
	
	result += token + ' ' + cookie + '\n'
	time.sleep(2)

open('tokens', 'w').write(result)




