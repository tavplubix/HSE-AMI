
/**
 *
 * @param _user_id integer id of student
 * @returns list of dictionaries with following keys:
 *
 * ```
 * {
 *     id:              integer,
 *     first_name:      string,
 *     last_name:       string,
 *     patronymic_name: string,
 *     email:           string
 *     group:           string
 * }
 * ```
 *
 * If no results available, then returns empty list
 */
async function get_user_info_by_id(_user_id) {

    return await $.get(
        "http://localhost:9999/api/users/" + _user_id,
        function (result) {
            return result
        }
    );
}

/**
 *
 * @param name_string concatenated string with name, surname and patronymic name
 *
 * @returns list of dictionaries with following keys:
 *
 * ```
 * {
 *     id:              integer,
 *     first_name:      string,
 *     last_name:       string,
 *     patronymic_name: string,
 *     email:           string
 * }
 * ```
 */
async function get_users_by_name(name_string) {
    var addr = "http://localhost:9999/api/users/" + name_string;
    return await $.get(addr)
        .done(function (result) {
            return result;
        }
        );
}

/**
 *
 * @param user_id integer id of student
 *
 * @returns list of dictionaries with following keys:
 *
 * ```
 * {
 *     user_id           bigint,
 *     first_name        string,
 *     date              string,
 *     start             string,
 *     end               string,
 *     building_addr     string,
 *     lesson_type       string,
 *     flow              string,
 *     course_short_name string,
 *     course_full_name  string
 * }
 * ```
 *
 *
 */

async function get_user_timetable(user_id, time_start, time_end) {
    var data = {'time_start': time_start, 'time_end': time_end};
    return await $.get(
        "http://localhost:9999/api/timetable/" + user_id,
        data,
        function (result) {
            return result
        }
    )
}

/**
 * @param user_id
 *
 * ```
 * {
 *     user_id               integer,
 *     first_name            string,
 *     flow                  string,
 *     course_name_short     string,
 *     course_name           string,
 *     deadline_name         string,
 *     deadline_time         string,
 *     deadlines_description string,
 *     estimated_time        string,
 *     real_time             string,
 * }
 * ```
 */
async function get_user_deadlines(user_id) {
    return await $.get(
        "http://localhost:9999/api/deadlines/" + user_id,
        function (result) {
            return result
        }
    )
}

/**
 * id
 * name
 * type

 */
async function get_user_contingents(user_id) {
    return await $.get(
        "http://localhost:9999/api/contingents/" + user_id,
        function (result) {
            return result
        }
    )
}


async function post_deadline(user_id, data) {
    return await $.get(
        "http://localhost:9999/api/create_deadline/" + user_id,
        data,
        function (result) {
            return result
        }
    )
}


async function post_pred_deadline(user_id, data) {
    return await $.get(
        "http://localhost:9999/api/set_est/" + user_id,
        data,
        function (result) {
            return result
        }
    )
}


async function post_real_deadline(user_id, data) {
    return await $.get(
        "http://localhost:9999/api/set_real/" + user_id,
        data,
        function (result) {
            return result
        }
    )
}