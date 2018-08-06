# asteroid

Native C program module supported, for lighter and faster web app. Embed local lua State, build web app via lua script.

asteroid 是一种轻型服务器架构，它支持原生的 C 语言程序模块，内嵌 lua 状态机，可以直接提供 lua 脚本支持。asteroid 使用 C 语言，保持了较好的平台兼容性，你可以在几乎所有 32 位计算机平台上编译使用。web 服务器部分，asteroid 使用了 [mongoose](https://cesanta.com/)，并把 mongoose 作为 asteroid 的一部分，原则上保持对 mongoose 官方的支持，即尽可能不对 mongoose 进行修改。此外，你需要安装 [lua](http://lua.org)，或者使用更高效的 [luajit](http://luajit.org)。部分简单的数据结构通过 [tinylist](https://github.com/evidence0john/tinylist) 实现。
