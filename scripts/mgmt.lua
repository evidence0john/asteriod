local index = new(Accept)
echo('<title>管理</title>')
---------------head---------------
echo(load_small_file(Global.spath..'head.html'))
---------------head---------------
echo(load_small_file(Global.spath..'mgmt.html'))
MyArticle:select_db()
print('<br>当前记录器使用率：', MyArticle.article_IDC..'/', ltls.info(4))
echo('<h2>文章列表：</h2>')
print('Delete&nbsp&nbsp&nbspTitle&nbsp&nbsp&nbsp&nbspEditor&nbsp&nbsp&nbsp&nbspTime')
for i = MyArticle.article_IDC, 1, -1 do
	local head, art = MyArticle:read_article(i)
	--print(head)
	if head ~= 'Deleted' then
		local title, editor, time, slen = MyArticle:head_info(head)
		echo('<input type=\"checkbox\" class=\"mgmtleft\" name=\"', i,'\">')
		echo('<a href=\"article.lua?', i, '\" target=\"_blank\">')
		echo(title, '</a>')
		echo('&nbsp'..editor..'&nbsp'..os.date('%x', time))
		print()
	end
end
echo('<br><h2>回收站：</h2>')
for i = MyArticle.article_IDC, 1, -1 do
	local head, art = MyArticle:read_article(i)
	--print(head)
	if head == 'Deleted' then
		local title, editor, time, slen = MyArticle:head_info(head)
		echo('<a href=\"article.lua?', i, '\" target=\"_blank\">')
		echo(title, '</a>')
		echo('&nbsp'..editor..'&nbsp'..os.date('%x', time))
	end
end

---------------foot---------------
print(string.rep('<br>', 3))
echo(load_small_file(Global.spath..'foot.html'))
---------------foot---------------