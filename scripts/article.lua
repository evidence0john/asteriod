local index = new(Accept)
MyArticle:select_db()
local reqID = index:Query()

echo(load_small_file(Global.spath..'head.html'))
print(reqID)

local head, art = MyArticle:read_article(reqID)

print(art)

print(string.rep('<br>', 10))



for i = 1, MyArticle.article_IDC do
	local head, art = MyArticle:read_article(i)
	if head ~= 'Deleted' then
		local title, editor, time, slen = MyArticle:head_info(head)
	end
	echo(title..'&nbsp'..editor..'&nbsp'..os.date('%c', time))
	print(i)
end



for k,v in pairs(Global) do
	print(k,v)
end
print('End')

echo(load_small_file(Global.spath..'foot.html'))