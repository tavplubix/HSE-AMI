import inspect
import os
from collections import Callable
from datetime import date
from datetime import datetime
from datetime import time
from datetime import timedelta
from json import JSONEncoder
from functools import wraps

class DatetimeJSONEncoder(JSONEncoder):
    def default(self, obj):
        try:
            if isinstance(obj, date):
                return obj.isoformat()
            if isinstance(obj, time):
                return obj.isoformat()
            if isinstance(obj, timedelta):
                return f"{obj}"
            iterable = iter(obj)
        except TypeError:
            pass
        else:
            return list(iterable)
        return JSONEncoder.default(self, obj)


def debug(args, end="\n", depth=1):
    filename = os.path.basename(inspect.stack()[depth][1])
    line = inspect.stack()[depth][2]
    function_name = inspect.stack()[depth][3]
    time = datetime.now()
    current_time = time.strftime("%H:%M:%S")
    current_date = time.strftime("%d-%m-%Y")
    print(f"{current_date} {current_time} [{function_name} @ {filename}:{line}] {args}", end=end)

def log(function):
    @wraps(function)
    def wrapper(*args, **kwargs):
        debug(f"Entered function {function.__name__} with args = {args}, kwargs = {kwargs}")
        result = function(*args, **kwargs)
        debug(f"Exited function")
        return result

    return wrapper