const containerHeight = 520;
const containerWidth = 185;
const minutesinDay = 60 * 12;
let collisions = [];
let width = [];
let leftOffSet = [];

var date_global;
var personal_id;

// append one event to calendar
var createEvent = (name, place, height, top, left, units, day) => {
    let margin_day = {'mon': 0, 'tue': 1, 'wen': 2, 'thu': 3, 'fri': 4, 'sat': 5, 'sun': 6};
    let node = document.createElement('DIV');
    node.className = 'event';
    node.innerHTML =
        '<span class=\'title\'>' + name + '</span> \
  <br><span class=\'location\'>' + place + '</span>';
    // Customized CSS to position each event
    node.style.width = (containerWidth / units) + 'px';
    node.style.height = height + 'px';
    node.style.top = top + 'px';
    node.style.left = 100 + margin_day[day] * 200 + left + 'px';
    document.getElementById(day).appendChild(node);
};

function getAttributes(events) {
    //resets storage
    width = [];
    leftOffSet = [];

    for (var i = 0; i < events.length; i++) {
        width.push(0);
        leftOffSet.push(0);
    }

    collisions.forEach((period) => {
        // number of events in that period
        let count = period.reduce((a, b) => {
            return b ? a + 1 : a;
        });
        if (count > 1) {
            period.forEach((event, id) => {
                // max number of events it is sharing a time period with determines width
                if (period[id]) {
                    if (count > width[id]) {
                        width[id] = count;
                    }
                }
                if (period[id] && !leftOffSet[id]) {
                    leftOffSet[id] = period[id];
                }
            });
        }
    });
};

var layOutDay = (events) => {
    // clear any existing nodes
    var days = ["mon", "tue", "wen", "thu", "fri", "sun", "sat"];
    days.forEach((day) => {
        var myNode = document.getElementById(day);
        myNode.innerHTML = '';
    });
    getAttributes(events);

    events.forEach((event, id) => {
        let day = event.day;
        let height = (event.end - event.start) / minutesinDay * containerHeight;
        let top = event.start / minutesinDay * containerHeight;
        let units = width[id];
        if (!units) {
            units = 1;
        }
        ;
        let left = (containerWidth / width[id]) * (leftOffSet[id] - 1) + 10;
        if (!left || left < 0) {
            left = 10;
        }
        ;
        createEvent(event.name, event.place, height, top, left, units, day);
    });
};

function make_date(date) {
    date.setDate(date.getDate() - date.getDay() + 1);
    date_global = date;
    let days = ['ПН', 'ВТ', 'СР', 'ЧТ', 'ПТ', 'СБ', 'ВС'];
    let node = document.getElementById("dates").getElementsByTagName("tr")[0];
    node.innerHTML = '';
    days.forEach((name, i) => {
        node.innerHTML += '<th> <div>' + date.getDate() + '.' + (date.getMonth() + 1) + '.' + date.getFullYear() + '</div>' +
            '<div>' + name + '</div>' + '</th>\n';
        date.setDate(date.getDate() + 1);
    });
    date.setDate(date.getDate() - 7);
}

async function update_pred_time(id) {
    const time = document.forms["pred_time"]["time"].value;
    document.getElementById(id + "your_dead_time").innerHTML = time;
    var data = {'val': parseInt(time), 'deadline_id': id};
    await post_pred_deadline(personal_id, data);
}

function deadline_done(id) {
    var node = $('#' + id + "dead_done");
    node.empty();
    node.append('<form name=\"real_time\" onsubmit=update_real_time(' + id + ')>' +
        '<input name=\"time\" type=\"input\" placeholder=\"Введите потраченное время...\">' +
        '<input type=\"submit\" class=\"button\">' +
        '</form>');
}

async function update_real_time(id) {
    const time = document.forms["real_time"]["time"].value;
    document.getElementById(id + "dead_done").innerHTML = 'Сделано! \n Вы потратили ' + time;
    var data = {'val': parseInt(time), 'deadline_id': id};
    await post_real_deadline(personal_id, data);
}

// deadline: id, name, desc, time, est_time, real_time
function gen_table(deadline) {
    let id = deadline['id'];
    let name = deadline['name'];
    let desc = deadline['desc'];
    let time = deadline['time'];
    let est_time = deadline['est_time'];
    let real_time = deadline['real_time'];
    if (!real_time) {
        real_time = '-';
    }
    if (!est_time) {
        est_time = '-';
    }
    let parent = document.getElementById("deadlines");
    let node = document.createElement("tr");
    node.innerHTML = '<th style=\"font-weight: bold\">' + name + '</th>\n' +
        '<th>' + desc + '</th>\n' +
        '<th>' + time + '</th>\n' +
        '<th>' + est_time + '</th>\n' +
        '<th>' + real_time + '</th>\n' +
        '<th id=\"' + id + 'your_dead_time\">\n' +
        '<form name=\"pred_time\" onsubmit=update_pred_time(' + id + ')>\n' +
        '<input id=\"time\" name=\"time\" type=\"input\" placeholder=\"Введите время...\">\n' +
        '<input type=\"submit\" class=\"button\">\n' +
        '</form>\n' +
        '</th>\n' +
        '<th id=\"' + id + 'dead_done">\n' +
        '<button class=\"button\" onclick=deadline_done(' + id + ')> Я сделал! </button>\n' +
        '</th>'
    parent.appendChild(node);
}

function gen_tables(deadlines) {
    deadlines.forEach(deadline => gen_table(deadline));
}

