local index = new(Accept)
echo('<title>管理</title>')
---------------head---------------
echo(load_small_file(Global.spath..'head.html'))
---------------head---------------
MyArticle:select_db()

for i = MyArticle.article_IDC, 1, -1 do
	local head, art = MyArticle:read_article(i)
	--print(head)
	if head ~= 'Deleted' then
		local title, editor, time, slen = MyArticle:head_info(head)
	end
	echo('<a href=\"article.lua?', i, '\">')
	echo(title, '</a>')
	echo('&nbsp'..editor..'&nbsp'..os.date('%x', time))
	print(i)
end

---------------foot---------------
print(string.rep('<br>', 3))
echo(load_small_file(Global.spath..'foot.html'))
---------------foot---------------