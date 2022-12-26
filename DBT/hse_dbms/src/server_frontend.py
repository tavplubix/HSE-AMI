import logging
import os

from flask import Flask
from flask import jsonify
from flask import request
from flask_cors import CORS

from src.server_backend import Server
from src.utils import DatetimeJSONEncoder
from src.utils import debug
from src.utils import log

api = "/api"

srv = Server()

app = Flask(
    __name__,
    root_path=os.path.abspath('.'),
)

CORS(app)

app.json_encoder = DatetimeJSONEncoder

@app.route(f"{api}/users/<int:user_id>", methods=["GET"])
def get_user_name(user_id: int):
    logging.debug(f"Got request at endpoint \"{api}/users/{user_id}\"")
    result = jsonify(srv.get_user_info(user_id=user_id))
    return result


@app.route(f"{api}/users/<user_name>", methods=["GET"])
def get_user_id_by_name(user_name: str):
    logging.debug(f"Got request at endpoint \"{api}/users/{user_name}\"")
    result = jsonify(srv.get_user_info(user_name=user_name))
    debug("Exiting")
    return result


@app.route(f"{api}/contingents/<int:user_id>", methods=["GET"])
def get_contingent_by_user_id(user_id):
    logging.debug(f"Got request ad endpoint \"{api}/contingents/{user_id}\"")
    result = jsonify(srv.get_contingent_by_user_id(user_id))
    return result

@app.route(f"{api}/buildings/<int:building_id>", methods=["GET"])
def get_building_by_id(building_id: int):
    logging.debug(f"Got request at endpoint \"{api}/buildings/{building_id}\"")
    result = jsonify(srv.get_building(id=building_id))
    return result


@app.route(f"{api}/buildings/<building_name>", methods=["GET"])
def get_building(building_name: str):
    logging.debug(f"Got request at endpoint \"{api}/buildings/{building_name}\"")
    result = jsonify(srv.get_building(building_name=building_name))
    debug("Exiting")
    return result


@app.route(f"{api}/auditoriums/<int:auditorium_id>", methods=["GET"])
def get_auditorium_by_id(auditorium_id: int):
    logging.debug(f"Got request at endpoint \"{api}/auditoriums/{auditorium_id}\"")
    result = jsonify(srv.get_auditorium(id=auditorium_id))
    return result


@app.route(f"{api}/auditoriums_numbers/<auditorium_name>", methods=["GET"])
def get_auditorium(auditorium_name):
    logging.debug(f"Got request at endpoint \"{api}/auditoriums_numbers/{auditorium_name}\"")
    result = jsonify(srv.get_auditorium(number=auditorium_name))
    debug("Exiting")
    return result


@app.route(f"{api}/teachers/<int:teacher_id>", methods=["GET"])
def get_teacher_by_id(teacher_id: int):
    logging.debug(f"Got request at endpoint \"{api}/teachers/{teacher_id}\"")
    result = jsonify(srv.get_teacher(id=teacher_id))
    return result


@app.route(f"{api}/teachers/<teacher_name>", methods=["GET"])
def get_teacher(teacher_name: str):
    logging.debug(f"Got request at endpoint \"{api}/teachers/{teacher_name}\"")
    result = jsonify(srv.get_teacher(name=teacher_name))
    debug("Exiting")
    return result


@app.route(f"{api}/timetable/<int:user_id>", methods=["GET"])
def get_user_timetable(user_id):
    time_start = request.args.get("time_start", default=None)
    time_end = request.args.get("time_end", default=None)
    logging.debug(f"Got request at endpoint \"{api}/timetable/{user_id}\" with args time_start = {time_start}, time_end = {time_end}")
    result = srv.get_timetable(user_id, time_start, time_end)
    return jsonify(result)


@app.route(f"{api}/deadlines/<int:user_id>", methods=["GET"])
def get_user_deadlines(user_id):
    debug("Entered")
    time_start = request.args.get("time_start", default=None)
    time_end = request.args.get("time_end", default=None)
    result = srv.get_deadlines(user_id, time_start, time_end)
    debug("Exiting")
    return jsonify(result)


@app.route(f"{api}/set_est/<int:user_id>", methods=["GET"])
def set_est_time(user_id):
    debug("Entered")
    deadline = request.args.get("deadline_id")
    value = request.args.get("val")
    srv.change_deadline_estimate(user_id, deadline, value)
    debug("Exiting")
    return jsonify({})


@app.route(f"{api}/set_real/<int:user_id>", methods=["GET"])
def set_real_time(user_id):
    debug("Entered")
    deadline = request.args.get("deadline_id")
    value = request.args.get("val")
    srv.change_deadline_real(user_id, deadline, value)
    debug("Exiting")
    return jsonify({})


@app.route(f"{api}/create_deadline/<int:user_id>", methods=["GET"])
def create_deadline(user_id):
    debug("Entered")
    contingent_id = request.args.get("contingent_id")
    time = request.args.get("time")
    weight = float(request.args.get("weight", default='0'))
    name = request.args.get("name")
    desc = request.args.get("desc", default='')
    id = jsonify(srv.create_deadilne(user_id, contingent_id, time, weight, name, desc))
    debug("Exiting")
    return id


