Clone =
function (tSrc)
	local t = {}
	for k,v in pairs(tSrc) do
		t[k] = v
	end
	if getmetatable(tSrc) then
		setmetatable(t, getmetatable(tSrc))
	end
	return t
end

ReadOnly =
function (t)
	local proxy = {}
	setmetatable(proxy, 
		{
			['__index'] = t,
			['__newindex'] =
			function ()
				return
			end
		})
	return proxy
end

IsInstance = 
function (t)
	return t.__instance__
end

--new from a instance
new_fi =
function (tSrc, ...)
	t = Clone(tSrc)
	t.__instance__ = true
	if t.__init__ then
		t:__init__(...)
	end
	return t
end
New_fi = new_fi

new =
function (tSrc, ...)
	if tSrc.__instance__ then
		return 'Cannot new from a instance'
	else
		t = Clone(tSrc)
		t.__instance__ = true
	end
	if t.__init__ then
		t:__init__(...)
	end
	return t
end
New = new

delete =
function (t, ...)
	if not t.__instance__ then
		return false
	end
	if t.__delete__ then
		t:__delete__(...)
	end
	return true
end
Delete = delete

--//get '__class_type__'
Ctype = 
function (obj)
	return obj.__class_type__
end

Merge = 
function (tR, tE, NTF)
	if NTF then
		t = Clone(tR)
	else
		t = tR
	end
	for k,v in pairs(tE) do
		t[k] = v
	end
	return t
end

Super = Merge

Inherit = 
function (tR, tE, NTF)
	if NTF then
		nt = Clone(tR)
	else
		nt = tR
	end
	local proxy = {}
	setmetatable(proxy, 
			{
				['__newindex'] = 
				function (t, k, v)
					if not tR[k] then
						nt[k] = v
					end
				end
			})
	Merge(proxy, tE)
	if NTF then
		Merge(nt, tR)
	end
	return nt
end