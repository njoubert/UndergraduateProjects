use 'webzync';

drop table if exists room;
create table room (
  id				varchar(40)				not null,
  name				varchar(255),
  primary key(iroom)
);

drop table if exists media;
create table media (
  id				int				not null auto_increment,
  url				varchar(255),
  original_url		varchar(255),	
  title				varchar(255),
  primary key(id)
);

drop table if exists events;
create table events (
  id				int				not null auto_increment,
  yuid				varchar(200)	not null,
  session_id		varchar(200)	not null,
  roomid				varchar(40),
  command			varchar(40)		not null,
  args				text,
  timestamp 		double,
  media				int,
  date				datetime,
  primary key(id)
);