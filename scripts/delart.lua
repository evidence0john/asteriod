local post = new(Accept)
---------------head---------------
echo(load_small_file(Global.spath..'head.html'))
---------------head---------------
if post.postlen == 0 then
	print('User Info Error')
else
	print(post.postbuf)
end
---------------foot---------------
echo(load_small_file(Global.spath..'foot.html'))
---------------foot---------------
