begin execute immediate '
    create  or replace procedure transfer_money(from_a int, to_a int, am number, st int, tid int := null) is
        not_enough_money EXCEPTION;
        from_am_old number;
        max_tr_id int;
        st_old int;
    begin
        select ACCOUNTS.amount into from_am_old from ACCOUNTS where id = from_a;
        if (am <= from_am_old and st = 0) or st = 1 then
            select nvl(max(id), 0) into max_tr_id from TRANSACTIONS;
            insert into TRANSACTIONS values (max_tr_id + 1, current_date, from_a, to_a, am, st);
        elsif am <= from_am_old and st = 2 then
            select status into st_old from TRANSACTIONS where id = tid;
            if st_old = 1 then
                update TRANSACTIONS set status = 2 where id = tid;
            end if;
        end if;
        if am <= from_am_old and (st = 0 or st = 2) then
            update ACCOUNTS set ACCOUNTS.amount = from_am_old - am where id = from_a;
            select ACCOUNTS.amount into from_am_old from ACCOUNTS where id = to_a;
            update ACCOUNTS set ACCOUNTS.amount = from_am_old + am where id = to_a;
        elsif am > from_am_old and (st = 0 or st = 2) then
            raise not_enough_money;
        end if;
    end;';
    execute immediate '
    create or replace procedure confirm_or_reject(tid int, st int) is
        wrong_status EXCEPTION;
        from_a int;
        to_a int;
        am number;
        st_old int;
    begin
        select from_ac, to_ac, amount, status into from_a, to_a, am, st_old from TRANSACTIONS where id = tid;
        if st_old != 1 then
            raise wrong_status;
        end if;
        if st = 2 then
            transfer_money(from_a, to_a, am, 2, tid);
        elsif st = 3 then
            update TRANSACTIONS set status = 3 where id = tid;
        else
            raise wrong_status;
        end if;
    end;';
end;