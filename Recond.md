# 并发测试记录

* 1.connect一般都会正常返回,但是在accept端计数总是会低于理论值,例如20线程,每个线程1000连接,对accept返回次数计数的结果一般只能达到19600,达不到2w的值,这时将客户端关闭(ctrl+c),连接断开时,服务端的accept计数又会上升,但仍未能达到2w.即使降低并发连接数到200,最后停下计数时也只能达到180,当测试5w连接时,计数结果在低于端口极限值约1000-100不等处停止增加(未达到极限值),客户端connect返回错误:connect error:Cannot assign requested address


* 2.根据抓包结果当连接为2w时(100线程,每个线程200连接),在计数停下之前的所有包都是正常的,没有发现[F]或[R]包,当ctrl+c断开连接后,客户端发送[R](也有[F]包)包给服务器断开连接,但奇怪的是在[R]包中间能抓到服务器发到客户端的[P]包(服务器会在连接建立后发送256字节的包),因为客户端连接已关闭,自然会回服务端[R]包,计数未能达到2w(少100-5000不等).


* 3. ctrl+c 记录点 时间点:18:49:57.808767  136903行 (100线程,每个线程500连接) 在记录点前未发现[F]或[R]包,断开后也是[F][R]包交错着零星的[P]包,记录点后accept的计数器会增加少许.记录点后客户端开始返回错误:connect error:Cannot assign requested; 计数未能达到5w(计数结果在低于端口极限值约1000-100不等处停止,但ctrl+c后有很小几率达到3w超出端口极限值2w8)


* 4. ctrl+c 记录点 时间点:18:59:03.157592 143308行 (1000线程,每个线程500连接,2个同时进程开) 在记录点前仍未发现[F]或[R]包,断开后也是[F][R]包交错着零星的[P]包,记录点后accept的计数器会增加少许.记录点后客户端开始返回错误:connect error:Cannot assign requested; 计数未能达到5w(计数结果在低于端口极限值约1000-100不等处停止,但ctrl+c后有较大几率达到3w超出端口极限值2w8)


* 5. ctrl+c 记录点 时间点:20:10:10.504553 139772行 (1000线程,每个线程500连接,4个同时进程开) 在记录点前仍未发现[F]或[R]包,断开后也是[F][R]包交错着零星的[P]包,记录点后accept的计数器会增加少许.记录点后客户端开始返回错误:connect error:Cannot assign requested,当一个一个关闭进程时,能抓到新的[S]包和[P]包. 计数未能达到5w(计数结果在低于端口极限值约1000-100不等处停止,但ctrl+c后有很大几率达到3w超出端口极限值2w8)


* 6. ctrl+c 记录点 时间点:21:00:00.221800 186940行 (1000线程,每个线程500连接,3个同时进程开,服务端每accept 500个链接就sleep(5)),该实验模拟队满的情况,根据抓包结果,在20:56:15.777633 的时间点发现[F]包(且从10579行到11579行连续出现1000条连接发送[F])为客户端发送到服务端的.在ctrl+c前开始出现connect error:Connection timed out和connect error:Cannot assign requested. 计数未能达到5w(计数结果在低于端口极限值很多的地方停止,ctrl+c也没有多少增加)

** 疑问:长连接在程序没有close或shutdown或程序退出的情况下 为什么会主动发送fin包,(未设keepalive位等选项)

* 7. (1000线程,每个线程500连接,单个进程,无sleep, 由本地连接纽约主机) 时间点:21:54:45.499296 82389行 计数停止,很快就开始出现connect error:Connection timed out;  时间点21:57:08.517767 89680行 ctrl+c记录点, 在ctrl+c之前未发现[F]或[R]包,之后能抓到[R][F][FP][P]包,[FP]包未了解??.   accept返回计数远未能达到5w(计数结果大约1w4)

* 8. 	(1000线程,每个线程500连接,4个进程,无sleep, 由本地连接纽约主机)
		00:48:32.047471 65345行 计数暂停 
		00:51:54.595309	113599行 ctrl+c 3个 在此之前无[R][F]包,ctrl+c 3个后又能收到[S]

* 9. 开启syncookie,其他选项默认,首先客户大量发送syn,服务端不accept.syn包到达服务端,在服务端状态为SYN_RECV,服务端分配空间将它放入syn(半连接)队列,然后发送syn+ack,客户端收到后回ack,这时客户端就认为连接建立(establish),服务端收到ack,认为连接建立,establish状态链接+1,放入全连接对列,等待accept调用.因为accept不调用,全连接队列自然要满,但是最先满的一般是半连接队列.一旦半连接队列满,他会开启syncookie,不再分配内存给新链接,不为新连接保存状态,syncookie的syn+ack一旦丢失是不会有重传的,但他发送的仍是syn+ack,这就意味着客户端会认为自己的连接已经建立并发送ack.由于全连接队列的积累很快就满员,一旦全连接队列满员,他首先会扔掉新来的syn包,这时连syncookie都不会发送,客户端的一切syn包都被扔掉.然后针对自认为已经建立的客户端的ack包,只要全连接队列满就会把ack包扔掉,然后重发ack+syn直到超时(待抓包测试),注意对于syncookie的连接,服务端是不保持其信息的,这也意味着他可能不会重传这种类型的连接(待抓包测试).
