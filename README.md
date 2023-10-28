# 网络聊天室

## 目录结构
```
├── chat_room.h 	// 存放sqlite函数声明和消息传递结构体
├── client.cpp		// 客户端的实现
├── Makefile
├── README.md
├── server.cpp		// 服务器的实现
└── sqlite.cpp		// 数据库部分的实现
```

## sqlite数据库

### 介绍
SQLite 是一种嵌入式关系型数据库管理系统（RDBMS），它在一个单一的、自包含的文件中实现了一个完整的、独立的 SQL 数据库引擎。与传统的客户端-服务器数据库管理系统不同，SQLite 不需要一个单独的服务器进程来管理数据库，而是直接将数据库存储在磁盘上的文件中。

### ubuntu上安装sqlite
```
sudo apt install sqlite3 libsqlite3-dev
```

### sqlite命令分类
- 系统命令：是以`.`开头的命令，主要用于对当前数据库操作
- 普通命令：是以`;`结尾的命令，主要对数据库中的表进行操作

### sqlite常用命令

```sqlite
.open <name>    打开指定数据库
.schema         查看表的结构
.table          列出当前数据库中所有表
.quit           退出数据库
.exit           退出数据库
.help           查看帮助信息
.databases      查看数据库
```

## 功能
- 注册
- 群聊

## 待完成功能
- [ ] 私聊
- [ ] 切换聊天
- [ ] 用户退出聊天室

## 存在的bug
- [ ] 可以同时登陆同一个帐号
