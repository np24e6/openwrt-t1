local ConfigService = require("api/ConfigService")

local Example = ConfigService:new({
	-- delete = false,          -- Disable deletion of UCI sections
	-- create = false,          -- Disable creation of UCI sections
	-- general_section = "main",-- General UCI section name
	-- anonymous = true,        -- Create UCI anonymous sections
	-- increment_name = true,   -- Create UCI sections with numeric incremental names
})

local ConfigExample = Example:section(
	"example", -- UCI config name
	"example"  -- UCI section type
)
ConfigExample:make_primary()
ConfigExample.default_options.id.maxlength = 8 -- Default id option can also have validations
-- ConfigExample.order_by = "option" -- Order UCI config by provided option
-- ConfigExample.sort_response_by = "option" -- Order API response by provided option

function ConfigExample:create_defaults(sid)
	-- Default values to be added with every creation
	return {
		test = "default-value-" .. sid -- sid is the default_options id
	}
end

	local opt_text = ConfigExample:option("text")
		-- opt_text.cfg_require = true -- Option is required
		opt_text.maxlength = 100
		opt_text.minlength = 5

	local opt_bool = ConfigExample:option("bool")
		function opt_bool:validate(value)
			-- Here you can write a custom validation for this field
			return self.dt:is_bool(value)
		end

		function opt_bool:set(value)
			-- Here you can write a custom option set logic
			self:table_set(self.config, self.sid, "enabled", value)
		end

		function opt_bool:get()
			-- Here you can write a custom option get logic
			return self:table_get(self.config, self.sid, "enabled")
		end

	local opt_select = ConfigExample:option("select")
		function opt_select:validate(value)
			return self.dt:check_array(value, { "first", "second" })
		end

	local opt_multi_select = ConfigExample:option("multi_select", { list = true })
		opt_multi_select.maxlength = 64

	local opt_test = ConfigExample:option("test")
		opt_test.readonly = true

-- Uploads, Actions, GET_TYPE are also possible and are the same as in function_example.lua file.

-----------------------------------------------HOOKS-----------------------------------------------

-- function Example:GET_init_hook()
-- end

-- function Example:GET_section_init_hook()
-- end

-- function Example:GET_validate_section_hook()
-- end

-- function Example:GET_after_data_hook(data)
-- end


-- function Example:POST_init_hook()
-- end

-- function Example:POST_validate_hook()
-- end

-- function Example:POST_before_commit_hook()
-- end

-- function Example:POST_after_commit_hook()
-- end

-- function Example:POST_section_init_hook()
-- end

-- function Example:POST_validate_section_hook()
-- end

-- function Example:POST_after_data_hook()
-- end


-- function Example:PUT_init_hook()
-- end

-- function Example:PUT_validate_hook()
-- end

-- function Example:PUT_before_commit_hook()
-- end

-- function Example:PUT_after_commit_hook()
-- end

-- function Example:PUT_section_init_hook()
-- end

-- function Example:PUT_validate_section_hook()
-- end

-- function Example:PUT_after_validate_section_hook()
-- end

-- function Example:PUT_after_data_hook()
-- end


-- function Example:DELETE_init_hook()
-- end

-- function Example:DELETE_validate_hook()
-- end

-- function Example:DELETE_before_commit_hook()
-- end

-- function Example:DELETE_after_commit_hook()
-- end

-- function Example:DELETE_section_init_hook()
-- end

-- function Example:DELETE_before_section_delete_hook()
-- end

-- function Example:DELETE_after_data_hook(data)
-- end

return Example
