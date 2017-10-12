local post = new(Accept)

--post:info()
--print(post:Query())

--title, editor, time, slen


print('Time = ', os.time())
print(os.date('%c', 1507560908))
print('Title = ', post:getPostByName('title'))
print('Editor = ', post:getPostByName('editor'))
local Body = post:getPostByName('body')
print('Slen =', #Body)

print('Body = ', Body)

print()
print()
print()
print(post.postbuf)

print()
print()
print()
print()
print()
print()

for k,v in pairs(Global) do
	print(k,v)
end
