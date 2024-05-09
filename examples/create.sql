create table t_user(
    id int primary key auto_increment comment '用户id',
    name varchar(50)    not null unique comment '用户名',
    password varchar(50)    not null comment '用户密码',
    state   enum('online','offline') default 'offline' comment '用户在线状态' 
)engine=innodb default charset=utf8 ;


create table t_friend(
    userid int not null comment '用户id',
    friendid int not null comment '朋友id',
    primary key(userid, friendid)
)engine=innodb default charset=utf8;


create table t_group(
    id int primary key auto_increment comment '群id',
    groupname varchar(50) not null unique comment '群昵称',
    groupdesc varchar(200) default '' comment '群描述'
)engine=innodb default charset=utf8;


create table t_groupUser(
    groupid int not null comment '群id',
    userid  int not null comment '用户id',
    grouprole   enum('admin','normal') default 'normal' comment '角色'
)engine = innodb default charset=utf8;

create table t_offlineMessage(
    userid int not null comment '用户id',
    message varchar(500) not null comment '离线消息'
)engine=innodb default charset=utf8;