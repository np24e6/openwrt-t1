local lua_lpac = require("lua_lpac")
local parse = require("luci.jsonc").parse

-- Wrapper to parse json and do basic validation.
return setmetatable({}, {
    __index = function (_, k)
        local f = rawget(lua_lpac, k)
        if not f then return end
        return function (...)
            local ok, err = f(...)
            if type(ok) ~= "string" or err then return nil, err end
            return parse(ok) or {}
        end
    end
})