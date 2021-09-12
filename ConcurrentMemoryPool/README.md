# 一、内存池灵魂三问
## 1.1 什么是内存池？
### <1> 池化技术
**池**是在计算机技术中经常使用的一种设计模式，其内涵在于：**将程序中需要经常使用的核心资源先申请出来，放到一个池内，由程序自己管理，这样可以提高资源的使用效率，也可以保证本程序占有的资源数量。** 经常使用的池技术包括==内存池==、==线程池==和==连接池==等，其中尤以内存池和线程池使用最多。

**通俗理解**:
>池化技术就是提前准备好大量的资源以备不时之需，通过增加对象的复用，减少对象创建、销毁的代价。

### <2> 内存池
内存池(Memory Pool) 是一种动态内存分配与管理技术。 通常情况下，程序员习惯直接使用 new、delete、malloc、free 等API申请分配和释放内存，这样导致的后果是：当程序长时间运行时，由于所申请内存块的大小不定，**频繁使用时会造成大量的内存碎片从而降低程序和操作系统的性能**。**内存池则是在真正使用内存之前，先申请分配一大块内存(内存池)留作备用，当程序员申请内存时，从池中取出一块动态分配，当程序员释放内存时，将释放的内存再放入池内，再次申请池可以 再取出来使用，并尽量与周边的空闲内存块合并。若内存池不够时，则自动扩大内存池，从操作系统中申请更大的内存池**。

## 1.2 为什么使用内存池？
上面说过频繁使用new、delete等API申请释放内存，造成大量内存碎片降低程序和系统的性能。因此，内存池主要解决两个问题：**内存碎片问题和申请效率的问题。**

