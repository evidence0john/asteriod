local index = new(Accept)
echo('<title>管理</title>')
---------------head---------------
echo(load_small_file(Global.spath..'head.html'))
---------------head---------------
echo(load_small_file(Global.spath..'mgmt.html'))
MyArticle:select_db()

for i = 1, MyArticle.article_IDC do
	local head, art = MyArticle:read_article(i)
	if head ~= 'Deleted' then
		local title, editor, time, slen = MyArticle:head_info(head)
	end
	echo(title..'&nbsp'..editor..'&nbsp'..os.date('%c', time))
	print(i)
end

---------------foot---------------
print(string.rep('<br>', 3))
echo(load_small_file(Global.spath..'foot.html'))
---------------foot---------------