local index = new(Accept)
MyArticle:select_db()
local reqID = index:Query()
---------------head---------------
echo(load_small_file(Global.spath..'head.html'))
---------------head---------------
local head, art = MyArticle:read_article(reqID)
local title, editor, time, slen = MyArticle:head_info(head)
echo('<title>'..title..'</title>')
echo('<center><h2>'..title..'</h2></center>')
print('<center><p>'..editor..os.date('%c', time)..'</p></center>')
print(art)
---------------foot---------------
print(string.rep('<br>', 3))
print('<strong><em>暂不支持评论</em></strong>')
echo(load_small_file(Global.spath..'foot.html'))
---------------foot---------------