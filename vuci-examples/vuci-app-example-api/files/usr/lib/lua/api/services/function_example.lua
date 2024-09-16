local FunctionService = require("api/FunctionService")

local Example = FunctionService:new()

-- GET /api/example_f/test
function Example:GET_TYPE_test()
	os.execute("sleep 2")
	return self:ResponseOK({
		result = "API result example"
	})
end

function Example:TestAction()
	-- For error handling use error functions:
	--      self:add_critical_error(100, "Example error message", "Source")
	--      self:add_error(100, "Example error message", "Source")

	return self:ResponseOK({
		result = "API action example",
		data = self.arguments.data
	})
end

-- POST /api/example_f/actions/test
local test_action = Example:action("test", Example.TestAction)

	local name = test_action:option("name")
		name.require = true
		name.maxlength = 256

	local custom = test_action:option("custom")
		function custom:validate(value)
			-- Here you can write a custom validation for this field
			if value == "value error" then
				return false, "Example error message"
			end
			return true
		end

function Example:UPLOAD_init()
	local location = "/tmp/%s"

	local function set_filename(name)
		local file_path = string.format(location, name)
		return file_path
	end

	-- Callbacks:
	--     on_boundry_complete
	--     on_value_complete
	--     on_content_type_complete
	--     on_content_disposition_complete
	--     on_filename_complete
	--     set_filename
	--     get_old_location
	--     set_file_size

	local callbacks = {
		set_filename = set_filename
	}

	-- Return:
	--      File save location
	--      Table for UPLOAD_after_upload_hook
	--      Callbacks
	--      Maximum file size
	return nil, nil, callbacks
end

function Example:UPLOAD_after_upload_hook(path, table)
	-- Do something after upload, must return file path
	return { path = path }
end

return Example
