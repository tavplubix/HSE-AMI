begin
    execute immediate
        'create table USERS
        (
        	ID NUMBER not null,
        	LOGIN VARCHAR2(64) not null,
        	PASSWORD VARCHAR2(64) not null,
        	NAME VARCHAR2(64) not null
        )';
    execute immediate
        'create unique index USERS_ID_UINDEX
        	on USERS (ID)';
    execute immediate
        'create unique index USERS_LOGIN_UINDEX
        	on USERS (LOGIN)';
    execute immediate
        'alter table USERS
        	add constraint USERS_PK
        		primary key (ID)';
    execute immediate
        'alter table USERS
        	add constraint USERS_PK_2
        		unique (LOGIN)';


    execute immediate
        'create table ACCOUNTS
        (
        	ID NUMBER not null,
        	NAME VARCHAR2(64),
        	TYPE NUMBER not null,
        	AMOUNT NUMBER(15,2) default 0 not null
        )';
    execute immediate
        'create unique index ACCOUNTS_ID_UINDEX
        	on ACCOUNTS (ID)';
    execute immediate
        'alter table ACCOUNTS
        	add constraint ACCOUNTS_PK
        		primary key (ID)';


    execute immediate
        'create table A2U
        (
        	"uid" NUMBER not null
        		constraint A2U_PK
        			primary key
        		constraint A2U_USERS_ID_FK
        			references USERS,
        	AID NUMBER
        		constraint A2U_ACCOUNTS_ID_FK
        			references ACCOUNTS
        				on delete cascade,
        	RIGHTS NUMBER not null
        )';


    execute immediate
        'create table TRANSACTIONS
        (
        	ID NUMBER not null,
            DT DATE not null,
        	FROM_AC NUMBER not null
        		constraint TRANSACTIONS_ACCOUNTS_ID_FK
        			references ACCOUNTS
        				on delete cascade,
        	TO_AC NUMBER not null
        		constraint TRANSACTIONS_ACCOUNTS_ID_FK_2
        			references ACCOUNTS
        				on delete cascade,
        	AMOUNT NUMBER not null,
        	STATUS NUMBER not null
        )';
    execute immediate
        'create unique index TRANSACTIONS_ID_UINDEX
        	on TRANSACTIONS (ID)';
    execute immediate
        'alter table TRANSACTIONS
        	add constraint TRANSACTIONS_PK
        		primary key (ID)';


    execute immediate
    'create table SESSIONS
(
	token varchar(16) not null,
	begin DATE not null,
	expire DATE not null,
	"uid" int not null
		constraint SESSIONS_USERS_ID_fk
			references USERS
				on delete cascade
)';

    execute immediate
'create unique index SESSIONS_token_uindex
	on SESSIONS (token)';

    execute immediate
'alter table SESSIONS
	add constraint SESSIONS_pk
		primary key (token)';

end;