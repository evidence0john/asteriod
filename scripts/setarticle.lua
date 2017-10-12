if MyArticle then
	print('Clashed!')
else
	MyArticle = new(Article_Obj, {
			--['rec_len'] = 13
		})
	local myid = ltls.available()
	local myconf = {
			--common
			['id'] = 0,
			['dsrc'] = './article_p0',
			--craete
			['tlsname'] = 'article_p0',
			['capacity'] = 2048,
			--load
			['tlspath'] = './article_p0'
		}
	myconf.id = myid
	MyArticle:Set('load', myconf)
end