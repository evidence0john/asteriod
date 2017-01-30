<meta charset="UTF-8">
# asteroid
<h2>欢迎加入 asteroid!</h2>
<p>asteroid 是一个轻型的计算工具集，提供小型可嵌入数据库、简单自然语言处理、CGI等方面的支持，核心部分主要使用 c/c++ 编写，大部分组件可以兼容在 windows 或 linux 等操作系统，某些组件甚至不需要操作系统。目前直接提供 lua 脚本语言的库的支持。你能够使用 asteroid 的组件建立多种应用：
<li>web 应用，比如一个 bbs，或者 blog，也能够承载一些线上交互式服务</li>
<li>APP 服务器，为移动客户端提供数据交互服务</li>
<li>IoT 应用，asteroid 拥有的数据库能够方便的与硬件进行交互，轻型高效，兼容嵌入式主机</li>
<li>游戏引擎，内嵌 asteroid 的数据库模块和其他组件加以图形音频接口，轻松实现 rpg，rts等类型的游戏</li></p>

<h3><li>一些简单的 Demo</li></h3>
<p>一个简单的站点：<a href = "http://openlic.tk:8080">http://openlic.tk:8080</a></p>
<h3><li>tinydb</li></h3>
<p>tinydb-lib <a href = "https://github.com/evidence0john/asteroid/tree/master/tinydb-lib">https://github.com/evidence0john/asteroid/tree/master/tinydb-lib</a></p>
<p>tinydb 是以 tinylist 数据表为基础建立的简单数据库应用。在 asteroid 中作为一种最基本，最常用的数据库方式使用。在 Windows 或 linux 操作系统下，tinydb 直接提供的 IPC 方式是通过直接共享内存实现的，如果你需要使用 socket 连接并操作 tinydb，参考 socket 扩展的相关代码。</p>

<p>tinydb 的核心 tinylist 是 lbrdb 模型的简单实现。tinydb 能够方便的嵌入到你的程序中，成为你的程序内部的数据库，也能独立编译，提供独立的数据服务。你会发现，这是一种小型简单的数据模型，占用极少的资源，拥有不错的效率，非常适合在资源并不充裕的计算平台上编译使用。</p>
<p>得益于共享内存的 IPC 方式，tinydb 能够允许多个进程对其进行实时的并行操作，为更复杂的数据应用和维护规则等提供了便利。</p>

<p>目前没有 tinydb 的文档，lbrdb_model 可以作为参考<a href = "https://github.com/evidence0john/lbrdb_model">https://github.com/evidence0john/lbrdb_model</a></p>

<p>tinydb-lua-lib 是 tinydb 的 lua 扩展：<a href = "https://github.com/evidence0john/asteroid/tree/master/tinydb-lua-lib">https://github.com/evidence0john/asteroid/tree/master/tinydb-lua-lib</a></p>
