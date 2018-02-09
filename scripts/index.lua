local index = new(Accept)
echo('<title>evi.fun</title>')
counter_inc(index.script)
echo(load_small_file(Global.spath..'head.html'))
echo(load_small_file(Global.spath..'home.html'))

MyArticle:select_db()


for i = MyArticle.article_IDC, 1, -1 do
	local head, art = MyArticle:read_article(i)
	--print(head)
	if head ~= 'Deleted' then
		local title, editor, time, slen = MyArticle:head_info(head)
		echo('<hr>')
		echo('<div id=\"absTitle\" class = \"apic\">')
		echo('<a href=\"article.lua?', i, '\">')
		local imgi, imgj = string.find(art, '<img.+>')
		echo('<img src=\"')
		if imgi then
			local tag = string.sub(art, imgi, imgj)
			imgi, imgj = string.find(tag, '\"[%w%p]+\"')
			echo(string.sub(tag, imgi + 1, imgj -1))
		else
			echo('asteroid.jpg')
		end
		echo('\">')
		echo('</a></div><div>')
		echo('<a href=\"article.lua?', i, '\">')
		echo('&nbsp', title,'</a>')
		echo('&nbsp'..editor..'&nbsp'..os.date('%x', time)..'<br>')
		local abs = string.sub(art, 1, 400)
		abs = tagRemove(abs)
		abs = string.sub(abs, 1, 250)
		if string.byte(string.sub(abs, #abs)) > 127 then
			abs = string.sub(abs, 1, #abs - 1)
		end
		print('<p>', abs, '...</p>')
		echo('</div>')
	end
end
echo(load_small_file(Global.spath..'foot.html'))
