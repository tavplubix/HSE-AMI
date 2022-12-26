import requests
import time



cred = open('tokens').read().split('\n')[0:-1]

counter = 0

while True:
	for line in cred:
		token, cookie = line.split(' ')
		url = 'http://google.com'
		data = { 
			'SID' : token,
			'prob_id' : '41',
			'lang_id' : '2',
			'action_40' : 'Send!'
		}
		files = { 'file' : ('code.c', open('benckmark.c', 'rb')) }
		headers = dict()
		headers['Cookie'] = cookie
		resp = requests.post(url, headers=headers, data=data, files=files)
		print()
		print(resp.status_code)
		print(resp.headers)
		print(resp.content)
		print()
		#time.sleep(2)
	open('benckmark.c', 'a').write('// ' + str(counter) + '\n')
	counter += 1



