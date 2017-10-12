--	Hello word Example:
	--创建新的 Accept 对象
	local index = new(Accept)
	--Hello world
	print('Hello world')
	if index.postlen == 0 then
		print('Post Nothing')
	else
		word = index:getPostByName('say')
		if word then
			print('You said : ', word)
		else
			print('Something wrong')
		end
	end
	--输出 Accept 对象的基本信息
	index:info()
	
	