create function find_users(_first_name character varying, _last_name character varying, _patronymic_name character varying) returns SETOF students
  language plpgsql
as
$$
begin
  return query
    select *
    from students
    where lower(students.first_name) like lower(concat(_first_name, '%'))
       or lower(students.last_name) like lower(concat(_last_name, '%'))
       or lower(students.patronymic_name) like lower(concat(_patronymic_name, '%'));
end;
$$;

alter function find_users(varchar, varchar, varchar) owner to postgres;


create function get_contingent_id_by_user_id(_user_id integer) returns TABLE(id bigint, name text, type character varying)
  language plpgsql
as
$$
begin
  return query
    select distinct stc.contingent_id, lc.fullname, lesson.lesson_type
    from lesson
           join contingents c on lesson.contingent_id = c.id
           join students_to_contingents stc on c.id = stc.contingent_id
           join learning_courses lc on lesson.course_id = lc.id
    where stc.student_id = _user_id;

end;
$$;

alter function get_contingent_id_by_user_id(integer) owner to postgres;



create function get_deadlines_by_id(_user_id integer, _time_start timestamp without time zone, _time_end timestamp without time zone) returns TABLE(user_id bigint, first_name character varying, flow character varying, course_name_short character varying, course_name text, deadline_id bigint, deadline_name character varying, deadline_time timestamp without time zone, deadlines_description text, estimated_time interval, real_time interval)
  language plpgsql
as
$$
begin
  return query select students.id                 as "id",
                      students.first_name         as "first_name",
                      contingents.contingent_name as "flow",
                      courses.shortname           as "course_name_short",
                      courses.fullname            as "course_name",
                      deadlines.id                as "deadline_id",
                      deadlines.deadline_name     as "deadline_name",
                      deadlines.deadline_time     as "deadline_time",
                      deadlines.description       as "deadlines_description",
                      avg(task_time.estimated_time)    as "estimated_time",
                      avg(task_time.real_time)         as "real_time"
               from students students
                      join students_to_contingents stc on students.id = stc.student_id
                      join contingents contingents on stc.contingent_id = contingents.id
                      join deadlines deadlines on contingents.id = deadlines.contingent_id
                      left join task_time task_time on deadlines.id = task_time.deadline_id
                      join learning_courses courses on deadlines.course_id = courses.id

               where students.id = _user_id
                 and deadlines.deadline_time between _time_start and _time_end
               group by students.id, students.first_name, contingents.contingent_name, courses.shortname, courses.fullname, deadlines.id, deadlines.deadline_name, deadlines.deadline_time, deadlines.description;
end
$$;

alter function get_deadlines_by_id(integer, timestamp, timestamp) owner to postgres;




create function get_timetable_by_user_id(_user_id integer) returns TABLE(user_id bigint, first_name character varying, lesson_time_id bigint, date date, start time without time zone, "end" time without time zone, building_addr text, lesson_type character varying, flow character varying, course_short_name character varying, course_full_name text)
  language plpgsql
as
$$
begin
  return query select students.id                 as "id",
                      students.first_name         as "first_name",
                      lessons.lesson_time_id      as "lesson_time_id",
                      lessons.date                as "date",
                      lesson_time.time_start      as "start",
                      lesson_time.time_end        as "end",
                      buildings.addr              as "building_addr",
                      lessons.lesson_type         as "type",
                      contingents.contingent_name as "flow",
                      curses.shortname            as "course_name_short",
                      curses.fullname             as "course_name"
               from students students
                      join students_to_contingents stc on students.id = stc.student_id
                      join contingents contingents on stc.contingent_id = contingents.id
                      join lesson lessons on contingents.id = lessons.contingent_id
                      join learning_courses curses on lessons.course_id = curses.id
                      join auditoriums auditoriums on lessons.auditorium_id = auditoriums.id
                      join buildings buildings on auditoriums.building_id = buildings.id
                      join lesson_time lesson_time on lessons.lesson_time_id = lesson_time.id
               where students.id = _user_id;
end
$$;

alter function get_timetable_by_user_id(integer) owner to postgres;



create function insert_deadline(_user_id bigint, _contingent_id bigint, _deadline_time timestamp without time zone, _weight double precision, _name character varying, _description text) returns void
  language plpgsql
as
$$
begin
  insert into deadlines(deadline_time, weight, deadline_name, description, contingent_id, course_id)
  values (_deadline_time, _weight, _name, _description, _contingent_id,
          (select course_id from lesson where _contingent_id=_contingent_id limit 1));
end
$$;

alter function insert_deadline(bigint, bigint, timestamp, double precision, varchar, text) owner to postgres;


