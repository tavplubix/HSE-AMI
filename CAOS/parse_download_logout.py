import requests
import os
import time

dump_file_name = 'dump'


def parse_dump(dump_file_name):
    dumpfile = open(dump_file_name, 'r').read()

    requests = []
    next_request = dumpfile.find('GET', 0)
    dumpfile += "\nqwerty"

    while next_request > 0:
        endl = dumpfile.find('\n', next_request)    
        line = dumpfile[next_request : endl]
        request = (line.split(' ')[1], {}, line.split(' ')[0])
            
        next_request = endl + 1
        endl = dumpfile.find('\n', next_request)
        line = dumpfile[next_request : endl]  
        while len(line.split(': ')) > 1: 
            s = line.split(': ')
            hname = s[0]
            hval = ': '.join(s[1:])
            request[1][hname] = hval
            next_request = endl + 1
            endl = dumpfile.find('\n', next_request)
            line = dumpfile[next_request : endl] 
        requests.append(request)
        next_request = dumpfile.find('GET', next_request + 1)
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


def send_request(request, url, params = ''):
    sid = get_sid(request)
    url = 'http://caos.ejudge.ru' + url + '/S' + sid + params
    if sid is None:
        return None
    headers = dict()
    try:
        headers['Cookie'] = request[1]['Cookie']
        headers['Host'] = request[1]['Host']
        headers['User-Agent'] = request[1]['User-Agent']
        headers['Accept'] = request[1]['Accept']
        headers['Accept-Language'] = request[1]['Accept-Language']
        headers['Accept-Encoding'] = request[1]['Accept-Encoding']
    except:
        print("\nWARNING: send_request: header not found\n")
    resp = requests.get(url, headers=headers)
    return (resp.headers, resp.content.decode('utf-8'))


def logout(request):
    send_request(request, '/ej/client/logout')


def all_run_ids(html):
    beg = 0
    ids = set()
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
        ids.add(to_id(html[beg:beg+5]))
        beg = html.find(pattern, beg+1)

    return sorted(list(ids))


def user_info(request):
    _, html = send_request(request, '/ej/client/view-submissions', '?all_runs=1')
    login_begin = html.find('[bpmi')
    login_end     = html.find(']', login_begin)
    if login_end < 0:
        print('WARING: user_info: cannot get info')
        return [1, 2, 3]
    login = html[login_begin + 1 : login_end]
    info_begin = html.find('<title>')
    info_end = html.find('</title>', info_begin)
    if info_end < 0:
        print('WARING: user_info: cannot get info')
        return [1, 2, 3]
    info = html[info_begin + len('<title>') : info_end]
    open(login + '.submissions.html', 'w').write(html)
    return (login, info, html, all_run_ids(html))


def download_all_runs(request, run_ids, dir_name):
    os.makedirs(dir_name, exist_ok=True)
    for i in run_ids:
        _,run = send_request(request, '/ej/client/view-source', '?run_id=' + str(i))
        open(dir_name + '/run_' + str(i) + '.html', 'w').write(run)
        print(i)


for url,headers,method in unique_requests(parse_dump(dump_file_name)):
    print(get_sid([url,headers]))    
    print(get_cookie([url,headers]))
    print(url)
    for hn in headers:
        print (hn + ': ' + headers[hn])
    info = user_info([url,headers])
    print(info[0], info[1], info[3])
    download_all_runs([url,headers], info[3], info[0])
    logout([url,headers])
    print('\n' + '='*80  + '\n')
    time.sleep(2)

