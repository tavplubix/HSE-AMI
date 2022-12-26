def link_lesson(self, lesson, student_id):
    # TODO использовать Server.get_*
    aud = self.db.query(f"""SELECT * FROM auditoriums WHERE id={lesson['auditorium_id']}""").dictresult()
    if len(aud) == 0:
        auditorium_name = lesson['auditorium'] + ' | ' + lesson['building']
        auditorium = self.auditoriumLoader.load_term(auditorium_name)
        if lesson['auditorium_id'] not in auditorium:
            lesson['auditorium_id'] = None
    lesson.pop('auditorium')
    lesson.pop('building')

    course = self.db.query(f"""SELECT * FROM learning_courses WHERE id={lesson['course_id']}""").dictresult()
    if len(course) == 0:
        self.db.insert('learning_courses',
                       {'id': lesson['course_id'], 'shortname': lesson['course'], 'fullname': lesson['course']})
    lesson.pop('course')

    cont = self.db.query(f"""SELECT * FROM contingents WHERE id={lesson['contingent_id']}""").dictresult()
    if len(cont) == 0:
        self.db.insert('contingents', {'id': lesson['contingent_id'], 'contingent_name': lesson['contingent']})
    lesson.pop('contingent')

    self.db.query(f"""INSERT INTO students_to_contingents VALUES ({student_id}, {lesson[
        'contingent_id']}) ON CONFLICT DO NOTHING""")

    tname = self.teacherLoader.split_name(lesson['teacher'])
    teacher = self.db.query(
        f"""SELECT * FROM teachers WHERE last_name='{tname[0]}' and first_name='{tname[1]}' and patronymic_name='{
        tname[2]}'""").dictresult()
    if len(teacher) == 0:
        teachers = self.teacherLoader.load_term(lesson['teacher'])
        if len(teachers) == 0:
            lesson['teacher_id'] = None
        else:
            lesson['teacher_id'] = list(teachers)[0]
    else:
        lesson['teacher_id'] = teacher[0]['id']
    lesson.pop('teacher')

    lesson.pop('id')
    return lesson
