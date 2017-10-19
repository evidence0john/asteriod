local x = require('fcgi')
str = 'aaaaaaa <img dddddd>'



--str = x.tagRemove(str)
--print(str)
local imgi, imgj = string.find(str, '<img.+>')
if imgi then
end
print(imgi)
--print(string.sub(str, string.find(str, '<img.+>')))