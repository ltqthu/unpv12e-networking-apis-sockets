# 第一个套接字程序

## ## 客户端

```cpp
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>

#define MAXLINE 4096

int main(int argc, char **argv) {
    int sockfd, n;
    char recvline[MAXLINE+1];
    struct sockaddr_in servaddr;
    
    if (argc != 2) {
        printf("usage: a.out <IPaddress>\n");
        exit(1);
    }

    if ( (sockfd= socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("socket error\n");
        exit(1);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(13);

    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
        printf("inet_pton error for %s\n", argv[1]);
        exit(1);
    }

    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        printf("connect error\n");
        exit(0);
    }

    while ( (n=read(sockfd, recvline, MAXLINE)) > 0) {
        recvline[n] = 0;
        if (fputs(recvline, stdout) == EOF) {
            printf("fputs error\n");
            exit(1);
        }
    }

    if (n<0) {
        printf("read error\n");
        exit(1);
    }

    exit(0);
}
```



## 服务端

```cpp
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<time.h>

#define MAXLINE 4096
#define LISTENQ 1024

int main(int argc, char **argv)
{
	int					listenfd, connfd;
	struct sockaddr_in	servaddr;
	char				buff[MAXLINE];
	time_t				ticks;

    if (argc != 2) {
        printf("usage: a.out <IPaddress>\n");
        exit(1);
    }

    if ( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("socket error\n");
        exit(1);
    }

    memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(13);	/* daytime server */

    if (bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        printf("bind error\n");
        exit(1);
    }

	if (listen(listenfd, LISTENQ) < 0) {
        printf("listen error\n");
        exit(1);
    }

	for ( ; ; ) {
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
        if (connfd < 0) {
            printf("connfd < 0");
        }
        ticks = time(NULL);
        snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
        if (write(connfd, buff, strlen(buff)) != strlen(buff)) {
            printf("write error");
        }

	    if (close(connfd) == -1) {
            printf("close error");
        }
	}
}
```









## bzero()和memset()函数

在原书中，全部使用bzero()函数来对socket地址结构执行清零操作。那么这两个函数有什么区别呢？

1、bzero()
```
函数原型：extern void bzero(void *s, int n)
头文件：<string.h>
功能：置字节字符串s的前n个字节为零且包括‘\0’
```
说明：无返回值

2、memset()

```
函数原型：extern void *memset(void *buffer, int c, int count)
头文件：<string.h>
功能：把buffer所指内存区域的前count个字节设置成c的值。
```

需要注意的是，`bzero`不是一个ANSI C函数，它起源于早起的Berkeley网络编程代码，几乎所有支持套接字API的厂商都提供bzero()。为什么书中使用非ANSI C函数bzero()而不使用ANSI C函数memset()呢？作者给出的答案是：bzero（带2个参数）比memset（带3个参数）更好记忆。另外还可以避免C编译器无法发现的错误，因为memset的第二个和第三个参数都是int，万一在开发过程中这两个参数被颠倒，那么编译器也不会报错，但是程序运行时会出现未知的错误。

尽管如此，从通用性和简短性考虑，本文依然使用更常规、使用更广泛的memset函数。

## exit()、_exit()与return的区别

### `exit()`和`return()`的对比

| return()                                                     | exit()                                                       |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| return(): 表示某个函数的结束，并返回结果。                   | exit(): 表示整个程序的结束。<br />exit(0): 正常运行程序并退出程序；<br />exit(1): 非正常运行导致退出程序； |
| return是关键字，返回函数值。                                 | exit 是函数。                                                |
| return是语言级别的，它表示了调用堆栈的返回；                 | exit是系统调用级别的，它表示了一个进程的结束。               |
| return是函数的退出(返回)；                                   | exit是进程的退出。                                           |
| return是C语言提供的                                          | exit是操作系统提供的（或者函数库中给出的）。                 |
| return用于结束一个函数的执行，将函数的执行信息传出个其他调用函数使用； | exit函数是退出应用程序，删除进程使用的内存空间，并将应用程序的一个状态返回给OS，这个状态标识了应用程序的一些运行信息，这个信息和机器和操作系统有关，一般是 0 为正常退出，非0 为非正常退出。 |


