import requests
import time

reqtxt = open('request').read().split('\n')

url = 'http://caos.ejudge.ru' + reqtxt[0].split()[1]

reqtxt = reqtxt[1:-1]

headers = {}
for header in reqtxt:
	#print(header)
	n,v = header.split(': ')
	headers[n] = v

ids = [1110, 1110, 1109, 1109, 1108, 1108, 1107, 1107, 1106, 1106, 1105, 1105, 1104, 1104, 1103, 1103, 1102, 1102, 1101, 1101, 1099, 1099, 1095, 1095, 1094, 1094, 1087, 1087, 1086, 1086, 1085, 1085, 1084, 1084, 1083, 1083, 1082, 1082, 1081, 1081, 1080, 1080, 1079, 1079, 1077, 1077, 1073, 1073, 1046, 1046, 849, 849, 847, 847, 845, 845, 844, 844, 743, 743, 742, 742, 741, 741, 740, 740, 739, 739, 673, 673, 652, 652, 650, 650, 627, 627, 623, 623, 618, 618, 617, 617, 598, 598, 597, 597, 596, 596, 397, 397, 396, 396, 390, 390, 389, 389, 388, 388, 387, 387, 385, 385, 384, 384, 374, 374, 373, 373, 372, 372, 369, 369, 368, 368, 367, 367, 366, 366, 365, 365, 364, 364, 363, 363, 362, 362, 331, 331, 330, 330, 329, 329, 323, 323, 185, 185, 184, 184, 50, 50, 49, 49, 47, 47, 45, 45, 8, 8, 7, 7]

for i in ids:
	resp = requests.get(url + str(i), headers=headers)
	print(str(i) + ': ' + str(resp.status_code))
	open('orlov/run_' + str(i) + '.html', 'bw').write(resp.content)
	time.sleep(1)
	

