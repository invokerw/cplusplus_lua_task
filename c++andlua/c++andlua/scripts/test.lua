


function main()
	ret = TASK_FindWindow("Notepad", "1.txt - 记事本")
	if not ret then
		Log("Find Window False")
	end
	ret = TASK_FindChildWindow("Edit", "")
	if not ret then
		Log("Find WindowEx False")
	end
	x = 0x41
	while Sleep(1000) do
		Log("TASK_KeyDown a")
		TASK_ChildKeyDown(x)
		--TASK_ChildKeyUp(x)
		x = x + 1
		if x == 0x5A then
			break;
		end
	end
	TASK_ChildKeyDown(VK_CONTROL)
	TASK_ChildKeyDown(0x53)
	TASK_Close()
end

function on_message(msg, src, arg)
end
