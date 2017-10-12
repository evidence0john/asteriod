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

Global.usr_rec_len = 13

local article_p0 = 'article_p0'

--open DiskSource
if Global.article_fp_p0 == nil then
	--Global.article_fp_p0 = OpenDiskSource('just.test')
	Global.article_fp_p0 = OpenDiskSource(article_p0..'.dsrc')
end
--create buffer object
if Global.usr_buf == nil then
	Global.usr_buf = new(Simple_buffer)
end
------------------- goto end of this source---------------

function Article_set_flag(flag, id)
	ltls.write(string.char(flag), id, 1)
end

function Article_get_flag(id)
	return string.byte(ltls.read(id, 1))
end

function Article_set_CID(CID, id)
	ltls.write(int32(CID), id, 4, 9)
end

function Article_get_CID(id)
	return toint32(ltls.read(id, 4, 9))
end

function Article_set_record(fp, head, body, id)
	offset = AddToDisk(fp, head)
	AddToDisk(fp, body)
	pointer = Global.usr_buf:add(head, body, id)
	while pointer == 0 do --SEC
		rmid = Global.usr_buf:remove(1)
		Article_set_flag(1, rmid)
		pointer = Global.usr_buf:add(head, body, id)
	end
	rec = string.char(2)..int64(offset)..int32(pointer)
	ltls.write(rec, id, Global.usr_rec_len)
end

function Article_to_cache(fp, id)
	if Article_get_flag() == 2 then
		return Article_get_CID(id) 
	end
	head, body = Article_from_Disk(fp, toint64(ltls.read(id, 8, 1)))
	pointer = Global.usr_buf:add(head, body, id)
	while pointer == 0 do
		rmid = Global.usr_buf:remove(1)
		Article_set_flag(1, rmid)
		pointer = Global.usr_buf:add(head, body, id)
	end
	Article_set_flag(2, id)
end

function Article_read_record(fp, id)
	flag = string.byte(ltls.read(id, 1))
	if flag == 255 then
		return 'Deleted'
		elseif flag == 1 then
			--print('On disk')
			Article_to_cache(fp, id)
			CID = Article_get_CID(id)
			return Global.usr_buf.head[CID], Global.usr_buf.article[CID], Global.usr_buf.tls_id[CID]
		elseif flag == 2 then
			--print('cached')
			CID = Article_get_CID(id)
			return Global.usr_buf.head[CID], Global.usr_buf.article[CID], Global.usr_buf.tls_id[CID]
	end
	print('flag = ', flag)
end

function Article_head_info(head)
	pos = string.find(head, '\n')
	title = string.sub(head, 1, pos)
	pose = string.find(head, '\n', pos + 1)
	editor = string.sub(head, pos, pose)
	pos = string.find(head, '\n', pose + 1)
	time = string.sub(head, pose, pos)
	slen = string.sub(head, pos)
	return title, editor, time, slen
end

function Article_from_Disk(fp, offset)
	bpos = fp:seek()
	fp:seek('set', offset)
	title = fp:read('l')
	editor = fp:read('l')
	time = fp:read('l')
	slen = fp:read('l')
	len = tonumber(slen)
	body = fp:read(len)
	return title..'\n'..editor..'\n'..time..'\n'..slen..'\n', body
end

-----------------------------------------------------

function DEMO_Article_New(head, body)
	Global.article_IDC = Global.article_IDC + 1
	if Global.article_IDC == ltls.info(4) then
		Global.article_IDC = Global.article_IDC -1
		return -1
	end
	Article_set_record(Global.article_fp_p0, head, body, Global.article_IDC)
	ltls.write(int32(Global.article_IDC), 0, 4, 4)
	return Global.article_IDC
end

function DEMO_Article_read(id)
	return Article_read_record(Global.article_fp_p0, id)
end

local mod = 2

if mod == 1 then
	--article record by ltls -create mod
	ltls.select(1)
	ltls.create(Global.usr_rec_len, 0, 2048)
	ltls.name(article_p0)
	Global.article_IDC = 0
	ltls.write(int32(0), 0, 4, 0)
	ltls.write(int32(Global.article_IDC), 0, 4, 4)
	print('Created new article manager')
else
	--article record by ltls -load mod
	ltls.select(1)
	ltls.import(article_p0)
	Global.article_IDC = toint32(ltls.read(0, 4, 4))
	for i = 1, Global.article_IDC do
		if Article_get_flag(i) == 2 then
			Article_set_flag(1, i)
		end
	end
	print('Imported article manager')
end