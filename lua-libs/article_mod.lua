--[[
-------------384
NAME--256 TIME--8 UID--4 LENGTH--4
-------------NoLimitation
TITLE\nUNAME\nTIME\nLENGTH\n
-------------13
FLAG--1 OFFSET--8 POINTER--4
-------------
]]

--[[
FLAG DEF:
0	nil
1	disk
2	cached
...
255 deleted
]]
----------------------------------

Article_Obj =
{
	['__class_type__'] = 'Article_Obj',
	--['fp']
	--['usr_buf']	
	['article_IDC'] = 0,
	['database_id'] = 0,
	['usr_rec_len'] = 13,
	['__init__'] =
	function (self, arg)
		if arg then
			if arg.rec_len then
				self:set_rec_len(arg.rec_len)
			end
		end
	end,
	['set_rec_len'] =
	function (self, len)
		self.usr_rec_len = len
	end,
	['set_db_id'] =
	function (self, id)
	 	self.database_id = id
	end,
	['select_db'] =
	function (self, id)
		if id then
			ltls.select(id)
		else
			ltls.select = self.database_id
		end
	end,
	['set_flag'] =
	function (self, flag, id)
		ltls.write(string.char(flag), id, 1)
	end,
	['get_flag'] =
	function (self, id)
		return string.byte(ltls.read(id, 1))
	end,--[[CID = cache ID]]
	['set_CID'] =
	function (self, CID, id)
		ltls.write(int32(CID), id, 4, 9)
	end,
	['get_CID'] =
	function (self, id)
		return toint32(ltls.read(id, 4, 9))
	end,
	['set_record'] =
	function (self, head, body, id)
		offset = StringToDisk(self.fp, head)
		StringToDisk(self.fp, body)
		pointer = self.usr_buf:add(head, body, id)
		while pointer == 0 do --SEC
			rmid = self.usr_buf:remove(1)
			self:set_flag(1, rmid)
			pointer = self.usr_buf:add(head, body, id)
		end
		--print('write offset = ', offset)
		rec = string.char(2)..int64(offset)..int32(pointer)
		ltls.write(rec, id, self.usr_rec_len)
	end,
	['cache'] =
	function (self, id)
		if self:get_flag() == 2 then
			return self:get_CID(id) 
		end
		head, body = self:load_Disk(toint64(ltls.read(id, 8, 1)))
		pointer = self.usr_buf:add(head, body, id)
		while pointer == 0 do
			rmid = self.usr_buf:remove(1)
			self:set_flag(1, rmid)
			pointer = self.usr_buf:add(head, body, id)
		end
		self:set_CID(pointer, id)
		self:set_flag(2, id)
	end,
	['head_info'] =
	function (self, head)
		pos = string.find(head, '\n')
		title = string.sub(head, 1, pos)
		pose = string.find(head, '\n', pos + 1)
		editor = string.sub(head, pos, pose)
		pos = string.find(head, '\n', pose + 1)
		time = string.sub(head, pose, pos)
		slen = string.sub(head, pos)
		return title, editor, time, slen
	end,
	['load_Disk'] =
	function (self, offset)
		bpos = self.fp:seek()
		self.fp:seek('set', offset)
		title = self.fp:read('l')
		editor = self.fp:read('l')
		time = self.fp:read('l')
		slen = self.fp:read('l')
		len = tonumber(slen)
		body = self.fp:read(len)
		self.fp:seek('set', bpos)
		return title..'\n'..editor..'\n'..time..'\n'..slen..'\n', body
	end,
	['Set'] =
	function (self, mod, arg)
		if mod == 'new' then
			print('create')
			ltls.select(arg.id)
			ltls.create(self.usr_rec_len, 0, arg.capacity)
			ltls.name(arg.tlsname)
			self:Set_fp(arg.dsrc)
			self:Set_buf()
			self.article_IDC = 0
			ltls.write(int32(0), 0, 4, 0)
			ltls.write(int32(self.article_IDC), 0, 4, 4)
			print('Created new article manager')
		elseif mod =='load' then
			self.database_id = arg.id
			ltls.select(self.database_id)
			ltls.import(arg.tlspath)
			self:Set_fp(arg.dsrc)
			self:Set_buf()
			self.article_IDC = toint32(ltls.read(0, 4, 4))
			for i = 1, self.article_IDC do
				if self:get_flag(i) == 2 then
					self:set_flag(1, i)
				end
			end
			print('Imported article manager')
		end
	end,
	['Set_fp'] =
	function (self, path)
		if self.fp == nil then
			self.fp = OpenDiskSource(path..'.dsrc')
		end
	end,
	['Set_buf'] =
	function (self)
		if self.usr_buf == nil then
			self.usr_buf = new(Simple_buffer)
		end
	end,
	['delete_article'] =
	function (self, id)
		self:set_flag(255, id)
	end,
	['read_article'] =
	function (self, id)
		flag = string.byte(ltls.read(id, 1))
		if flag == 255 then
			return 'Deleted'
			elseif flag == 1 then
				--print('On disk')
				self:cache(id)
				CID = self:get_CID(id)
				return self.usr_buf.head[CID], self.usr_buf.article[CID]
			elseif flag == 2 then
				--print('cached')
				CID = self:get_CID(id)
				return self.usr_buf.head[CID], self.usr_buf.article[CID]
		end
	end,
	['new_article'] =
	function (self, head, body)
		self.article_IDC = self.article_IDC + 1
		if self.article_IDC == ltls.info(4) then
			self.article_IDC = self.article_IDC -1
			return -1
		end
		self:set_record(head, body, self.article_IDC)
		ltls.write(int32(self.article_IDC), 0, 4, 4)
		return self.article_IDC
	end,
	['export'] =
	function (self)
		ltls.export()
	end,
	['__delete__'] =
	function (self)
		self.fp:close()
	end
}

--[[
local tmp = new(Article_Obj, {
			['rec_len'] = 13
		}
	)

tmp:Set('load', {
		['id'] = 1,
		['tlspath'] = './article_p0',
		['dsrc'] = './article_p0',
	})
]]
--[[
tmp:Set('new', {
		['id'] = 1,
		['tlsname'] = 'article_p0',
		['dsrc'] = './article_p0',
		['capacity'] = 2048
	})
]]
--[[
print(tmp:new_article('this is title\nevi\n100000010\n12\n', 'hello world!'))

--print(toint64(ltls.read(1, 8, 1)))
--print('aidc = ', toint32(ltls.read(0, 4, 4)))
local head, art = tmp:read(tmp.article_IDC)
print(art)
]]
