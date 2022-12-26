create table USERS
(
	ID NUMBER not null,
	LOGIN VARCHAR2(64) not null,
	PASSWORD VARCHAR2(64) not null,
	NAME VARCHAR2(64) not null
);
create unique index USERS_ID_UINDEX
	on USERS (ID);
create unique index USERS_LOGIN_UINDEX
	on USERS (LOGIN);
alter table USERS
	add constraint USERS_PK
		primary key (ID);
alter table USERS
	add constraint USERS_PK_2
		unique (LOGIN);


create table ACCOUNTS
(
	ID NUMBER not null,
	NAME VARCHAR2(64),
	TYPE NUMBER not null,
	AMOUNT NUMBER(15,2) default 0 not null
);
create unique index ACCOUNTS_ID_UINDEX
	on ACCOUNTS (ID);
alter table ACCOUNTS
	add constraint ACCOUNTS_PK
		primary key (ID);


create table A2U
(
	"uid" NUMBER not null
		constraint A2U_USERS_ID_FK
			references USERS,
	AID NUMBER not null
		constraint A2U_ACCOUNTS_ID_FK
			references ACCOUNTS
				on delete cascade,
	RIGHTS NUMBER not null,
	constraint A2U_PK
		primary key ("uid", AID)
);


create table TRANSACTIONS
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
);
create unique index TRANSACTIONS_ID_UINDEX
	on TRANSACTIONS (ID);
alter table TRANSACTIONS
	add constraint TRANSACTIONS_PK
		primary key (ID);


create table SESSIONS
(
	TOKEN VARCHAR2(16) not null,
	BEGIN DATE not null,
	EXPIRE DATE not null,
	"uid" NUMBER not null
		constraint SESSIONS_USERS_ID_FK
			references USERS
				on delete cascade
);
create unique index SESSIONS_TOKEN_UINDEX
	on SESSIONS (TOKEN);
alter table SESSIONS
	add constraint SESSIONS_PK
		primary key (TOKEN)

