function naive_traverse(input)
	local xyz = input
	local i = 0
	return function ()
	::NEXT_TURN::
		i = i + 1
		if i > #xyz then
			if #xyz == 0 then
				return nil
			else
				xyz = string.sub(xyz, 2)
				i = 0
				goto NEXT_TURN
			end
			i = 0
			xyz = string.sub(xyz, 1, #xyz - 1)
			goto NEXT_TURN
		end
		j = #xyz - i + 1
		return string.sub(xyz, 1, j)
	end
end
