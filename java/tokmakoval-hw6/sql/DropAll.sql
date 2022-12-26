declare
    c int;
begin
    select count(*) into c from USER_TABLES where table_name = upper('Sessions');
   if c = 1 then
      execute immediate 'drop table Sessions';
   end if;

   select count(*) into c from USER_TABLES where table_name = upper('Transactions');
   if c = 1 then
      execute immediate 'drop table Transactions';
   end if;

   select count(*) into c from USER_TABLES where table_name = upper('A2U');
   if c = 1 then
      execute immediate 'drop table A2U';
   end if;

   select count(*) into c from USER_TABLES where table_name = upper('Accounts');
   if c = 1 then
      execute immediate 'drop table Accounts';
   end if;

   select count(*) into c from USER_TABLES where table_name = upper('Users');
   if c = 1 then
      execute immediate 'drop table Users';
   end if;
end;
