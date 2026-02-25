free rtos:
其实还是单线程，要想从本质上多线程处理任务，还是得多核芯片，freertos只是模拟了一个多线程而已



# freertos中的一个线程的延时函数执行时，CPU会跑去执行别的任务，而Arduio的delay运行时会占用CPU资源
![[Pasted image 20251201154141.png]]
![[Pasted image 20251201154513.png]]



# 对于当前任务来说叫：阻塞（判断消息队列）
![[Pasted image 20251201155459.png]]