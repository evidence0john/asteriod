local post = new(Accept)
---------------head---------------
echo(load_small_file(Global.spath..'head.html'))
---------------head---------------
if post.postlen == 0 then
	echo(load_small_file(Global.spath..'newarticle.html'))
else
	local editor = post:getPostByName('editor')
	local cookie = post:getPostByName('cookie')
	if editor == nil or cookie == nil then
		print('User Info Error')
	else
		if Global.usr_cookie[editor] ~= cookie then
			print('User Info Error')
		else
			MyArticle:select_db()
			local time = os.time()
			local title = post:getPostByName('title')
			local Body = post:getPostByName('body')
			local slen = #Body
			local head = title..'\n'..editor..'\n'..time..'\n'..slen..'\n'
			local id = MyArticle:new_article(head, Body)
			if id == -1 then
				print('Error : Container Overflow')
			else
				echo('<a href=\"article.lua?', id, '\" target=\"_blank\">')
				echo(title, '</a>')
				echo('&nbsp'..editor..'&nbsp'..os.date('%x', time))
				MyArticle:export()
			end
		end
	end
end
---------------foot---------------
echo(load_small_file(Global.spath..'foot.html'))
---------------foot---------------