async function send_name() {
    date_global = new Date();
    var person_name = String(document.forms["send"]["name"].value).trim();
    var names = await get_users_by_name(person_name);

    var node = $("<select id='persons'> </select>");
    node.attr("size", names.length);
    names.forEach((name) => {
        var child = $("<option> </option>");
        child.attr("value", name['id']);
        child.text(name['last_name'] + ' ' + name['first_name'] + ' ' + name['patronymic_name'] + ', ' + name['group_name']);
        node.append(child);
    });
    var parent = $("#names_list");
    var sub_button = $("<input type=\"submit\" class=\"button\">");
    var form = $("<form name=\"names\" onsubmit=\"get_id_make_all(); return false;\"> </form>");
    form.append(node, sub_button);
    parent.text("Найдите себя");
    parent.append(form);
    return false;
}

async function get_id_make_all() {
    var sel = $("#persons")[0];
    personal_id = sel.options[sel.selectedIndex].value;
    $("#names_list").empty();
    await update_timetable(personal_id, new Date());
    $("#schedule_cont").show();
    await update_deadlines(personal_id);
    $("#deadlines_cont").show();
}

//id, name, desc, time, est_time, real_time
async function update_deadlines(personal_id) {
    var answer = await get_user_deadlines(personal_id);
    var deadlines = [];
    answer.forEach((elem) => {
        var deadline = {};
        deadline['name'] = elem['deadline_name'];
        deadline['id'] = elem['deadline_id'];
        deadline['desc'] = elem['deadlines_description'];
        deadline['est_time'] = elem['estimated_time'];
        deadline['real_time'] = elem['real_time'];
        deadline['time'] = elem['deadline_time'].replace('T', ' ');
        deadlines.push(deadline);
    });
    console.log(deadlines);
    gen_tables(deadlines);
}

async function prev_week() {
    date_global.setDate(date_global.getDate() - 7);
    make_date(date_global);
    if (personal_id) {
        update_timetable(personal_id, date_global);
    }
}


async function next_week() {
    date_global.setDate(date_global.getDate() + 7);
    make_date(date_global);
    if (personal_id) {
        update_timetable(personal_id, date_global);
    }
}


async function adding_dead_form() {
    $("#form_container").append("\n" +
        "<form name='new_dead' id=\"new_dead\" onsubmit=\"send_deadline(); return false;\">\n" +
        "    <table  class=\"deadline\" id=\"new_dead_table\">\n" +
        "    </table>\n" +
        "</form>");
    var node = $("<tr id='new_dead_th'>  </tr>");
    node.append("<th><input id=\"name\" type=\"text\" placeholder=\"Введите название...\"></th>");
    node.append("<th><input id=\"desc\" type=\"text\" placeholder=\"Введите описание...\"></th>");
    var today = new Date();
    var today_format = today.getFullYear() + '-' + (today.getMonth() + 1) + '-' + today.getDate() + 'T00:00';

    node.append("<th><input id=\"date\" type=\"datetime-local\" value=" + today_format + " min=" + today_format + "></th>");
    var child = $("<select id=\"contingent\" list=\"cont\" placeholder=\"Для кого дедлайн...\"> </select>");
    var conts = await get_user_contingents(personal_id);
    conts.forEach((elem) => {
        var opt = $("<option value=" + elem['id'] + ">" + elem['name'] + ', ' + elem['type'] + "</option>");
        child.append(opt);
    });
    var tmp = $("<th> </th>");
    tmp.append(child);
    node.append(tmp);
    node.append("<th><input class=\"button\" type=\"submit\"></th>");
    $("#new_dead_table").empty();
    $("#new_dead_table").prepend(node);

}

async function send_deadline() {
    var form = $('form[name="new_dead"]')[0]

    const name = form["name"].value;
    const desc = form["desc"].value;
    const date = form["date"].value;
    const cont = form["contingent"].value;
    var data = {'desc': desc, 'name': name, 'time': date.replace('T', ' '), 'contingent_id': cont};
    console.log(data);

    await post_deadline(personal_id, data);
    $("#form_container").empty();
    await update_deadlines(personal_id);
}

function parse(time) {
    return (parseInt(time.split(':')[0]) - 9) * 60 + parseInt(time.split(':')[1]);
}

function make_time_ok(time) {
    var d = time.getDate();
    var m = (time.getMonth() + 1);
    var y = time.getFullYear();
    return (d < 10 ? '0' : '') +  d + '-' + (m < 10 ? '0' : '') + m + '-' + y;
}

// event = {name, place, start, end, day}
async function update_timetable(id, start_time) {
    start_time.setDate(start_time.getDate() - start_time.getDay());
    var end_time = new Date(start_time);
    end_time.setDate(end_time.getDate() + 7);
    var answer = await get_user_timetable(id, make_time_ok(start_time), make_time_ok(end_time));
    var events = [];
    make_date(date_global);
    var days_num = {0: 'mon', 1: 'tue', 2: 'wen', 3: 'thu', 4: 'fri', 5: 'sat', 6: 'sun'};
    answer.forEach((elem) => {
            let event = {};
            event['day'] = days_num[new Date(elem['date']).getDay()];
            event['name'] = elem['course_short_name'] + ', ' + elem['lesson_type'];
            event['place'] = elem['building_addr'];
            event['start'] = parse(elem['start']);
            event['end'] = parse(elem['end']);
            events.push(event);
        }
    );
    layOutDay(events);
}
