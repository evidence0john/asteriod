--set server port
port = '8000'
--native app settings
--[[
if native_lib is set, native app will be enabled, remove or comment
native_lib will disabled native app, and native_match and native_keyword
will be ignored while loading configurations
]]
native_lib = './demo.so'
native_match = 'prefix'
native_keyword = '/native/'
--local lua state settings
lState_match = 'prefix'
lState_keyword = '/lua/'
lState_init = './init.lua'
lState_home = '.'

--common http server settings, ref: mongoose http server options
document_root = '/home/evide/C0D1NG/asteroid_home/home/'
--document_root = '.'
index_files = 'index.html'
enable_directory_listing = 'yes'
cgi_file_pattern = ""
--cgi_interpreter =