需要指出的是，在非main函数中调用return和exit效果的区别很明显，但是在main函数中调用return和exit的现象就很模糊，多数情况下现象都是一致的。

### `exit()`和`_exit()`的区别

exit和_exit就是用来正常终止一个进程的，主要区别是_exit会立刻进入内核，而exit先执行一些清除工作（包括执行各种终止处理程序，关闭所有标准I／O等，一旦关闭了IO，例如Printf等函数就不会输出任何东西了），然后才进入内核。这两个函数会对父子进程有一定的影响，当用vfork创建子进程时，子进程会先在父进程的地址空间运行（这跟fork不一样），如果子进程调用了exit就会把父进程的IO给关掉。


这两个函数都带一个参数表示终止状态，这跟我们平时写的return效果是一样的，如果不返回一个终止状态，那表示这个进程的终止状态就是未定义的。


### 进程终止有5种方法：

1. 正常终止

	- 从main函数return返回
	- 调用exit
	- 调用_exit

2. 异常终止
   - 调用abort
   - 由一个信号来终止 

 



---

exit（）

函数名: exit()

所在头文件：stdlib.h(如果是”VC6.0“的话头文件为：windows.h)

功 能: 关闭所有文件，终止正在执行的进程。

exit(0)表示正常退出，

exit(x)（x不为0）都表示异常退出，这个x是返回给操作系统（包括UNIX,Linux,和MS DOS）的，以供其他程序使用。

stdlib.h: void exit(int status);//参 数status，程序退出的返回值

_exit()

    直接使进程终止运行，清除其使用的内存空间，并销毁其在内核中的各种数据结构。

return（）

    return 表示从被调函数返回到主调函数继续执行，返回时可附带一个返回值，由return后面的参数指定。 return通常是必要的，因为函数调用的时候计算结果通常是通过返回值带出的。 如果函数执行不需要返回计算结果，也经常需要返回一个状态码来表示函数执行的顺利与否（-1和0就是最常用的状态码），主调函数可以通过返回值判断被调函数的执行情况。

 


exit()和return的区别：

1.exit用于结束正在运行的整个程序，它将参数返回给OS，把控制权交给操作系统；而return 是退出当前函数，返回函数值，把控制权交给调用函数。如果main()在一个递归程序中，exit()仍然会终止程序；但return将控制权移交给递归的前一级，直到最初的那一级，此时return才会终止程序。

2.exit是系统调用级别，它表示一个进程的结束；而return 是语言级别的，它表示调用堆栈的返回。

3. 在main函数结束时，会隐式地调用exit函数，所以一般程序执行到main()结尾时，则结束主进程。exit将删除进程使用的内存空间，同时把错误信息返回给父进程。

return和exit()的另一个区别在于，即使在除main()之外的函数中调用exit()，它也将终止程序。

_exit()与exit的区别：

头文件：

exit:#include<stdlib.h>

_exit:#include<unistd.h>

_exit()函数:直接使进程停止运行,清除其使用的内存空间,并销毁其在内核中的各种数据结构;

exit()函数则在这些基础上作了一些包装,在执行退出之前加了若干道工序。

exit()函数与_exit()函数最大的区别就在于 exit()函数在调用 exit 系统调用之前要检查文件的打开情况,把文件缓冲区中的内容写回文件。

退出过程
　　1.调用atexit()注册的函数（出口函数）；按ATEXIT注册时相反的顺序调用所有由它注册的函数,这使得我们可以指定在程序终止时执行自己的清理动作.例如,保存程序状态信息于某个文件,解开对共享数据库上的锁等.
　　2.cleanup()；关闭所有打开的流，这将导致写所有被缓冲的输出，删除用TMPFILE函数建立的所有临时文件.
　　3.最后调用_exit()函数终止进程。
　　_exit做3件事（man）：
　　1，Any open file descriptors belonging to the process are closed
　　2，any children of the process are inherited by process 1, init
　　3，the process's parent is sent a SIGCHLD signal
　　exit执行完清理工作后就调用_exit来终止进程。
————————————————
版权声明：本文为CSDN博主「Mr-Lr」的原创文章，遵循CC 4.0 BY-SA版权协议，转载请附上原文出处链接及本声明。
原文链接：https://blog.csdn.net/lb543210/article/details/81435694