### 1.2.1 内碎片 VS 外碎片
内存碎片就是一部分内存不能分配给程序员使用，而是以碎片的方式存在，这就是**内存碎片问题**。
1. 外碎片
假设系统依次分配了16byte、8byte、16byte、4byte，8byte。这时要分配一个24byte的空间，操作系统回收了两个16byte和一个8byte，总的剩余空间有40byte，但是却不能分配出一个连续24byte的空间，这就是**内存碎片问题**，更进一步来说我们把这种内存碎片称为**外碎片**。
![在这里插入图片描述](https://img-blog.csdnimg.cn/20210628102042552.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzQwMDc2MDIy,size_16,color_FFFFFF,t_70)
2. 内碎片
与之相对应的则是**内碎片**，内碎片是由于内存器分配的内存大小**大于**程序申请的内存大小。
![在这里插入图片描述](https://img-blog.csdnimg.cn/20210628103606139.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzQwMDc2MDIy,size_16,color_FFFFFF,t_70)

### 1.2.1 申请效率问题
程序频繁向操作系统申请内存必然导致效率的下降，举个栗子：

假设一学期的你生活费是6000块。

方式1：开学时6000块直接给你，自己保管，自己分配如何花。

方式2：每次要花钱时，联系父母，父母转钱。

同样是6000块钱，第一种方式的效率肯定更高，因为第二种方式跟父母的沟通交互成本太高了。

现在很多的开发环境都是多核多线程，在申请内存的场景下，必然存在激烈的锁竞争问题。所以这次我们实现的内存池需要考虑以下几方面的问题。

1. 内存碎片问题。
2. 性能问题。
3. 多核多线程环境下，锁竞争问题。  

在开发前首先了解TCMalloc的几个基本概念

# 二、TCMalloc基本概念
在了解下面的概念前，我们先达成一个共识，系统分给进程的一块内存是虚拟内存且线性分配的，为什么是虚拟内存？可以看下面这篇博客
[Linux概念——进程地址空间](https://blog.csdn.net/qq_40076022/article/details/114544984)
用一张图表示一下会更形象直观
![在这里插入图片描述](https://img-blog.csdnimg.cn/e9112751868847d0950b602f49ed2f06.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzQwMDc2MDIy,size_16,color_FFFFFF,t_70)
一个大块的空闲内存用一个指针ptr来标识内存的分配情况，每分出去一块内存就让指针后移，但是这种标识方法面临一个问题：**当分配的内存释放回来的时候该怎么分配？**

很简单，使用**链表将分出去的内存管理起来**，因此就有了FreeList

## 2.1 FreeList
FreeList其本质就是**一个大块内存被切分成若干定长大小的链表结点，最后链接起来**，注意FreeList和链表ListNode的区别：
- FreeList**没有Next指针**，不需要Next指针存放下一个结点的地址
- FreeList使用了**结点的前4/8字节**”(其实就是整块内存的前8字节)存放下一个节点的指针。
- 分配出去的节点，节点整块内存空间可以被复写(指针的值可以被覆盖掉)
![image-20210912215627710](C:\Users\高蒙\AppData\Roaming\Typora\typora-user-images\image-20210912215627710.png)

通过**FreeList可以将定长的小块内存管理起来**，为了适应64为OS下的指针大小，本项目默认最小的内存为8KB，方便分配和释放。

FreeList里的单个object大小

| 数组索引 | FreeLIst单个object的大小 |
| :------: | :----------------------: |
|    0     |            8b            |
|    1     |           16b            |
|    2     |           24b            |
|    3     |           32b            |
|   ...    |           ...            |
|   184    |           64k            |

![image-20210912215956938](C:\Users\高蒙\AppData\Roaming\Typora\typora-user-images\image-20210912215956938.png)

	// 控制在12.5%内的内碎片浪费
	// [1,128]					8byte对齐	     freelist[0,16)  1/8=12.5%
	// [129,1024]				16byte对齐		 freelist[16,72)  129/136=
	// [1025,8*1024]			128byte对齐	     freelist[72,128)
	// [8*1024+1,64*1024]		1024byte对齐     freelist[128,184)



## 2.2 Page

Page是TCMalloc管理内存的基本单位（这里的Page并不是操作系统的Page），Page的默认大小是8KB，在TCMalloc中可以调整为32K或64KB，

**注意**！

**本项目页的默认大小为==4KB==**。
![在这里插入图片描述](https://img-blog.csdnimg.cn/7c0c8f79f8c143ddacad54c84efd5b64.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzQwMDc2MDIy,size_16,color_FFFFFF,t_70)

## 2.3 Span概念

一个Span由n个Page构成，且构成Span的n个Page是连续的

![在这里插入图片描述](https://img-blog.csdnimg.cn/8cefb6ad7c754937a9171deeaf23ed41.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzQwMDc2MDIy,size_16,color_FFFFFF,t_70)

如图：
- 1个==Page==构成的4KB的==Span==
- 2个连续==Page==构成的8KB的==Span==
- 3个连续==Page==构成的12KB的==Span==

## 2.4 Spanlist概念

为了方便Span的管理，**Span和Span之间可以构成双向链表我们称之为SpanList**，内存管理中通常将持有相同数量Page的Span构成一个双向链表，如下图所示(N个持有1Page的Span构成的SpanList
![在这里插入图片描述](https://img-blog.csdnimg.cn/e8b14bf480da4d6aa85168da55ca88dc.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzQwMDc2MDIy,size_16,color_FFFFFF,t_70)

## 2.5 object概念

一个`Span`会被按照某个大小拆分为N个`Objects`，同时这N个`Objects`构成一个`FreeList`

我们以将`1Page`的`Span`为例，`Span`、`Page`、`Object`关系图示如下：

![image-20210912215015331](C:\Users\高蒙\AppData\Roaming\Typora\typora-user-images\image-20210912215015331.png)

## 5.Sizeclass概念



## 



# 三、总体框架

concurrent memory pool主要由以下3个部分构成：

1. thread cache：线程缓存是每个线程独有的，用于小于64k的内存的分配，线程从这里申请内存不需要加锁，每个线程独享一个cache，这也就是这个并发线程池高效的地方。

2. central cache：中心缓存是所有线程所共享，thread cache是按需从central cache中获取的对象。central cache周期性的回收thread cache中的对象，避免一个线程占用了太多的内存，而其他线程的内存吃紧。达到内存分配在多个线程中更均衡的按需调度的目的。central cache是存在竞争的，所以从这里取内存对象是需要加锁，不过一般情况下在这里取内存对象的效率非常高，所以这里竞争不会很激烈。

   ​	居中调度：

3. page cache：页缓存是在central cache缓存上面的一层缓存，存储的内存是以页为单位存储及分配的，central cache没有内存对象时，从page cache分配出一定数量的page，并切割成定长大小的小块内存，分配给central cache。page cache会回收central cache满足条件的span对象，并且合并相邻的页，组成更大的页，缓解内存碎片的问题。  

![image-20210912215216539](C:\Users\高蒙\AppData\Roaming\Typora\typora-user-images\image-20210912215216539.png)

# 1. threadcache

映射自由链表的哈希桶，尽量用原生的数据结构写，不用stl

多个线程抢占一个资源

<128

> 128



```
	//申请内存对象
	void* Allocate(size_t size);
	//释放内存对象
	void Deallocate(void* ptr, size_t size);
	//从中心缓存获取对象
	void* FetchFromCentralCache(size_t index, size_t size);
	//释放对象后，链表过长，将内存回收到Central Cache
	void ListTooLong(FreeList& list, size_t size);
	
	FreeList _freeLists[NLISTS]
```

![img](https://ask.qcloudimg.com/http-save/yehe-1093396/82pd2a0gdi.png?imageView2/2/w/1620)

# 1.SizeClass

对于小于64K的对象，我们统称为小对象分配，并将其划分为4个类别，我们将这种划分方式称为Size Class，通过size class可以找到对应的内存大小。Tcmalloc通过这种方式可以将内存碎片的浪费率控制在12.5%以内。为什么这么说呢？首先，Size class将64K划分为4个类别分别为：

- 8字节对齐，范围[1, 128] , 128/8 = 16
- 16字节对齐, 范围[129, 1024]
- 32字节对齐, 范围[1025, 8*1024]
- 1024字节对齐, 范围[8$*$1024+1，64$*$1024]

