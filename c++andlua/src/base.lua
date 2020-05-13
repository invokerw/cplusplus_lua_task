
local g_thread_stack = {}
local g_other_thread_set = {} --set, not map
local g_other_thread_set_adding = {} --set, not map
local g_log_index = 0

------------------------------------------------------------------------------------------------------------------------
--ÒÔÏÂº¯Êý½ö¹©³ÌÐòÔÚC++´úÂëÖÐµ÷ÓÃ
function _Start(ptr)
	--Log("-------------------------- _Start --------------------------------")
	g_ptr = ptr
	g_thread_stack[#g_thread_stack+1] = coroutine.create(main)
	--first run
	--_Run(ptr)
end

function _Run(ptr) --RunºÍTick²¢²»Í¬
	--Log("-------------------------- _Run --------------------------------")
	g_ptr = ptr
	--merge
	for t,_ in pairs(g_other_thread_set_adding)
	do
		g_other_thread_set[t] = true
	end
	g_other_thread_set_adding = {}
	--for main thread
	while #g_thread_stack>=1 and coroutine.status(g_thread_stack[#g_thread_stack])=="dead"
	do
		g_thread_stack[#g_thread_stack] = nil
	end
	if #g_thread_stack>=1 then
		local r,desc = coroutine.resume(g_thread_stack[#g_thread_stack])
		--if not r then error(desc) end
		if not r then error(debug.traceback(g_thread_stack[#g_thread_stack],desc)) end
	end
	--for other thread
	for t,_ in pairs(g_other_thread_set)
	do
		if coroutine.status(t)=="dead" then
			g_other_thread_set[t]=nil
		else
			local r,desc = coroutine.resume(t)
			--if not r then error(desc) end
			if not r then error(debug.traceback(t,desc)) end
		end
	end
end

function _OnMessage(ptr, msg, arg, src)
	if on_message==nil then return end
	g_ptr = ptr
	g_thread_stack[#g_thread_stack+1] = coroutine.create(on_message)
	local r,desc = coroutine.resume(g_thread_stack[#g_thread_stack], msg, src, arg)
	--if not r then error(desc) end
	if not r then error(debug.traceback(g_thread_stack[#g_thread_stack],desc)) end
end

function _OnScriptMessage(ptr, msg, arg, src)
	if on_message==nil then return end
	g_ptr = ptr
	g_thread_stack[#g_thread_stack+1] = coroutine.create(on_message)
	local r,desc = coroutine.resume(g_thread_stack[#g_thread_stack], msg, src, _deserialize(arg))
	--if not r then error(desc) end
	if not r then error(debug.traceback(g_thread_stack[#g_thread_stack],desc)) end
end

function _serialize(obj)
	local lua = ""
	local t = type(obj)
	if t == "number" then
		lua = lua .. obj
	elseif t == "boolean" then
		lua = lua .. tostring(obj)
	elseif t == "string" then
		lua = lua .. string.format("%q", obj)
	elseif t == "table" then
		lua = lua .. "{"
		for k, v in pairs(obj) do
	   		lua = lua .. "[" .. _serialize(k) .. "]=" .. _serialize(v) .. ","
		end
		lua = lua .. "}"
	elseif t == "nil" then
		return nil
	else
		error("can not _serialize a " .. t .. " type.")
	end
	return lua
end

function _deserialize(lua)
	local t = type(lua)
	if t == "nil" or lua == "" then
		return nil
	elseif t == "number" or t == "string" or t == "boolean" then
		lua = tostring(lua)
	else
		error("can not _deserialize a " .. t .. " type.")
	end
	lua = "return " .. lua
	local func = loadstring(lua)
	if func == nil then
		return nil
	end
	local tb = func();
	if type(tb) ~= "table" then
		return nil
	end
	return tb
end



function _AddTraceInfo(info) --TODO: º¯ÊýÃû×Ö¸ÄÏÂ
	return info..debug.traceback("",2)
end

function _DumpTable(ot, t)
	for k,v in pairs(t) do
		if type(v)=="table" then 
			ot[#ot+1] = k.."={"
			_DumpTable(ot, v)
			ot[#ot+1] = "};" 
		elseif type(v)=="userdata" then 
			ot[#ot+1] = k.."=USERDATA;"
		elseif type(v)=="boolean" then 
			ot[#ot+1] = k.."="..tostring(v)..";"
		elseif type(v)=="function" then 
			ot[#ot+1] = k.."="..tostring(v)..";"
		else 
			ot[#ot+1] = k.."="..v..";"
		end  
	end  
end

------------------------------------------------------------------------------------------------------------------------
--ÒÔÏÂº¯Êý½ö¹©³ÌÐòÔÚlua´úÂëÖÐµ÷ÓÃ
function Nop()
	coroutine.yield()
	return true
end

------------------------------------------------------------------------------------------------------------------------
--ÒÔÏÂº¯Êý²ß»®´úÂëÖÐ¿ÉÒÔµ÷ÓÃ
function Sleep(ms)
	local b = API_GetTimeMS()
	while Nop() do
		local now = API_GetTimeMS()
		if now-b>ms then break end
	end
	return true
end

--function THREAD_Switch(thread_entry)
--	--TODO: ¼ì²é£¬²»ÄÜÔÚcoroutineÖÐµ÷ÓÃ
--	--TODO: Èç¹ûcoroutineÖÐµ÷ÓÃ£¬Ö±½ÓÖ´ÐÐthread_entry¼´¿É
--	g_thread_stack[#g_thread_stack+1] = coroutine.create(thread_entry)
--end

function THREAD_New(thread_entry, arg)
	local t = coroutine.create(thread_entry)
	g_other_thread_set_adding[t] = true
	local r,desc = coroutine.resume(t, arg)
	--if not r then error(desc) end
	if not r then error(debug.traceback(t,desc)) end
end

function Log(log)
	g_log_index = g_log_index+1
	log = "g_log_index="..g_log_index.." "..log
	API_Log(g_ptr, log)
end

function GetTimeMS()
	return API_GetTimeMS()
end

function DumpTable(t)
	local ot = {} 
	_DumpTable(ot, t)
	return table.concat(ot)
end

